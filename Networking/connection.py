import socket
import threading  # If client is not already enough fun :)

import constants
from Networking.receiver import Receiver

from Networking.payload_information import PayloadInformation


class Connection:
    def __init__(self, player_id, game, address=constants.default_game_server_ip):
        self._port = 2137
        self._address = address
        self._socket = None
        self._receiver = None
        self._receiver_thread = None
        self._player_id = player_id
        self._game = game

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
            self._receiver = Receiver(self._socket, self._game)
            self._receiver_thread = threading.Thread(target=self._receiver.receive_multiple_information)
            self._receiver_thread.start()
            return True

    def close_connection(self):
        self.send_disconnect_information()
        self._receiver.terminate()
        self._socket.close()

    def send_disconnect_information(self):
        self.send_single_information(constants.information_disconnect, constants.information_disconnect,
                                     self._player_id, -1, -1, 0.0, 0.0, 0.0)
        #  Those variables are random, server first checks the disconnect information and closes the connection.

    def send_single_information(self, action, type_of, player_id, x_location, y_location, tank_angle, hp, turret_angle):
        payload_out = PayloadInformation(action, type_of, player_id, x_location, y_location, tank_angle, hp,
                                         turret_angle)
        nsent = self._socket.send(payload_out)
        if nsent:
            return True
        return False

    @property
    def receiver(self):
        return self._receiver

    @receiver.setter
    def receiver(self, receiver):
        self._receiver = receiver

    # To my best knowledge I think that thread for sending information is not needed. Might be changed if it's required
