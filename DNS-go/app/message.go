package main

type Header struct {
	ID      uint16
	Flags   uint16
	QDCount uint16
	ANCount uint16
	NSCount uint16
	ARCount uint16
}

type Question struct {
	qname  string
	qtype  uint16
	qclass uint16
}

type Answer struct {
	aname    string
	atype    uint16
	aclass   uint16
	TTL      uint32
	RDLENGTH uint16
	RDATA    string
}

type Message struct {
	Header    *Header
	Questions []Question
	Answers   []Answer
}
