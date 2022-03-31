from board import Board
from tile import Tile
import pygame
import sys
import constants
import json

import random
import time


class Game:
    def __init__(self):
        self._board = None
        self._health_points = 100  # Game is something like instance of a player
        self._screen = None
        self._tiles = {}
        self._width = None
        self._height = None
        self._background = None
        self._clock = None

    def setup(self):
        self._width = 800  # Those values need to be downloaded from socket
        self._height = 600
        self._board = Board(self._width, self._height)
        
        pygame.init()
        pygame.display.set_caption("Project - Distracted Programming")

        for x in range(self._width):
            for y in range(self._height):
                # loading from file should go somewhere around here
                if random.random()>0.2:
                    filename = "./resources/grass.json"
                else:
                    filename = "./resources/house.json"
                tile = self.load_tile(filename)

                self._board.setUpTile(x, y, Tile(x, y, self._tiles[filename]["texture"]))
                # todo Tile should(?) get the whole tile instead of just the texture

        self._screen = pygame.display.set_mode((self._width, self._height + constants.bar_height))
        self._background = pygame.Surface((self._width, self._height))
        self._clock = pygame.time.Clock()

    def load_tile(self, filename):
        tile = self._tiles.get(filename)
        if tile:
            return tile
        with open(filename, 'r') as file:
            tile = json.load(file)
        tile["texture"] = pygame.image.load(tile["texture"])
        self._tiles[filename] = tile
        return tile

    def draw_board(self):
        for x in range(self._width):
            for y in range(self._height):
                if x % 50 == 0 and y % 50 == 0:
                    self._background.blit(self._board.getTile(x, y).texture, (x, y))
        self._screen.blit(self._background, (0,0))

    def play(self):
        self.draw_board()

        x = 500
        y = 500
        tank = pygame.sprite.Sprite()
        tank.image = self._tiles["./resources/house.json"]["texture"]
        sprite_group = pygame.sprite.Group()
        sprite_group.add(tank)
        tank.rect = (x, y)


        running = True
        while running:
            self._clock.tick()
            pygame.display.set_caption("Project - Distracted Programming " + str(int(self._clock.get_fps())) + " fps")
            for ev in pygame.event.get():
                if ev.type == pygame.QUIT:
                    pygame.quit()
                    running = False

            # self._screen.blit(self._background, (0,0))

            # test:
            sprite_group.clear(self._screen, self._background)
            x += random.randint(-1, 1)
            y += random.randint(-1, 1)
            tank.rect = (x, y)
            sprite_group.draw(self._screen)

            self._screen.blit(tank.image, (x, y))
            # test^

            pygame.display.flip()
        sys.exit()
