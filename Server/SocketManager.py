BUFFER_SIZE = 1050
OK_PACKET_SIZE = 2


class SocketManager:
    @staticmethod
    def send_data(conn, message):
        # send size of message
        conn.send(str(len(message)).encode())

        # receive OK for size
        data = conn.recv(OK_PACKET_SIZE)

        # send message
        if isinstance(message, str):
            conn.send(message.encode())

        elif isinstance(message, bytes):
            conn.send(message)

        else:
            raise ValueError("Unsupported type of data to send")

        # receive OK for message
        data = conn.recv(OK_PACKET_SIZE)

    @staticmethod
    def receive(conn):
        # receive size
        size = conn.recv(BUFFER_SIZE)

        # send OK for size
        conn.send("OK".encode())

        # receive message
        data = conn.recv(int(size.decode()))

        # send OK for message
        conn.send("OK".encode())

        data = data.decode()
        return data
