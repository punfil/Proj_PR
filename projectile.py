import pygame
from math import sin, cos, pi

import constants


class Projectile(pygame.sprite.Sprite):
    def __init__(self, id, owner, x, y, angle, turret, attributes):
        super().__init__()
        self._id = id
        self._x = x
        self._y = y
        self._angle = angle
        self._owner = owner  # projectile cannot collide with its owner (the tank that fired it)
        self._turret = turret
        self._alive = True

        self._speed = attributes["speed"]
        self._damage = attributes["damage"]
        self._lifetime = attributes["lifetime"]

        self.image = attributes["texture"]
        self.image = pygame.transform.rotozoom(self.image, self._angle, 1)
        self.rect = self.image.get_rect()
        self.rect.center = (self._x, self._y)

    def update(self, delta_time):
        if self._owner.player_no == self._turret.game._my_player_id and self._alive:
            x, y = self.save_my_data()
            self._lifetime -= delta_time
            if self._lifetime <= 0 and self._alive:
                self._turret.game.send_projectile_update(self._id, self._x, self._y, self._angle, constants.projectile_not_exists)
                self._alive = False
                return
            dx = -self._speed * sin(self._angle * (pi / 180)) * delta_time
            dy = -self._speed * cos(self._angle * (pi / 180)) * delta_time
            self._x += dx
            self._y += dy
            self._turret.game.send_projectile_update(self._id, self._x, self._y, self._angle, constants.projectile_exists)
            self._x, self._y = x, y
        self.rect.center = (self._x, self._y)

    def update_from_server(self, x, y):
        self._x = x
        self._y = y

    def save_my_data(self):
        return self._x, self._y

    @property
    def x(self):
        return self._x

    @property
    def y(self):
        return self._y

    @property
    def angle(self):
        return self._angle

    @property
    def hp(self):
        return self._

    @property
    def id(self):
        return self._id
