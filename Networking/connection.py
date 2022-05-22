import socket
from ctypes import *
from time import sleep

import select

import constants
from Networking.payload_configuration import PayloadConfiguration
from Networking.payload_information import PayloadInformation


class Connection:
    def __init__(self, game, address=constants.default_game_server_ip):
        self._port = 2137
        self._address = address
        self._socket = None
        self._player_id = None
        self._game = game
        self._data_exchange_thread = None

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
            return True

    def close_connection(self):
        self.send_disconnect_information()
        self._socket.close()

    # Sending part

    def send_disconnect_information(self):
        self.send_single_information(constants.information_disconnect, constants.information_disconnect,
                                     self._player_id, -1, -1, 0.0, 0.0, 0.0)
        #  Those variables are random, server first checks the disconnect information and closes the connection.

    def send_want_to_change_tank_or_turret(self, x_location, y_location, tank_angle, hp, turret_angle):
        self.send_single_information(constants.information_update, constants.information_tank, self.player_id,
                                     x_location, y_location, tank_angle, hp, turret_angle)

    def send_want_to_new_projectile(self, projectile_id, x_location, y_location, projectile_angle):
        self.send_single_information(constants.information_create, constants.information_projectile, self.player_id,
                                     x_location, y_location, projectile_angle, constants.projectile_exists,
                                     float(projectile_id))

    def send_want_to_change_projectile(self, projectile_id, x_location, y_location, projectile_angle, hp):
        self.send_single_information(constants.information_update, constants.information_projectile, self.player_id,
                                     x_location, y_location, projectile_angle, hp,
                                     float(projectile_id))

    def send_single_information(self, action, type_of, player_id, x_location, y_location, tank_angle, hp, turret_angle):
        payload_out = PayloadInformation(action.encode('utf-8'), type_of.encode('utf-8'), player_id, int(x_location),
                                         int(y_location), tank_angle, hp,
                                         turret_angle)
        nsent = self._socket.send(payload_out)
        if nsent:
            return True
        return False

    # Receiving part

    def receive_all_information(self):
        quit = False
        receivings = []
        while quit is False:
            r, _, _ = select.select([self._socket], [], [], 0)
            if r:
                buff = self._socket.recv(sizeof(PayloadInformation))
                if len(buff) < 28:
                    print("##DEBUG Error - received less bytes than expected!")
                    return receivings
                payload_in: PayloadInformation = PayloadInformation.from_buffer_copy(buff)
                receivings.append(payload_in)
            else:
                quit = True
        return receivings

    def receive_configuration(self):
        for i in range(5):  # Five tries to connect - ~5seconds
            r, _, _ = select.select([self._socket], [], [], 0)
            if r:
                buff = self._socket.recv(sizeof(PayloadConfiguration))
                payload_in = PayloadConfiguration.from_buffer_copy(buff)
                return payload_in.width, payload_in.height, payload_in.background_scale, payload_in.player_count, payload_in.player_id, payload_in.tank_spawn_x, payload_in.tank_spawn_y, payload_in.map_number
            sleep(0.1)
        return constants.configuration_receive_error, constants.configuration_receive_error, constants.configuration_receive_error, constants.configuration_receive_error, 0, 0, 0, 0

    # Prints should be replaced with serious actions
    def process_received_information(self, received_information_arr):
        for received_information in received_information_arr:
            # When searching for an item if not found we can just simply add such one!
            if received_information.action.decode('utf-8') == constants.information_update or \
                    received_information.action.decode('utf-8') == constants.information_create:
                if received_information.type_of.decode('utf-8') == constants.information_tank:
                    self._game.update_tank(received_information.player_id, received_information.x_location,
                                           received_information.y_location,
                                           received_information.tank_angle, received_information.hp,
                                           received_information.turret_angle)
                elif received_information.type_of.decode('utf-8') == constants.information_projectile:
                    # Create projectile
                    if received_information.action.decode('utf-8') == constants.information_create:
                        self._game.add_projectile_from_network(received_information.player_id, int(received_information.turret_angle), received_information.x_location, received_information.y_location, received_information.tank_angle)
                    # Update projectile
                    elif received_information.action.decode('utf-8') == constants.information_update:
                        self._game.update_projectile(received_information.player_id, int(received_information.turret_angle), received_information.x_location, received_information.y_location, received_information.hp)
                else:
                    print("Received command to update. The target was inappropriate!")
            elif received_information.action.decode('utf-8') == constants.information_disconnect:
                self._game.remove_tank(received_information.player_id)
            elif received_information.action.decode('utf-8') == constants.information_death:
                self._game.show_death_screen_and_exit() # This should be changed to some screen showing you're dead
                return
            else:
                print(f"Received wrong command! You wanted to: {received_information.action.decode('utf-8')}")

    @property
    def player_id(self):
        return self._player_id

    @player_id.setter
    def player_id(self, player_id):
        self._player_id = player_id
    # To my best knowledge I think that thread for sending information is not needed. Might be changed if it's required
