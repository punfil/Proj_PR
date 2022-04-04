from board import TankBoard
from tile import Tile
from background_board import BackgroundBoard
from tank import Tank
import pygame
import sys
import constants
import json

import random


class Game:
    def __init__(self):
        self._screen = None
        self._tiles = {}  # Here the textures are loaded
        self._width = None
        self._height = None
        self._player_count = None

        # Tank related variables
        self._tanks = None
        self._tanks_sprites = None
        self._tanks_sprites_group = None

        # Background - here are objects to be displayed. Only int sizes are allowed
        self._background_board = None
        self._background_scale = None

        self._clock = None

    def setup(self):
        self._width = 800  # Those values need to be downloaded from socket
        self._height = 600
        self._background_scale = 50
        self._player_count = 1
        tank_spawn_x = 500
        tank_spawn_y = 500  # Can be random received from server or constant spawn point

        self._background_board = BackgroundBoard(self._width, self._height, self._background_scale)

        self.load_map("doesn't work yet, map path+filename will go here")

        pygame.init()
        pygame.display.set_caption("Project - Distracted Programming")

        self._screen = pygame.display.set_mode((self._width, self._height + constants.bar_height))
        self._clock = pygame.time.Clock()

        # Tank setup - Will even the tank class be required? No one knows...
        _ = self.load_tile("./resources/tank.json")
        self._tanks_sprites_group = pygame.sprite.Group()
        self._tanks = [None for _ in range(self._player_count)]
        self._tanks_sprites = [None for _ in range(self._player_count)]
        for i in range(self._player_count):
            self._tanks[i] = Tank(i, tank_spawn_x, tank_spawn_y)
            tank = pygame.sprite.Sprite()
            tank.image = self._tiles["./resources/tank.json"]["texture"]
            tank.rect = (self._tanks[i].x, self._tanks[i].y)
            self._tanks_sprites_group.add(tank)
            self._tanks_sprites[i] = tank

    def load_map(self, filename):
        """loads map from file --- !doesn't work yet!"""

        # currently a very simple random board generator, actual loading will come later
        for x in range(self._background_board.width):
            for y in range(self._background_board.height):
                if random.random() > 0.2:
                    filename = "./resources/grass.json"
                else:
                    filename = "./resources/house.json"
                tile_attributes = self.load_tile(filename)
                self._background_board.set_tile(x, y, Tile(x, y, tile_attributes))

    def load_tile(self, filename):
        tile = self._tiles.get(filename)
        if tile:
            return tile
        with open(filename, 'r') as file:
            tile = json.load(file)
        tile["texture"] = pygame.image.load(tile["texture"])
        tile["texture"] = pygame.transform.scale(tile["texture"], (self._background_scale, self._background_scale))
        self._tiles[filename] = tile
        return tile

    def play(self):
        self._background_board.draw(self._screen, draw_all=True)

        while True:
            self._clock.tick(120)

            self._background_board.draw(self._screen)  # not a performance issue - only draws updated background parts
            pygame.display.set_caption("Project - Distracted Programming " + str(int(self._clock.get_fps())) + " fps")

            for ev in pygame.event.get():
                if ev.type == pygame.QUIT or (ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE):
                    pygame.quit()
                    sys.exit(0)
            keys = pygame.key.get_pressed()
            if keys[pygame.K_LEFT]:
                self._tanks[0].offsetX(-constants.default_movement_speed)
            if keys[pygame.K_RIGHT]:
                self._tanks[0].offsetX(constants.default_movement_speed)
            if keys[pygame.K_UP]:
                self._tanks[0].offsetY(-constants.default_movement_speed)
            if keys[pygame.K_DOWN]:
                self._tanks[0].offsetY(constants.default_movement_speed)
            if keys[pygame.K_SPACE]:
                # updating tiles test
                tile = random.choice([self.load_tile("resources/grass.json"), self.load_tile("resources/grass.json"), self.load_tile("resources/house.json")])
                randx = random.randint(0, self._background_board.width-1)
                randy = random.randint(0, self._background_board.height-1)
                self._background_board.set_tile(randx, randy, Tile(randx, randy, tile))

            self._tanks_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._tanks_sprites_group.draw(self._screen)
            for i in range(self._player_count):
                self._screen.blit(self._tanks_sprites[i].image, (self._tanks[i].x, self._tanks[i].y))
            # test^

            pygame.display.flip()
