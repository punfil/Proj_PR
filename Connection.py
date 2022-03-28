import socket


class Connection:
    def __init__(self, hostname, port):
        self.__hostname = hostname
        self.__port = port

    def connect(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((self.__hostname, self.__port))
            s.sendall(b"Hello world!")
            data = s.recv(1024)
            print("Received an answer of " + data)


