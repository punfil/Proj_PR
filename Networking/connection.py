import ctypes
import socket
import threading  # If client is not already enough fun :)

import constants
from Networking.receiver import Receiver

from Networking.payload_information import PayloadInformation


class Connection:
    def __init__(self, game, address=constants.default_game_server_ip):
        self._port = 2137
        self._address = address
        self._socket = None
        self._receiver = None
        self._receiver_thread = None
        self._player_id = None
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
            self._socket.settimeout(constants.socket_timeout)
            self._receiver = Receiver(self._socket, self._game)
            self._receiver_thread = None
            return True

    def close_connection(self):
        self.send_disconnect_information()
        self._receiver.terminate()
        self._socket.close()

    def send_disconnect_information(self):
        print("##DEBUG Sending disconnect information!")
        self.send_single_information(constants.information_disconnect, constants.information_disconnect,
                                     self._player_id, -1, -1, 0.0, 0.0, 0.0)
        #  Those variables are random, server first checks the disconnect information and closes the connection.

    def send_want_to_change_tank_or_turret(self, x_location, y_location, tank_angle, hp, turret_angle):
        self.send_single_information(constants.information_update, constants.information_turret, self.player_id,
                                     x_location, y_location, tank_angle, hp, turret_angle)

    def send_want_to_new_projectile(self, x_location, y_location, tank_angle):
        self.send_single_information(constants.information_create, constants.information_projectile, self.player_id,
                                     x_location, y_location, tank_angle, constants.projectile_exists, 0.0)

    def send_single_information(self, action, type_of, player_id, x_location, y_location, tank_angle, hp, turret_angle):
        payload_out = PayloadInformation(action.encode(), type_of.encode(), player_id, x_location, y_location, tank_angle, hp,
                                         turret_angle)
        nsent = self._socket.send(payload_out)
        if nsent:
            return True
        return False

    def initialize_receiver(self):
        self._receiver_thread = threading.Thread(target=self._receiver.receive_multiple_information)
        self._receiver_thread.start()

    @property
    def receiver(self):
        return self._receiver

    @receiver.setter
    def receiver(self, receiver):
        self._receiver = receiver

    @property
    def player_id(self):
        return self._player_id

    @player_id.setter
    def player_id(self, player_id):
        self._player_id = player_id
    # To my best knowledge I think that thread for sending information is not needed. Might be changed if it's required
