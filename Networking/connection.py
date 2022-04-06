import socket
import sys
from ctypes import *
from payload import Payload


class Connection:
    def __init__(self):
        self._port = 2137
        self._socket = None

    def establish_connection(self):
        try:
            self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self._socket.connect(("127.0.0.1", self._port))
        except socket.error as err:
            return False
        except AttributeError as err:
            return False
        finally:
            return True

    def close_connection(self):
        self._socket.close()

    def receive_single_payload(self):
        buff = self._socket.recv(sizeof(Payload))
        payload_in = Payload.from_buffer_copy(buff)
        return payload_in
