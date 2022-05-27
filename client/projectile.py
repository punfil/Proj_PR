import pygame
from math import sin, cos, pi

import constants


class Projectile(pygame.sprite.Sprite):
    """
    Represents projectile object.
    Attributes:
        _id: ID of the projectile (unique)
        ..others
    """
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
        """
        Overrides the method from pygame.sprite.Sprite
        Updates projectiles position
        :param float delta_time: Time elapsed since last call of this function
        :return: None
        """

        if self._owner.player_no == self._turret.game._my_player_id and self._alive:
            x, y = self.save_my_data()
            self._lifetime -= delta_time
            if self._lifetime <= 0 and self._alive:
                self.die()
                return

            if self._x < 0 or self._y < 0 or self._x > self._turret.game._width or self._y > self._turret.game._height:
                self.die()
                return

            if self._turret.game.get_tile_at_screen_position(self._x, self._y).get_attribute("blocks_movement"):
                self.die()
                return

            dx = -self._speed * sin(self._angle * (pi / 180)) * delta_time
            dy = -self._speed * cos(self._angle * (pi / 180)) * delta_time
            self._x += dx
            self._y += dy
            self._turret.game.send_projectile_update(self._id, self._x, self._y, self._angle,
                                                     constants.projectile_exists)
            #self._x, self._y = x, y
        self.rect.center = (self._x, self._y)

    def update_from_server(self, x, y):
        """
        Updates coordinates of the projectile according to the information received from server
        :param int x: X coordinate of the projectile's location
        :param y: Y coordinate of the projectile's location
        :return: None
        """
        self._x = x
        self._y = y

    def save_my_data(self):
        """
        Function that returns the location of the projectile
        :return: (x, y) of the projectile's location
        :rtype (int, int)
        """
        return self._x, self._y

    def die(self):
        """
        Sets the projectile _alive state to False, sends request to server to delete the projectile
        :return: None
        """
        self._alive = False
        self._turret.game.send_projectile_update(self._id, self._x, self._y, self._angle,
                                                 constants.projectile_not_exists)

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
