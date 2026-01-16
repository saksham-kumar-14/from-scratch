# POSIX C for client and server app that will use datagram socket
- Client and Server will interact in the following way:
  - The client first sends the file name to the server
  - On receiving the file name, the server checks if the file exists in the local directory or not
    - If the file is not available, the server sends a NOT_FOUND <FILE> message to the client and client displays "NOT FOUND" and exits.
    - If the file exists, the server reads the file line and sends it to the client
  - Upon receiving the first line, the client creates a new file to store the received words. It sends WORD_i messages to the server, starting with WORD_1 to request the first word.
  - For each WORD_i request, the server sends the corresponding word from the file to the client.
  This continues until the server sends the last word
  - Upon receiving the last word, the client writes it to the file, closes the file and exits. 
  The server also terminates
