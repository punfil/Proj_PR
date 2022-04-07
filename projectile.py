import pygame
from math import sin, cos, pi


class Projectile(pygame.sprite.Sprite):
    def __init__(self, owner, x, y, angle, attributes):
        super().__init__()

        self._x = x
        self._y = y
        self._angle = angle
        self._owner = owner  # projectile cannot collide with its owner (the tank that fired it)

        self._speed = attributes["speed"]
        self._damage = attributes["damage"]
        self._lifetime = attributes["lifetime"]

        self.image = attributes["texture"]
        self.image = pygame.transform.rotozoom(self.image, self._angle, 1)
        self.rect = self.image.get_rect()
        self.rect.center = (self._x, self._y)

    def update(self, delta_time):
        self._lifetime -= delta_time
        if self._lifetime <= 0:
            self.kill()
            return

        dx = -self._speed * sin(self._angle * (pi / 180)) * delta_time
        dy = -self._speed * cos(self._angle * (pi / 180)) * delta_time
        self._x += dx
        self._y += dy
        self.rect.center = (self._x, self._y)
