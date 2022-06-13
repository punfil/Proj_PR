import time

import pygame_menu.themes

from Boards.background_board import BackgroundBoard
from Networking.connection import Connection
from tank import Tank
from explosion import Explosion
import pygame
import sys
import constants
import json

import numpy as np


class Game:
    """
    Represents the whole game. Connects all the components into one piece
    Attributes:
        _screen: Screen the game is displayed at
        ...other
    """

    def __init__(self):
        self._screen = None
        self._resources = {}  # loaded jsons of game resources (tiles, tanks, projectiles, etc)
        self._width = constants.window_width
        self._height = constants.window_height
        self._player_count = None
        self._my_player_id = None
        self._tank_version = 0

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
        self._explosions_sprites_group = None
        self._hp_bars_sprites_group = None

        # Background - here are objects to be displayed. Only int sizes are allowed
        self._background_board = None
        self._background_scale = constants.background_scale

        self._spawn_points = None

        self._clock = None
        self._menu = None
        self._in_menu = True

    @staticmethod
    def load_default_ip() -> str:
        """
        Loads last used server IP from text file from constants.default_cache_save_file
        :return: Loaded server IP or None if file didn't exist\
        :rtype: str
        """
        return_ip = None
        try:
            with open(constants.default_cache_save_file, "r") as f:
                return_ip = f.read()
        except FileNotFoundError:
            pass
        return return_ip

    @staticmethod
    def save_default_ip(ip: str) -> None:
        """
        Function that saves IP used in this game to file for later use
        :param ip: IP to be saved
        :return: None
        """
        try:
            with open(constants.default_cache_save_file, "w") as f:
                f.write(ip)
        except FileExistsError:
            pass

    def quit_menu(self):
        """
        Disables the menu and quits it
        :return: None
        """
        self._in_menu = False
        self._menu.disable()

    def display_menu(self):
        """
        Enters the menu. Enables it as well
        :return: None
        """
        self._in_menu = True
        self._menu.enable()
        self._menu.mainloop(self._screen)

    def change_server_ip(self, ip):
        """
        Updates the server's IP address to the given value
        :param str ip: New server's IP address
        :return: None
        """
        self._server_address = ip

    def setup(self):
        """
        Initializes all the variables
        Initializes graphics
        Initializes connection with the server
        Prepares the map and tanks
        :return: True if succeeded else False
        :rtype: bool
        """
        pygame.init()
        pygame.display.set_caption("Project - Distracted Programming")

        self._screen = pygame.display.set_mode((self._width, self._height + constants.bar_height))
        self._clock = pygame.time.Clock()

        self._server_address = self.load_default_ip()
        if self._server_address is None:
            self._server_address = constants.default_game_server_ip

        self._menu = pygame_menu.Menu("Tank simulator", constants.window_width, constants.window_height,
                                      theme=pygame_menu.themes.THEME_DARK)
        self._menu.add.text_input('Server IP Address :', default=self._server_address,
                                  onchange=self.change_server_ip)
        self._menu.add.selector('Tank: ', constants.tank_selections,
                                onchange=lambda _, tank_version: self.set_tank_version(tank_version))
        self._menu.add.button("Play", self.quit_menu)

        def exit_game_button():
            """
            Allows to exit the game using exit button in the menu
            :return: None
            """
            self.exit_game(False)

        self._menu.add.button("Quit", exit_game_button)
        self.display_menu()

        """initializes all variables, loads data from server"""
        self._connection = Connection(self, self._server_address)
        if not self._connection.establish_connection():
            self.show_server_full_or_busy_screen()
            return False
        tank_full_hp = 10.0
        self._connection.send_preferences(self._tank_version, tank_full_hp)
        _, _, _, self._player_count, self._my_player_id, tank_spawn_x, tank_spawn_y, map_no = self._connection.receive_configuration()
        if self._player_count == constants.configuration_receive_error:
            self.show_server_full_or_busy_screen()
            return False
        self._player_count = 1  # This variable is modified within other functions that will be used to add existing players
        self._connection.player_id = self._my_player_id

        self._background_board = BackgroundBoard(self, self._width, self._height, self._background_scale)

        self.load_map(constants.maps[map_no])

        # changing spawn_points coordinates from grid units to pixels
        for sp in self._spawn_points:
            sp[0] = sp[0] * self._background_scale + self._background_scale / 2
            sp[1] = sp[1] * self._background_scale + self._background_scale / 2
        my_spawn_point = self._spawn_points[self._my_player_id % len(self._spawn_points)]
        tank_spawn_x, tank_spawn_y, tank_spawn_angle = my_spawn_point[0], my_spawn_point[1], my_spawn_point[2]

        # todo - I want to somehow merge all these different sprite groups into one big group with different layers.
        self._tanks_sprites_group = pygame.sprite.Group()
        self._turrets_sprites_group = pygame.sprite.Group()
        self._projectiles_sprites_group = pygame.sprite.Group()
        self._explosions_sprites_group = pygame.sprite.Group()
        self._hp_bars_sprites_group = pygame.sprite.Group()

        # Adding my tank. Opponents tanks will be added later
        self._tanks = []
        self._my_tank = Tank(self._my_player_id, self, tank_spawn_x, tank_spawn_y, tank_spawn_angle,
                             self.load_resource(constants.tank_versions[self._tank_version]))
        self.send_tank_position(self._my_tank.x, self._my_tank.y, self._my_tank.angle,
                                self._my_tank.hp, self._my_tank.turret.angle)
        # sending the correct tank position (determined from spawn point) to the server

        self._tanks_sprites_group.add(self._my_tank)
        self._turrets_sprites_group.add(self._my_tank.turret)
        self._hp_bars_sprites_group.add(self._my_tank.hp_bar)
        self._tanks.append(self._my_tank)

        return True

    def get_tank_with_player_id(self, player_id):
        """
        Returns tank with given player ID
        :param int player_id: ID of the player the tank belongs to
        :return: Tank with given player ID or None if not found
        :rtype: Tank
        """
        for i in self._tanks:
            if i.player_no == player_id:
                return i
        return None

    def add_new_tank(self, player_id, x, y, tank_angle, tank_version):
        """
        Adds new tank according to the information received from the server
        :param int player_id: ID of the player this tank belongs to
        :param int x: X coordinate of the tank's location
        :param int y: Y coordinate of the tank's location
        :param float tank_angle: Angle of the tank
        :param int tank_version: Number representing the tank version
        :return: None
        """
        if tank_version not in constants.tank_versions:
            tank_version = 0
        tank = Tank(player_id, self, x, y, tank_angle, self.load_resource(constants.tank_versions[tank_version]))
        self._tanks_sprites_group.add(tank)
        self._turrets_sprites_group.add(tank.turret)
        self._hp_bars_sprites_group.add(tank.hp_bar)
        self._tanks.append(tank)
        self._player_count += 1

    def load_map(self, filename):
        """
        Loads map from file
        :param str filename: Name of the file which the map will be loaded from
        :return: None
        """
        with open(filename, 'r') as file:
            save_data = json.load(file)

        self._background_board.deserialize(save_data["map_data"])
        self._spawn_points = save_data["spawn_points"]

    def load_resource(self, filename):
        """
        Loads a json resource file. Automatically converts textures to pygame images
        :param str filename: Name of the file which the resource will be loaded from
        :return: Loaded resource
        :rtype: dict
        """

        resource = self._resources.get(filename)
        if resource:
            return resource
        with open(filename, 'r') as file:
            resource = json.load(file)
        if resource.get("texture"):
            resource["texture"] = pygame.image.load(resource["texture"])
        if resource.get("animation_frames"):
            for i, img_path in enumerate(resource["animation_frames"]):
                resource["animation_frames"][i] = pygame.image.load(img_path)
        resource["resource_name"] = filename
        self._resources[filename] = resource
        return resource

    def get_tile_at_screen_position(self, x, y):
        """
        Returns tile from background board corresponding to the given (x,y) screen position
        :param int x: X coordinate of the tile's location to be returned
        :param int y: Y coordinate of the tile's location to be returned
        :return: Tile at the given location
        :rtype: Tile
        """
        return self._background_board.get_tile(int(x / self._background_scale), int(y / self._background_scale))

    def screen_position_to_grid_position(self, x, y):
        """
        Converts screen position (in pixels) to grid position (in tiles)
        :param int x: X coordinate of the screen position
        :param int y: Y coordinate of the screen position
        :return: Grid position at given screen position
        :rtype: (int, int)
        """
        return int(x / self._background_scale), int(y / self._background_scale)

    def surface_to_grayscale(self, surface: pygame.Surface):
        """
        Changes the given surface colors to grayscale
            - from https://stackoverflow.com/questions/10261440/how-can-i-make-a-greyscale-copy-of-a-surface-in-pygame
        :param surface: surface to be grayscaled
        :return: grayscaled surface
        :rtype: pygame.Surface
        """
        arr = pygame.surfarray.pixels3d(surface)
        mean_arr = np.dot(arr[:, :, :], [0.216, 0.587, 0.144])
        mean_arr3d = mean_arr[..., np.newaxis]
        new_arr = np.repeat(mean_arr3d[:, :, :], 3, axis=2)
        return pygame.surfarray.make_surface(new_arr)

    def show_server_full_or_busy_screen(self):
        """
        Display the screen that the server is full or busy at the moment and returns to the main menu
        :return: None
        """
        finished = False
        time_start = time.time()
        dead_image = pygame.image.load("./Pictures/busy_or_full.png").convert_alpha()
        self._screen.blit(dead_image, dead_image.get_rect(center=self._screen.get_rect().center))
        while not finished:
            for ev in pygame.event.get():
                if ev.type == pygame.QUIT or (ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE):
                    finished = True
            pygame.display.flip()
            if time.time() - time_start > constants.server_full_or_busy_screen_display_time_sec:
                finished = True

        if self.setup():
            self.play()

    def show_death_screen(self):
        """
        Displays the screen that this player has died and returns to the main menu
        :return: None
        """
        self._connection.close_connection()
        finished = False
        time_start = time.time()
        dead_image = pygame.image.load("./Pictures/dead.png").convert_alpha()
        self._screen.blit(self.surface_to_grayscale(self._screen), (0, 0))
        self._screen.blit(dead_image, dead_image.get_rect(center=self._screen.get_rect().center))
        while not finished:
            for ev in pygame.event.get():
                if ev.type == pygame.QUIT or (ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE):
                    finished = True
            pygame.display.flip()
            if time.time() - time_start > constants.death_screen_display_time_sec:
                finished = True
            self._clock.tick(constants.target_fps)

        if self.setup():
            self.play()

    def exit_game(self, should_close_connection):
        """
        Optionally closes the connection with server and exits game
        :param bool should_close_connection: Whether to close the connection to the server or not
        :return: None
        """
        if should_close_connection:
            self._connection.close_connection()
        self.save_default_ip(self._server_address)
        sys.exit(0)

    def add_projectile(self, projectile):
        """
        Adds a new projectile to the projectiles sprite group
        :param Projectile projectile: Projectile to be added to projectiles sprite group
        :return: None
        """
        self._projectiles_sprites_group.add(projectile)

    def add_hp_bar(self, hp_bar):
        """
        Adds hp bar to the hp bars sprite group
        :param HPBar hp_bar: HPBar to be added to hp_bars sprite group
        :return: None
        """
        self._hp_bars_sprites_group.add(hp_bar)

    def remove_hp_bar(self, hp_bar):
        """
        Removes hp bar from the hp bars sprite group
        :param HPBar hp_bar: HPBar to be remove from hp_bars sprite group
        :return: None
        """
        hp_bar.kill()

    # Networking
    def add_projectile_from_network(self, player_id, projectile_id, x_location, y_location, projectile_angle):
        """
        Adds projectile created by other player according to the information received from the server
        :param int player_id: ID of the player this projectile belongs to
        :param int projectile_id: ID of this projectile
        :param int x_location: X coordinate of this projectile's location
        :param int y_location: Y coordinate of this projectile's location
        :param float projectile_angle: Angle of this projectile
        :return: None
        """

        tank = self.get_tank_with_player_id(player_id)
        if tank.turret.get_projectile_with_id(projectile_id) is None:
            tank.turret.add_projectile_from_server(projectile_id, x_location, y_location, projectile_angle)

    def remove_tank(self, player_id):
        """
        Removes tank assigned to the player with the given player ID
        :param int player_id: ID of the player whose tank should be removed
        :return: None
        """
        tank = self.get_tank_with_player_id(player_id)
        if tank is not None:
            tank.turret.remove_all_projectiles()
            tank.turret.kill()
            tank.kill()
            self._tanks.remove(tank)
            self._player_count -= 1

    def update_tank(self, player_id, x_location, y_location, tank_angle, hp, turret_angle):
        """
        Updates the tank values according to the information received from the server.
        :param int player_id: ID of the player that this tank belongs to
        :param int x_location: X coordinate of the updated tank's location
        :param int y_location: Y coordinate of the updated tank's location
        :param float tank_angle: Angle of the updated tank
        :param float hp: HP of the updated tank
        :param float turret_angle: Angle of the updated tank's turret
        :return: None
        """
        tank = self.get_tank_with_player_id(player_id)
        if tank is None:
            print("tank angle =", tank_angle, "turret angle =", turret_angle)
            self.add_new_tank(player_id, x_location, y_location, tank_angle, tank_version=int(turret_angle))
        else:
            self.get_tank_with_player_id(player_id).update_values_from_server(x_location, y_location, tank_angle, hp,
                                                                              turret_angle)

    def remove_projectile(self, player_id, projectile_id):
        """
        Removes projectile according to the information received from the server
        :param int player_id: ID of the player this projectile belongs to
        :param int projectile_id: ID of the projectile to be removed
        :return: None
        """
        tank = self.get_tank_with_player_id(player_id)
        if tank is not None:
            tank.turret.delete_projectile(projectile_id)

    def update_projectile(self, player_id, projectile_id, x_location, y_location, projectile_angle, hp):
        """
        Updates the projectile values according to the information received from the server.
        :param int player_id: ID of the player that this projectile belongs to
        :param int projectile_id: ID of the projectile to be updated
        :param int x_location: X coordinate of the updated projectile's location
        :param int y_location: Y coordinate of the updated projectile's location
        :param float projectile_angle: Angle of the projectile
        :param float hp: HP of the updated projectile - whether it exists or not
        :return: None
        """
        tank = self.get_tank_with_player_id(player_id)
        projectile = tank.turret.get_projectile_with_id(projectile_id)
        if projectile is None:
            tank.turret.add_projectile_from_server(projectile_id, x_location, y_location, projectile_angle)
            projectile = tank.turret.get_projectile_with_id(projectile_id)

        if hp == constants.projectile_not_exists:
            self._explosions_sprites_group.add(Explosion(projectile.x, projectile.y,
                                                         projectile.angle, projectile.explosion))
            self.remove_projectile(player_id, projectile_id)
        elif hp == constants.projectile_exists:
            projectile.update_from_server(x_location, y_location)

    def send_tank_position(self, x_location, y_location, tank_angle, hp, turret_angle):
        """
        Sends calculated position of the tank
        :param int x_location: New X coordinate of the tank's position
        :param int y_location: New Y coordinate of the tank's position
        :param float tank_angle: New tank's angle
        :param float hp: New tank's HP
        :param float turret_angle: New tank's turret's angle
        :return: None
        """
        self._connection.send_want_to_change_tank_or_turret(x_location, y_location, tank_angle, hp, turret_angle)

    def send_projectile_add(self, projectile_id, x_location, y_location, projectile_angle):
        """
        Sends information to add new projectile to the game
        :param int projectile_id: ID of the new projectile
        :param int x_location: X coordinate of the projectile's position
        :param int y_location: Y coordinate of the projectile's position
        :param float projectile_angle: Angle of the projectile
        :return: None
        """
        self._connection.send_want_to_new_projectile(projectile_id, x_location, y_location, projectile_angle)

    def send_projectile_update(self, projectile_id, x_location, y_location, projectile_angle, hp):
        """
        Sends information to update projectile owned by this player
        :param int projectile_id: ID of the updated projectile
        :param int x_location: X coordinate of the projectile's position
        :param int y_location: Y coordinate of the projectile's position
        :param float projectile_angle: Angle of the projectile (Despite it doesn't change over time)
        :param float hp: HP of the projectile (Exists or not)
        :return: None
        """
        self._connection.send_want_to_change_projectile(projectile_id, x_location, y_location, projectile_angle, hp)

    def play(self):
        """
        Runs the whole game!
        :return: None
        """
        self._background_board.draw(self._screen, draw_all=True)
        delta_time = 0.0
        received = False
        while True:
            delta_time += self._clock.tick(constants.target_fps) / 1000  # number of seconds passed since the last frame

            self._background_board.draw(self._screen)  # not a performance issue - only draws updated background parts
            pygame.display.set_caption("Project - Distracted Programming " + str(int(self._clock.get_fps())) + " fps")

            for ev in pygame.event.get():
                if ev.type == pygame.QUIT or (ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE):
                    self.exit_game(True)

            # Receive processed information
            received_information_arr = self._connection.receive_all_information()
            if len(received_information_arr) > 0:
                received = True
                self._connection.process_received_information(received_information_arr)

            keys = pygame.key.get_pressed()
            self._my_tank.keyboard_input(keys)

            # Calculate values and at the same time send to server
            if received is True:
                self._tanks_sprites_group.update(delta_time)
                self._turrets_sprites_group.update(delta_time)
                self._projectiles_sprites_group.update(delta_time)
                self._explosions_sprites_group.update(delta_time)
                self._hp_bars_sprites_group.update()
                delta_time = 0.0
                received = False

            self._tanks_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._turrets_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._projectiles_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._explosions_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._hp_bars_sprites_group.clear(self._screen, self._background_board.background_surface)

            # Draw all the information on the screen
            self._tanks_sprites_group.draw(self._screen)
            self._turrets_sprites_group.draw(self._screen)
            self._projectiles_sprites_group.draw(self._screen)
            self._explosions_sprites_group.draw(self._screen)
            self._hp_bars_sprites_group.draw(self._screen)

            pygame.display.flip()

    def set_tank_version(self, new_tank_version):
        self._tank_version = new_tank_version

    @property
    def my_player_id(self):
        return self._my_player_id

    @my_player_id.setter
    def my_player_id(self, player_id):
        self._my_player_id = player_id
