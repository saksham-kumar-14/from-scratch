package main

import (
	"encoding/binary"
	"net"
	"strings"
	"time"
)

const (
	flagQR uint16 = 1 << 15
	maskOpCode uint16 = 0b1111 << 11
	flagAA     uint16 = 1 << 10
	flagTC     uint16 = 1 << 9
	flagRD     uint16 = 1 << 8
	flagRA     uint16 = 1 << 7
	maskZ       uint16 = 0b111 << 4
	maskRCODE   uint16 = 0b1111
)

// Header
func InitHeader(req []byte) *Header {
	h := &Header{
		ID:      binary.BigEndian.Uint16(req[0:2]),
		QDCount: binary.BigEndian.Uint16(req[4:6]),
		NSCount: 0,
		ARCount: 0,
	}

	reqFlags := binary.BigEndian.Uint16(req[2:4])
	h.setHeaderFlags(reqFlags)

	return h
}

func (h *Header) setHeaderFlags(reqFlags uint16) {
	opcode := (reqFlags >> 11) & 0x0F
	rd := (reqFlags >> 8) & 0x01

	var rcode uint16 = 0
	if opcode != 0 {
		rcode = 4
	}

	h.Flags = flagQR | flagRA | (opcode << 11) | (rd << 8) | rcode
}


// Question
func InitQuestion(req []byte, name string, next int) Question {
	return Question{
		qname:  name,
		qtype:  binary.BigEndian.Uint16(req[next : next+2]),
		qclass: binary.BigEndian.Uint16(req[next+2 : next+4]),
	}
}

func EncodeName(name string) []byte {
	var ans []byte
	parts := strings.Split(name, ".")
	for _, part := range parts {
		ans = append(ans, byte(len(part)))
		ans = append(ans, part...)
	}
	ans = append(ans, 0)
	return ans
}

func DecodeName(req []byte, offset int) (string, int) {
	var parts []string
	curr := offset
	jumped := false
	next := -1

	for {
		b := req[curr]
		if b&0xC0 == 0xC0 {
			if !jumped {
				next = curr + 2
			}
			pointer := int(binary.BigEndian.Uint16(req[curr:curr+2]) & 0x3FFF)
			curr = pointer
			jumped = true
			continue
		}

		if b == 0 {
			curr++
			break
		}

		curr++
		label := string(req[curr : curr+int(b)])
		parts = append(parts, label)
		curr += int(b)
	}

	if !jumped {
		next = curr
	}
	return strings.Join(parts, "."), next
}

// Answer
func InitAnswer(name string, resBuf []byte, start int) Answer {
	if resBuf == nil {
		return Answer{
			aname:    name,
			atype:    1,
			aclass:   1,
			TTL:      60,
			RDLENGTH: 4,
			RDATA:    "8.8.8.8",
		}
	}

	ansName, next := DecodeName(resBuf, start)
	return Answer{
		aname:    ansName,
		atype:    binary.BigEndian.Uint16(resBuf[next : next+2]),
		aclass:   binary.BigEndian.Uint16(resBuf[next+2 : next+4]),
		TTL:      binary.BigEndian.Uint32(resBuf[next+4 : next+8]),
		RDLENGTH: binary.BigEndian.Uint16(resBuf[next+8 : next+10]),
		RDATA:    net.IP(resBuf[next+10 : next+14]).String(),
	}
}

func forwardQuery(header *Header, q Question, resolverAddr string) Answer {
	rd := (header.Flags >> 8) & 0x01
	queryFlags := rd << 8

	msg := &Message{
		Header: &Header{
			ID:      header.ID, // Mirror original ID
			Flags:   queryFlags,
			QDCount: 1,
		},
		Questions: []Question{q},
	}

	encodedMsg := EncodeMessage(msg)

	remoteAddr, _ := net.ResolveUDPAddr("udp", resolverAddr)
	conn, _ := net.DialUDP("udp", nil, remoteAddr)
	defer conn.Close()

	// Prevent infinite blocking
	conn.SetReadDeadline(time.Now().Add(2 * time.Second))
	conn.Write(encodedMsg)

	resBuf := make([]byte, 512)
	n, _, err := conn.ReadFromUDP(resBuf)
	if err != nil {
		return InitAnswer(q.qname, nil, 0)
	}

	_, next := DecodeName(resBuf[:n], 12)
	start := next + 4

	return InitAnswer("", resBuf[:n], start)
}

// Message
func InitMessage(req []byte, resolverAddr string) *Message {
	header := InitHeader(req)
	var questions []Question
	var answers []Answer

	curr := 12 // DNS Header length
	for i := 0; i < int(header.QDCount); i++ {
		name, next := DecodeName(req, curr)
		q := InitQuestion(req, name, next)
		questions = append(questions, q)

		var ans Answer
		if resolverAddr != "" {
			ans = forwardQuery(header, q, resolverAddr)
		} else {
			ans = InitAnswer(name, nil, 0)
		}
		answers = append(answers, ans)

		curr = next + 4
	}

	header.ANCount = uint16(len(answers))
	return &Message{
		Header:    header,
		Questions: questions,
		Answers:   answers,
	}
}

func EncodeMessage(m *Message) []byte {
	b := make([]byte, 12)

	// Encode Header
	h := m.Header
	binary.BigEndian.PutUint16(b[0:2], h.ID)
	binary.BigEndian.PutUint16(b[2:4], h.Flags)
	binary.BigEndian.PutUint16(b[4:6], h.QDCount)
	binary.BigEndian.PutUint16(b[6:8], h.ANCount)
	binary.BigEndian.PutUint16(b[8:10], h.NSCount)
	binary.BigEndian.PutUint16(b[10:12], h.ARCount)

	// Encode Questions
	for _, q := range m.Questions {
		b = append(b, EncodeName(q.qname)...)
		qBuf := make([]byte, 4)
		binary.BigEndian.PutUint16(qBuf[0:2], q.qtype)
		binary.BigEndian.PutUint16(qBuf[2:4], q.qclass)
		b = append(b, qBuf...)
	}

	// Encode Answers
	for _, a := range m.Answers {
		b = append(b, EncodeName(a.aname)...)
		aBuf := make([]byte, 10)
		binary.BigEndian.PutUint16(aBuf[0:2], a.atype)
		binary.BigEndian.PutUint16(aBuf[2:4], a.aclass)
		binary.BigEndian.PutUint32(aBuf[4:8], a.TTL)
		binary.BigEndian.PutUint16(aBuf[8:10], a.RDLENGTH)
		b = append(b, aBuf...)
		ip := net.ParseIP(a.RDATA).To4()
		b = append(b, ip...)
	}

	return b
}
