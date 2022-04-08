from time import sleep

from tile import Tile
from Boards.background_board import BackgroundBoard
from Networking.connection import Connection
from tank import Tank
import pygame
import sys
import constants
import json

import random


class Game:
    def __init__(self):
        self._screen = None
        self._resources = {}  # loaded jsons of game resources (tiles, tanks, projectiles, etc)
        self._width = None
        self._height = None
        self._player_count = None
        self._my_player_id = None

        # Connection related variables
        self._connection = None

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
        self._background_scale = None

        self._clock = None

    def setup(self):
        """initializes all variables, loads data from server"""

        # self._connection = Connection()
        # if not self._connection.establish_connection():
        #     return False
        # self._width, self._height, self._background_scale, self._player_count, self._my_player_id, tank_spawn_x, tank_spawn_y = self._connection.receive_single_configuration()
        # if self._width == 0:
        #     return False

        self._width = constants.window_width  # Those values need to be downloaded from socket
        self._height = constants.window_height
        self._background_scale = constants.background_scale
        self._player_count = 1
        tank_spawn_x = 500
        tank_spawn_y = 500  # Can be random received from server or constant spawn point

        self._background_board = BackgroundBoard(self._width, self._height, self._background_scale)

        self.load_map("doesn't work yet, map path+filename will go here")

        pygame.init()
        pygame.display.set_caption("Project - Distracted Programming")

        self._screen = pygame.display.set_mode((self._width, self._height + constants.bar_height))
        self._clock = pygame.time.Clock()

        self._tanks_sprites_group = pygame.sprite.Group()
        self._turrets_sprites_group = pygame.sprite.Group()
        self._projectiles_sprites_group = pygame.sprite.Group()
        self._hp_bars_sprites_group = pygame.sprite.Group()
        self._tanks = []
        for i in range(self._player_count):
            tank = Tank(i, self, tank_spawn_x, tank_spawn_y, self.load_resource("resources/tank.json"))
            self._tanks_sprites_group.add(tank)
            self._turrets_sprites_group.add(tank.turret)
            self._hp_bars_sprites_group.add(tank.hp_bar)
            self._tanks.append(tank)
            self._my_tank = tank  # Warning!
        return True

    def load_map(self, filename):
        """loads map from file --- !doesn't work yet!"""

        # currently, a very simple random board generator, actual loading will come later
        for x in range(self._background_board.width):
            for y in range(self._background_board.height):
                if random.random() > 0.2:
                    filename = "./resources/grass.json"
                else:
                    filename = "./resources/house.json"
                tile_attributes = self.load_resource(filename)
                self._background_board.set_tile(x, y, Tile(x, y, tile_attributes))

    def load_resource(self, filename):
        """loads a json resource file. Automatically converts textures to pygame images"""

        resource = self._resources.get(filename)
        if resource:
            return resource
        with open(filename, 'r') as file:
            resource = json.load(file)
        resource["texture"] = pygame.image.load(resource["texture"])
        resource["texture"] = pygame.transform.scale(resource["texture"],
                                                     (self._background_scale, self._background_scale))
        self._resources[filename] = resource
        return resource

    def draw_hp_bars(self):
        for i in range(self._player_count):
            pygame.draw.rect(self._background_board.background_surface, (255, 0, 0),
                             (self._tanks[i].x, self._tanks[i].y - 20, 50, 10))  # NEW
            # pygame.draw.rect(self._background_board.background_surface, (0, 128, 0),
            # (self._tanks[i].x, self._tanks[i].y - 20, 50 - (5 * (10 - self._tanks[i].hp)), 100))  # NEW

    def exit_game(self):
        """closes the connection with server and exits game"""
        self._connection.close_connection()
        sys.exit(0)

    def add_projectile(self, projectile):
        """adds a new projectile to the projectiles sprite group"""
        self._projectiles_sprites_group.add(projectile)

    def play(self):
        """runs the game"""

        self._background_board.draw(self._screen, draw_all=True)

        while True:
            delta_time = self._clock.tick(30) / 1000  # number of seconds passed since the last frame

            self._background_board.draw(self._screen)  # not a performance issue - only draws updated background parts
            pygame.display.set_caption("Project - Distracted Programming " + str(int(self._clock.get_fps())) + " fps")

            for ev in pygame.event.get():
                if ev.type == pygame.QUIT or (ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE):
                    self.exit_game()

            keys = pygame.key.get_pressed()
            self._my_tank.keyboard_input(keys)

            if keys[pygame.K_SPACE]:
                # updating tiles test
                tile = random.choice(
                    [self.load_resource("resources/grass.json"), self.load_resource("resources/grass.json"),
                     self.load_resource("resources/house.json")])
                randx = random.randint(0, self._background_board.width - 1)
                randy = random.randint(0, self._background_board.height - 1)
                self._background_board.set_tile(randx, randy, Tile(randx, randy, tile))

            self._tanks_sprites_group.update(delta_time)
            self._turrets_sprites_group.update(delta_time)
            self._projectiles_sprites_group.update(delta_time)
            self._hp_bars_sprites_group.update()
            # Important - send the server the information that the position changed!

            self._tanks_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._turrets_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._projectiles_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._hp_bars_sprites_group.clear(self._screen, self._background_board.background_surface)

            self._tanks_sprites_group.draw(self._screen)
            self._turrets_sprites_group.draw(self._screen)
            self._projectiles_sprites_group.draw(self._screen)
            self._hp_bars_sprites_group.draw(self._screen)

            pygame.display.flip()
