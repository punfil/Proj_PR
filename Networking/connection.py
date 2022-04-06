import socket
import sys
from ctypes import *
from Networking.payload_tank import PayloadTank
from Networking.payload_configuration import PayloadConfiguration


class Connection:
    def __init__(self, address="192.168.0.21"):
        self._port = 2137
        self._address = address
        self._socket = None

    def establish_connection(self):
        try:
            self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self._socket.connect((self._address, self._port))
        except socket.error as err:
            return False
        except AttributeError as err:
            return False
        finally:
            return True

    def close_connection(self):
        self._socket.close()

    def receive_single_tank(self):
        buff = self._socket.recv(sizeof(PayloadTank))
        payload_in: PayloadTank = PayloadTank.from_buffer_copy(buff)
        return payload_in.player_id, payload_in.x_location, payload_in.y_location

    def send_single_tank(self):
        payload_out = PayloadTank(1, 5, 3)
        nsent = self._socket.send(payload_out)
        if nsent:
            return True
        return False

    def receive_single_configuration(self):
        buff = self._socket.recv(sizeof(PayloadConfiguration))
        payload_in = PayloadConfiguration.from_buffer_copy(buff)
        return payload_in.width, payload_in.height, payload_in.background_scale, payload_in.player_count, payload_in.tank_spawn_x, payload_in.tank_spawn_y
