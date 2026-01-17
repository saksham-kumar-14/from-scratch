all: server client

server:
	gcc server.c -o server

client:
	gcc client.c -o client

run-server:
	./server

run-client:
	./client

clean:
	rm -rf ./server ./client 127.0.0.1* *.enc

.PHONY: all run-server run-client clean
