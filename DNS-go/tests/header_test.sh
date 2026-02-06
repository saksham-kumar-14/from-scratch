#!/bin/bash
echo "Starting Go DNS Server..."
go run ./app &

SERVER_PID=$!

sleep 2

# Run the dig command
echo "Sending DNS query..."
dig @127.0.0.1 -p 2053 +noedns example.com

# Kill the server after the test is done
echo "Shutting down server (PID: $SERVER_PID)..."
kill $SERVER_PID
