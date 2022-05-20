import pygame_menu.themes

from Boards.background_board import BackgroundBoard
from Networking.connection import Connection
from tank import Tank
import pygame
import sys
import constants
import json
import threading
import time


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
        self._mutex = threading.Lock()  # This will be used for receiving information + reading from memory.
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
        self._menu.add.text_input('Server IP Address :', default=constants.default_game_server_ip, onchange=self.change_server_ip)
        self._menu.add.button("Play", self.quit_menu)
        self.display_menu()


        """initializes all variables, loads data from server"""
        self._connection = Connection(self, self._server_address)
        if not self._connection.establish_connection():
            return False
        _, _, _, self._player_count, self._my_player_id, tank_spawn_x, tank_spawn_y, map_no = self._connection._receiver.receive_configuration()
        if self._player_count == constants.configuration_receive_error:
            print("INFO: Error connecting to server. Please try again later. Bye!")
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

        self._connection.initialize_receiver()

        return True

    def get_tank_with_player_id(self, player_id):
        for i in self._tanks:
            if i.player_no == player_id:
                return i
        return None

    def add_new_tank(self, player_id, x, y, tank_angle):
        tank = Tank(player_id, self,  x, y, tank_angle, self.load_resource("resources/tank.json"))
        self._tanks_sprites_group.add(tank)
        self._turrets_sprites_group.add(tank.turret)
        self._hp_bars_sprites_group.add(tank.hp_bar)
        self._tanks.append(tank)
        print("New tank!")

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

    def exit_game(self):
        """closes the connection with server and exits game"""
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

    def remove_tank(self, player_id):
        tank = self.get_tank_with_player_id(player_id)
        tank.kill()

    def update_tank(self, player_id, x_location, y_location, tank_angle, hp, turret_angle):
        tank = self.get_tank_with_player_id(player_id)
        if tank is None:
            self.add_new_tank(player_id, x_location, y_location, tank_angle)
        else:
            self.get_tank_with_player_id(player_id).update_values_from_server(x_location, y_location, tank_angle, hp, turret_angle)

    def send_tank_position(self, x_location, y_location, tank_angle, hp, turret_angle):
        self._connection.send_want_to_change_tank_or_turret(x_location, y_location, tank_angle, hp, turret_angle)

    def play(self):
        """runs the game"""

        self._background_board.draw(self._screen, draw_all=True)

        while True:
            start = time.time()
            delta_time = self._clock.tick(
                constants.main_loop_per_second) / 1000  # number of seconds passed since the last frame

            self._background_board.draw(self._screen)  # not a performance issue - only draws updated background parts
            pygame.display.set_caption("Project - Distracted Programming " + str(int(self._clock.get_fps())) + " fps")

            for ev in pygame.event.get():
                if ev.type == pygame.QUIT or (ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE):
                    self.exit_game()

            keys = pygame.key.get_pressed()
            self._my_tank.keyboard_input(keys)

            self._tanks_sprites_group.update(delta_time)
            self._turrets_sprites_group.update(delta_time)
            self._projectiles_sprites_group.update(delta_time)
            self._hp_bars_sprites_group.update()

            self._tanks_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._turrets_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._projectiles_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._hp_bars_sprites_group.clear(self._screen, self._background_board.background_surface)

            self._tanks_sprites_group.draw(self._screen)
            self._turrets_sprites_group.draw(self._screen)
            self._projectiles_sprites_group.draw(self._screen)
            self._hp_bars_sprites_group.draw(self._screen)

            pygame.display.flip()

            end = time.time()
            time.sleep(1 / constants.main_loop_per_second - (start - end))

    @property
    def my_player_id(self):
        return self._my_player_id

    @my_player_id.setter
    def my_player_id(self, player_id):
        self._my_player_id = player_id
