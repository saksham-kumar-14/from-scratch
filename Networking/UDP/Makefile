all: server client

server: server.c
	gcc server.c -o server

client: client.c
	gcc client.c -o client

run-server:
	./server

run-client:
	./client

clean:
	rm -rf output.txt ./server ./client

.PHONY: all run-server run-client clean
