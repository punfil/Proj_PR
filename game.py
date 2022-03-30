from board import Board
from grass import Grass
from asphalt import Asphalt
import pygame
import sys
import constants


class Game:
    def __init__(self):
        self._board = None
        self._health_points = 100  # Game is something like instance of a player
        self._screen = None
        self._background = None
        self._textures = {}
        self._width = None
        self._height = None

    def setup(self):
        self._width = 800  # Those values need to be downloaded from socket
        self._height = 600
        self._board = Board(self._width, self._height)
        pygame.init()
        pygame.display.set_caption("Project - Distracted Programming")
        self._textures['grass'] = pygame.transform.scale(pygame.image.load(constants.grass_jpg), (1, 1))
        for x in range(self._width):
            for y in range(self._height):
                self._board.setUpPile(x, y,
                                      Grass(x, y, None, self._textures["grass"]))  # For now no tank will be available
        self._screen = pygame.display.set_mode((self._width, self._height + constants.bar_height))
        self._background = pygame.Surface((self._width, self._height))

    def play(self):
        running = True
        while running:
            for ev in pygame.event.get():
                if ev.type == pygame.QUIT:
                    pygame.quit()
                    running = False
            for x in range(self._width):
                for y in range(self._height):
                    self._screen.blit(self._board.getPile(x, y).texture, (x, y))
            pygame.display.flip()
        sys.exit(0)
