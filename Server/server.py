#!/usr/bin/env python

import socket

TCP_IP = '192.168.43.114'
TCP_PORT = 5007
BUFFER_SIZE = 1024

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

conn, addr = s.accept()

print
('Connection address:', addr)
while 1:
    data = conn.recv(BUFFER_SIZE)
    if not data: break
    print
    ('recieved data:', data)
    data = data + '\0'
    conn.send(data)

conn.close()
