import select
import socket
import sys
import time
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
            self._socket.settimeout(5.0)
            return True

    def close_connection(self):
        self._socket.close()

    def receive_single_tank(self):
        buff = self._socket.recv(sizeof(PayloadTank))
        payload_in: PayloadTank = PayloadTank.from_buffer_copy(buff)
        return payload_in.player_id, payload_in.x_location, payload_in.y_location

    def send_single_tank(self, id, x, y):
        payload_out = PayloadTank(id, x, y)
        nsent = self._socket.send(payload_out)
        if nsent:
            return True
        return False

    def receive_single_configuration(self):
        for i in range(5):
            print(i, "iteration")
            r, _, _ = select.select([self._socket], [], [], 0)
            if r:
                buff = self._socket.recv(sizeof(PayloadConfiguration))  # Here is the problem, sometimes does not receive data and program hangs
                payload_in = PayloadConfiguration.from_buffer_copy(buff)
                print("Final, [", payload_in.width, ", ", payload_in.height, "]")
                return payload_in.width, payload_in.height, payload_in.background_scale, payload_in.player_count, 0, payload_in.tank_spawn_x, payload_in.tank_spawn_y
            time.sleep(1)
        return 0, 0, 0, 0, 0, 0, 0
