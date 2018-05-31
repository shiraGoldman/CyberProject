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


PACKET_FORMAT = "TS%s01%s%s%s"
PACKET_INITIAL_SIZE = 8
FILE_PATH = "C:\\Users\\admin\\Downloads\\calc.exe"
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
        data = data  # + '\0'.encode()
        conn.send(data)
        data = conn.recv(BUFFER_SIZE)#ok for packgae
		

        i = 0
        is_last = 0
        for chunk in bin_file_to_buffer(FILE_PATH):
            #packet = PACKET_FORMAT % (format(i, '02d'), format(is_last, '02d'), chunk, PACKET_INITIAL_SIZE+len(chunk))
            #print(packet.encode())
            packet = chunk
            conn.send(packet)
            data = conn.recv(BUFFER_SIZE)#ok for packgae
            ##break
            i += 1
    conn.close()
