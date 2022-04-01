from board import TankBoard
from tile import Tile
from background_board import BackgroundBoard
import pygame
import sys
import constants
import json

import random
import time


class Game:
    def __init__(self):
        self._tank_board = None
        self._health_points = 100  # Game is something like instance of a player
        self._screen = None
        self._tiles = {}  # Here the textures are loaded
        self._width = None
        self._height = None

        # Background - here are objects to be displayed. Only ints are allowed
        self._background_board = None
        self._background_surface = None
        self._background_scale = None

        self._clock = None

    def setup(self):
        self._width = 800  # Those values need to be downloaded from socket
        self._height = 600
        self._background_scale = 50
        self._tank_board = TankBoard(self._width, self._height)
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

        x = 500
        y = 500
        tank = pygame.sprite.Sprite()
        _ = self.load_tile("./resources/tank.json")
        tank.image = self._tiles["./resources/tank.json"]["texture"]
        sprite_group = pygame.sprite.Group()
        sprite_group.add(tank)
        tank.rect = (x, y)

        running = True
        while running:
            self._clock.tick()
            pygame.display.set_caption("Project - Distracted Programming " + str(int(self._clock.get_fps())) + " fps")
            for ev in pygame.event.get():
                if ev.type == pygame.QUIT or (ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE):
                    pygame.quit()
                    sys.exit(0)
                    running = False

            # test:
            sprite_group.clear(self._screen, self._background_surface)
            x += random.randint(-1, 1)
            y += random.randint(-1, 1)
            tank.rect = (x, y)
            sprite_group.draw(self._screen)

            self._screen.blit(tank.image, (x, y))
            # test^

            pygame.display.flip()
        sys.exit(0)
