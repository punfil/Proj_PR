from board import TankBoard
from tile import Tile
from background_board import BackgroundBoard
from tank import Tank
import pygame
import sys
import constants
import json

import random
import time


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

        # Background - here are objects to be displayed. Only ints are allowed
        self._background_board = None
        self._background_surface = None
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

        pygame.init()
        pygame.display.set_caption("Project - Distracted Programming")

        for x in range(int(self._width / self._background_scale)):
            for y in range(int(self._height / self._background_scale)):
                # loading from file should go somewhere around here
                if random.random() > 0.2:
                    filename = "./resources/grass.json"
                else:
                    filename = "./resources/house.json"
                _ = self.load_tile(filename)
                self._background_board.setUpTile(x, y, Tile(self._background_board.getXForDrawing(x),
                                                            self._background_board.getYForDrawing(y),
                                                            self._tiles[filename]["texture"]))
                # todo Tile should(?) get the whole tile instead of just the texture

        self._screen = pygame.display.set_mode((self._width, self._height + constants.bar_height))
        self._background_surface = pygame.Surface((self._width, self._height))
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

    def draw_background_surface(self):
        for x in range(int(self._width / self._background_scale)):
            for y in range(int(self._height / self._background_scale)):
                self._background_surface.blit(self._background_board.getTile(x, y).texture,
                                              (self._background_board.getXForDrawing(x),
                                               self._background_board.getYForDrawing(y)))
        self._screen.blit(self._background_surface, (0, 0))

    def play(self):
        self.draw_background_surface()

        while True:
            self._clock.tick()
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


            self._tanks_sprites_group.clear(self._screen, self._background_surface)
            self._tanks_sprites_group.draw(self._screen)
            for i in range(self._player_count):
                self._screen.blit(self._tanks_sprites[i].image, (self._tanks[i].x, self._tanks[i].y))
            # test^

            pygame.display.flip()
        sys.exit(0)
