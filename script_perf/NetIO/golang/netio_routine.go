package main

import (
	"flag"
	"fmt"
	"net"
	"os"
	"runtime"
	"time"
)

var response = []byte("HTTP/1.0 200 OK\r\nCache-Control: no-cache\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body><h1>200 OK</h1>\nHello world.\n</body></html>\n")
var rps = 0

func main() {
	port := flag.String("port", "8081", "Specify the port")
	cores := flag.Uint("cores", 1, "Specify the CPU cores using")
	flag.Parse()
	
	/*go 1.8 after,No settings required runtime.GOMAXPROCS */
	runtime.GOMAXPROCS(int(*cores))

	addr := "0.0.0.0:" + *port
	masterSock, err := net.Listen("tcp4", addr)
	if err != nil {
		fmt.Printf("Fail to listen %s: %s\n", *port, err.Error())
		os.Exit(1)
	}
	defer masterSock.Close()

	fmt.Println("Start listening the port: ", *port)

	go rpsStats()

	for {
		conn, err := masterSock.Accept()
		if err != nil {
			fmt.Println("Fail to accept: ", err.Error())
			continue
		}
		go worker(conn)
	}
}

func worker(conn net.Conn) {
	defer conn.Close()
	var buf [1024]byte
	
	/* Maybe a little better than  buf := make([]byte,1024) ,but it's not the best way to deal with it */
	bytes, err := conn.Read(buf[0:])
	if err != nil {
		return
	}

	if bytes > 0 {
		rps++
		_, err = conn.Write(response)
	}
}

func rpsStats() {
	for {
		time.Sleep(1000 * time.Millisecond)
		fmt.Printf("Total: %d RPS\n", rps)
		rps = 0
	}
}
