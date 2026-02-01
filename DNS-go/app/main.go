package main

import (
	"fmt"
	"net"
	"flag"
)

func main() {
	fmt.Println("Server is running")
	resolverAddr := flag.String("resolver", "", "The address of the upstream resolver")
	flag.Parse()

	udpAddr, err := net.ResolveUDPAddr("udp", "127.0.0.1:2053")
	if err != nil {
		return
	}

	udpConn, err := net.ListenUDP("udp", udpAddr)
	if err != nil {
		return
	}
	defer udpConn.Close()

	for {
		buf := make([]byte, 512)
		size, source, err := udpConn.ReadFromUDP(buf)
		if err != nil || size < 12 {
			continue
		}

		response := InitMessage(buf[:size], *resolverAddr)
		encodedResponse := EncodeMessage(response)
		udpConn.WriteToUDP(encodedResponse, source)
	}
}
