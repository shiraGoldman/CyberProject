#!/usr/bin/env python
import socket

# message Packet
# {
#      required int32 seqNumber = 1;
#      required Opcode opcode = 2;
#      required bool isLast = 3;
#      required bytes content = 4;
#      required int len = 5
# }


FILE_PATH = "C:\\Users\\admin\\Downloads\\calc.exe"
EXE_STR = "netstat -a -n -o"
CHUNK_SIZE = 1024


def bin_file_to_buffer(file_path):
    """Lazy function (generator) to read a file piece by piece.
    Default chunk size: 1k."""
    with open(file_path, 'rb') as fp:
        while True:
            data1 = fp.read(CHUNK_SIZE)
            if not data1:
                break
            yield data1


if __name__ == '__main__':
    TCP_IP = '10.7.0.70'
    TCP_PORT = 5003
    BUFFER_SIZE = 1024

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((TCP_IP, TCP_PORT))
    s.listen(1)

    conn, addr = s.accept()

    print('Connection address:', addr)
    while 1:
        data = conn.recv(BUFFER_SIZE)
        if not data: break
        print('received data:', data)
        data = data
        conn.send(data)
        data = conn.recv(BUFFER_SIZE)  # ok for packgae

        # UPDATE:
        conn.send("UPDATE".encode())
        data = conn.recv(BUFFER_SIZE)  # ok for packgae
        for chunk in bin_file_to_buffer(FILE_PATH):
            packet = chunk
            print(packet)
            conn.send(packet)
            data = conn.recv(BUFFER_SIZE)  # ok for packgae
        conn.send("END_UPDATE".encode())
        data = conn.recv(BUFFER_SIZE)  # ok for packgae

        # Execute
        conn.send("EXECUTE".encode())
        data = conn.recv(BUFFER_SIZE)  # ok for packgae
        packet = EXE_STR
        conn.send(packet)
        data = conn.recv(BUFFER_SIZE)  # ok for packgae
        # get response



        conn.send("END_EXECUTE".encode())
        data = conn.recv(BUFFER_SIZE)  # ok for packgae

    conn.close()
