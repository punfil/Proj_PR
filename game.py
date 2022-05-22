import time

import pygame_menu.themes

from Boards.background_board import BackgroundBoard
from Networking.connection import Connection
from tank import Tank
import pygame
import sys
import constants
import json


class Game:
    def __init__(self):
        self._screen = None
        self._resources = {}  # loaded jsons of game resources (tiles, tanks, projectiles, etc)
        self._width = constants.window_width
        self._height = constants.window_height
        self._player_count = None
        self._my_player_id = None

        # Connection related variables
        self._connection = None
        self._server_address = constants.default_game_server_ip

        # Tank related variables
        self._my_tank = None  # For easier access
        self._my_tank_sprite = None
        self._tanks = None
        self._tanks_sprites = None
        self._tanks_sprites_group = None
        self._turrets_sprites_group = None
        self._projectiles_sprites_group = None
        self._hp_bars_sprites_group = None

        # Background - here are objects to be displayed. Only int sizes are allowed
        self._background_board = None
        self._background_scale = constants.background_scale

        self._spawn_points = None

        self._clock = None
        self._menu = None
        self._in_menu = True

    def quit_menu(self):
        self._in_menu = False
        self._menu.disable()

    def display_menu(self):
        self._menu.mainloop(self._screen)

    def change_server_ip(self, ip):
        self._server_address = ip

    def setup(self):
        pygame.init()
        pygame.display.set_caption("Project - Distracted Programming")

        self._screen = pygame.display.set_mode((self._width, self._height + constants.bar_height))
        self._clock = pygame.time.Clock()

        self._menu = pygame_menu.Menu("Tank simulator", constants.window_width, constants.window_height,
                                      theme=pygame_menu.themes.THEME_DARK);
        self._menu.add.text_input('Server IP Address :', default=constants.default_game_server_ip,
                                  onchange=self.change_server_ip)
        self._menu.add.button("Play", self.quit_menu)

        def exit_game_button():
            self.exit_game(False)

        self._menu.add.button("Quit", exit_game_button)
        self.display_menu()

        """initializes all variables, loads data from server"""
        self._connection = Connection(self, self._server_address)
        if not self._connection.establish_connection():
            self.show_server_full_or_busy_screen()
            return False
        _, _, _, self._player_count, self._my_player_id, tank_spawn_x, tank_spawn_y, map_no = self._connection.receive_configuration()
        if self._player_count == constants.configuration_receive_error:
            self.show_server_full_or_busy_screen()
            return False
        self._connection.player_id = self._my_player_id

        self._background_board = BackgroundBoard(self, self._width, self._height, self._background_scale)

        # TODO Map number - variable "map_no - done :)
        self.load_map("save.json")

        # changing spawn_points coordinates from grid units to pixels
        for sp in self._spawn_points:
            sp[0] = sp[0] * self._background_scale + self._background_scale / 2
            sp[1] = sp[1] * self._background_scale + self._background_scale / 2

        # todo - I want to somehow merge all these different sprite groups into one big group with different layers.
        self._tanks_sprites_group = pygame.sprite.Group()
        self._turrets_sprites_group = pygame.sprite.Group()
        self._projectiles_sprites_group = pygame.sprite.Group()
        self._hp_bars_sprites_group = pygame.sprite.Group()

        # Adding my tank. Opponents tanks will be added later
        self._tanks = []
        self._my_tank = Tank(self._my_player_id, self, tank_spawn_x, tank_spawn_y, 0.0,  # Default angle
                             self.load_resource("resources/tank.json"))
        self._tanks_sprites_group.add(self._my_tank)
        self._turrets_sprites_group.add(self._my_tank.turret)
        self._hp_bars_sprites_group.add(self._my_tank.hp_bar)
        self._tanks.append(self._my_tank)

        return True

    def get_tank_with_player_id(self, player_id):
        for i in self._tanks:
            if i.player_no == player_id:
                return i
        return None

    def add_new_tank(self, player_id, x, y, tank_angle):
        tank = Tank(player_id, self, x, y, tank_angle, self.load_resource("resources/tank.json"))
        self._tanks_sprites_group.add(tank)
        self._turrets_sprites_group.add(tank.turret)
        self._hp_bars_sprites_group.add(tank.hp_bar)
        self._tanks.append(tank)

    def load_map(self, filename):
        """loads map from file"""

        with open(filename, 'r') as file:
            save_data = json.load(file)

        self._background_board.deserialize(save_data["map_data"])
        self._spawn_points = save_data["spawn_points"]

    def load_resource(self, filename):
        """loads a json resource file. Automatically converts textures to pygame images"""

        resource = self._resources.get(filename)
        if resource:
            return resource
        with open(filename, 'r') as file:
            resource = json.load(file)
        resource["texture"] = pygame.image.load(resource["texture"])
        resource["resource_name"] = filename
        self._resources[filename] = resource
        return resource

    def get_tile_at_screen_position(self, x, y):
        """returns tile from background board corresponding to the given (x,y) screen position"""
        return self._background_board.get_tile(int(x / self._background_scale), int(y / self._background_scale))

    def screen_position_to_grid_position(self, x, y):
        """converts screen position (in pixels) to grid position (in tiles)"""
        return int(x / self._background_scale), int(y / self._background_scale)

    def show_server_full_or_busy_screen(self):
        finished = False
        time_start = time.time()
        dead_image = pygame.image.load("Pictures/busy_or_full.png").convert_alpha()
        self._screen.blit(dead_image, dead_image.get_rect(center=self._screen.get_rect().center))
        while not finished:
            for ev in pygame.event.get():
                if ev.type == pygame.QUIT or (ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE):
                    finished = True
            pygame.display.flip()
            if time.time() - time_start > constants.server_full_or_busy_screen_display_time_sec:
                finished = True
        sys.exit(0)

    def show_death_screen_and_exit(self):
        self._connection.close_connection()
        finished = False
        time_start = time.time()
        dead_image = pygame.image.load("Pictures/dead.png").convert_alpha()
        self._screen.blit(dead_image, dead_image.get_rect(center=self._screen.get_rect().center))
        while not finished:
            for ev in pygame.event.get():
                if ev.type == pygame.QUIT or (ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE):
                    finished = True
            pygame.display.flip()
            if time.time() - time_start > constants.death_screen_display_time_sec:
                finished = True
        sys.exit(0)

    def exit_game(self, should_close_connection):
        """closes the connection with server and exits game"""
        if should_close_connection:
            self._connection.close_connection()
        sys.exit(0)

    def add_projectile(self, projectile):
        """adds a new projectile to the projectiles sprite group"""
        self._projectiles_sprites_group.add(projectile)

    def add_hp_bar(self, hp_bar):
        """adds hp bar to the hp bars sprite group"""
        self._hp_bars_sprites_group.add(hp_bar)

    def remove_hp_bar(self, hp_bar):
        """removes hp bar from the hp bars sprite group"""
        hp_bar.kill()

    # Networking
    def add_projectile_from_network(self, player_id, projectile_id, x_location, y_location, projectile_angle):
        tank = self.get_tank_with_player_id(player_id)
        tank.turret.add_projectile_from_server(projectile_id, x_location, y_location, projectile_angle)

    def remove_tank(self, player_id):
        tank = self.get_tank_with_player_id(player_id)
        if tank is not None:
            tank.kill()
            self._tanks.remove(tank)

    def update_tank(self, player_id, x_location, y_location, tank_angle, hp, turret_angle):
        tank = self.get_tank_with_player_id(player_id)
        if tank is None:
            self.add_new_tank(player_id, x_location, y_location, tank_angle)
        else:
            self.get_tank_with_player_id(player_id).update_values_from_server(x_location, y_location, tank_angle, hp,
                                                                              turret_angle)

    def remove_projectile(self, player_id, projectile_id):
        tank = self.get_tank_with_player_id(player_id)
        tank.turret.delete_projectile(projectile_id)

    def update_projectile(self, player_id, projectile_id, x_location, y_location, hp):
        if hp == constants.projectile_not_exists:
            self.remove_projectile(player_id, projectile_id)
        elif hp == constants.projectile_exists:
            tank = self.get_tank_with_player_id(player_id)
            tank.turret.get_projectile_with_id(projectile_id).update_from_server(x_location, y_location)

    def send_tank_position(self, x_location, y_location, tank_angle, hp, turret_angle):
        self._connection.send_want_to_change_tank_or_turret(x_location, y_location, tank_angle, hp, turret_angle)

    def send_projectile_add(self, projectile_id, x_location, y_location, projectile_angle):
        self._connection.send_want_to_new_projectile(projectile_id, x_location, y_location, projectile_angle)

    def send_projectile_update(self, projectile_id, x_location, y_location, projectile_angle, hp):
        self._connection.send_want_to_change_projectile(projectile_id, x_location, y_location, projectile_angle, hp)

    def play(self):
        """runs the game"""
        self._background_board.draw(self._screen, draw_all=True)
        delta_time = 0.0
        received = True
        while True:
            delta_time += self._clock.tick(45) / 1000  # number of seconds passed since the last frame

            self._background_board.draw(self._screen)  # not a performance issue - only draws updated background parts
            pygame.display.set_caption("Project - Distracted Programming " + str(int(self._clock.get_fps())) + " fps")

            for ev in pygame.event.get():
                if ev.type == pygame.QUIT or (ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE):
                    self.exit_game(True)

            keys = pygame.key.get_pressed()
            self._my_tank.keyboard_input(keys)

            # Calculate values and at the same time send to server
            if received is True:
                self._tanks_sprites_group.update(delta_time)
                self._turrets_sprites_group.update(delta_time)
                self._projectiles_sprites_group.update(delta_time)
                self._hp_bars_sprites_group.update()
                delta_time = 0.0
                received = False

            self._tanks_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._turrets_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._projectiles_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._hp_bars_sprites_group.clear(self._screen, self._background_board.background_surface)

            # Receive processed information
            received_information_arr = self._connection.receive_all_information()
            if len(received_information_arr) > 0:
                received = True
                self._connection.process_received_information(received_information_arr)

            # Draw all the information on the screen
            self._tanks_sprites_group.draw(self._screen)
            self._turrets_sprites_group.draw(self._screen)
            self._projectiles_sprites_group.draw(self._screen)
            self._hp_bars_sprites_group.draw(self._screen)

            pygame.display.flip()

    @property
    def my_player_id(self):
        return self._my_player_id

    @my_player_id.setter
    def my_player_id(self, player_id):
        self._my_player_id = player_id
