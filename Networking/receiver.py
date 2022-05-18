import select
from ctypes import *
import constants
import time

from Networking.payload_information import PayloadInformation
from Networking.payload_configuration import PayloadConfiguration


class Receiver:
    def __init__(self, socket, game):
        self._running = True
        self._socket = socket
        self._game = game

    def receive_multiple_information(self):
        while self._running:
            r, _, _ = select.select([self._socket], [], [], 0)
            if r:
                buff = self._socket.recv(sizeof(PayloadInformation))
                if len(buff) < 28:
                    print("##DEBUG Error - received less bytes than expected!")
                    continue
                payload_in: PayloadInformation = PayloadInformation.from_buffer_copy(buff)
                self.process_received_information(payload_in)
            else:
                time.sleep(constants.receiver_sleep_time)

    def terminate(self):
        self._running = False

    def receive_configuration(self):
        for i in range(5):  # Five tries to connect - ~5seconds
            r, _, _ = select.select([self._socket], [], [], 0)
            if r:
                buff = self._socket.recv(sizeof(PayloadConfiguration))
                payload_in = PayloadConfiguration.from_buffer_copy(buff)
                return payload_in.width, payload_in.height, payload_in.background_scale, payload_in.player_count, payload_in.player_id, payload_in.tank_spawn_x, payload_in.tank_spawn_y, payload_in.map_number
            time.sleep(constants.configuration_receive_timeout)
        return constants.configuration_receive_error, constants.configuration_receive_error, constants.configuration_receive_error, constants.configuration_receive_error, 0, 0, 0, 0

    # Prints should be replaced with serious actions
    def process_received_information(self, received_information: PayloadInformation):
        # When searching for an item if not found we can just simply add such one!
        if received_information.action.decode('utf-8') == constants.information_update or \
                received_information.action.decode('utf-8') == constants.information_create:
            if received_information.type_of.decode('utf-8') == constants.information_tank:
                self._game.update_tank(received_information.player_id, received_information.x_location, received_information.y_location,
                                       received_information.tank_angle, received_information.hp, received_information.turret_angle)
            elif received_information.type_of.decode('utf-8') == constants.information_projectile:
                print("Update somebody's projectile")
            elif received_information.type_of.decode('utf-8') == constants.information_turret:
                print("Update somebody's turret")
            else:
                print("Received command to update. The target was inappropriate!")
        elif received_information.action.decode('utf-8') == constants.information_disconnect:
            self._game.delete_tank(received_information.player_id)
        else:
            print(f"Received wrong command! You wanted to: {received_information.action.decode('utf-8')}")
