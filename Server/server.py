#!/usr/bin/env python
import socket
from SocketManager import SocketManager

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
NEW_IP = "192.168.43.49"
NEW_PORT = 4001
FILE_INFO_DIR = r"C:\Users\admin\Desktop\New folder"
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
    TCP_IP = '127.0.0.1'
    TCP_PORT = 5006

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((TCP_IP, TCP_PORT))
    s.listen(1)

    conn, addr = s.accept()

    print('Connection address:', addr)
    while 1:
        data = SocketManager.receive(conn)
        if not data: break
        print('received data:', data)
        SocketManager.send_data(conn, data)

        # # UPDATE:
        # conn.send("UPDATE".encode())
        # data = conn.recv(BUFFER_SIZE)  # ok for packgae
        # for chunk in bin_file_to_buffer(FILE_PATH):
        #     packet = chunk
        #     print(packet)
        #     conn.send(packet)
        #     data = conn.recv(BUFFER_SIZE)  # ok for packgae
        # conn.send("END_UPDATE".encode())
        # data = conn.recv(BUFFER_SIZE)  # ok for packgae

        # # Execute
        # conn.send("EXECUTE".encode())
        # data = conn.recv(2)  # ok for packgae
        # packet = EXE_STR
        # conn.send(packet.encode())
        # data = conn.recv(2)  # ok for packgae
        # # get response
        # data = conn.recv(BUFFER_SIZE * 100)
        # conn.send("OK".encode())
        # print("hsgfasygdfjksd")
        # print(data.decode())  # TODO handle /n
        # conn.send("END_EXECUTE".encode())
        # data = conn.recv(BUFFER_SIZE)  # ok for packgae

        # Change IP
        # while True: # TODO: timeout or scheduler
        #     conn.send("CHANGE_IP".encode())
        #     data = conn.recv(2)  # ok for packgae
        #     # send size of packet
        #     packet_str = "%s:%s" % (NEW_IP, NEW_PORT)
        #     conn.send(str(len(packet_str)).encode())
        #     data = conn.recv(2)  # ok for packgae
        #     conn.send(packet_str.encode())
        #     data = conn.recv(2)  # ok for packgae
        #     data = conn.recv(6)
        #     if data.decode() == 'accept':
        #         print(str(data))
        #         break

        # file system info
        SocketManager.send_data(conn, "FILE_SYSTEM_INFO")
        SocketManager.send_data(conn, FILE_INFO_DIR)
        data = SocketManager.receive(conn)

        print(data)

conn.close()
