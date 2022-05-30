import pygame
from math import sin, cos, pi

import constants


class Explosion(pygame.sprite.Sprite):
    """
    Represents explosion animation object.
    """
    def __init__(self, x, y, angle, attributes):
        super().__init__()
        self._x = x
        self._y = y

        self._lifetime = attributes["lifetime"]
        self._starting_lifetime = self._lifetime

        self._animation_frames = attributes["animation_frames"][:]

        if attributes["inherit_angle"]:
            for i, image in enumerate(self._animation_frames):
                self._animation_frames[i] = pygame.transform.rotozoom(image, angle, 1)

        self.image = self._animation_frames[0]

        self.rect = self.image.get_rect()
        self.rect.center = (self._x, self._y)

    def update(self, delta_time):
        """
        Overrides the method from pygame.sprite.Sprite
        Decreases animation lifetime and updates currently displayed frame accordingly
        :param float delta_time: Time elapsed since last call of this function
        :return: None
        """
        self._lifetime -= delta_time
        if self._lifetime <= 0:
            self.kill()
            return

        frames_number = len(self._animation_frames)
        frame = frames_number - int(self._lifetime / self._starting_lifetime * frames_number) - 1
        self.image = self._animation_frames[frame]
        self.rect = self.image.get_rect()
        self.rect.center = (self._x, self._y)
