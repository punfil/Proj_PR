import constants
import pygame
from math import sin, cos, pi
from turret import Turret
from hp_bar import HPBar


class Tank(pygame.sprite.Sprite):
    def __init__(self, player_no, game, x, y, attributes):
        super().__init__()
        self._player_no = player_no
        self._game = game
        self._x = x
        self._y = y

        self._hp = attributes["hp"]
        self._max_hp = self._hp
        self._max_speed = attributes["max_speed"]
        self._acceleration = attributes["acceleration"]
        self._deceleration = attributes["deceleration"]
        self._drag = attributes["drag"]
        self._turn_rate = attributes["turn_rate"]
        self._driftiness = attributes["driftiness"]

        turret_attributes = self._game.load_resource(attributes["turret"])
        self._turret = Turret(self, self._game, turret_attributes)

        self._hp_bar = HPBar(self)

        self._speed = 0
        self._angle = 0       # direction the tank is *facing*
        self._move_angle = 0  # direction the tank is *moving*
                              # ~it's driftin' time~

        self.image = attributes["texture"]
        self.original_image = attributes["texture"]
        # ^required, because repeatedly rotating the same image decreases its quality and increases size^
        self.rect = self.image.get_rect()
        self.rect.center = (x, y)

        self.keys = []  # keys pressed by player

    def keyboard_input(self, keys):
        """receives pressed keys, that will be used to determine user input when updating the tank"""
        self.keys = keys

    def handle_keyboard(self, delta_time):
        """controls the tank, based on pressed keys"""

        # forward/backward movement
        velocity_changed = False
        if self.keys[pygame.K_UP]:
            self.accelerate(self._acceleration * delta_time)
            velocity_changed = True
        if self.keys[pygame.K_DOWN]:
            self.accelerate(-self._deceleration * delta_time)
            velocity_changed = True
        if not velocity_changed:
            self.drag(self._drag * delta_time)

        # turning the tank
        if self.keys[pygame.K_LEFT]:
            self.rotate(self._turn_rate * delta_time)
        if self.keys[pygame.K_RIGHT]:
            self.rotate(-self._turn_rate * delta_time)

        if self.keys[pygame.K_q]:
            self.rotate_turret(delta_time)
        if self.keys[pygame.K_e]:
            self.rotate_turret(-delta_time)
        if self.keys[pygame.K_w]:
            self._turret.shoot()

    def update(self, delta_time):
        self.handle_keyboard(delta_time)

        self._move_angle = self._angle  # drifting doesn't work yet :<

        dx = -self._speed * sin(self._move_angle * (pi / 180)) * delta_time
        dy = -self._speed * cos(self._move_angle * (pi / 180)) * delta_time

        # todo - maybe reduce speed when tank enters collision?
        if self.check_x_move(dx):
            self._x += dx
        if self.check_y_move(dy):
            self._y += dy

        self.rect.center = (self._x, self._y)

    def accelerate(self, acceleration):
        """increases/decreases the speed, only in the direction the tank is facing"""

        self._speed += acceleration
        self._speed = min(self._speed, self._max_speed)
        self._speed = max(self._speed, -self._max_speed)  # limit speed when driving backwards

    def drag(self, drag):
        """slows down the tank (called only when no acceleration input was given)"""
        start_speed = self._speed
        if self._speed > 0:
            self._speed -= drag
        else:
            self._speed += drag

        if start_speed * self._speed < 0:  # aka if speeds before and after applying drag were in opposite directions
            self._speed = 0

    def rotate(self, angle):
        """rotates the tank left or right, by a given angle"""
        self._angle += angle
        self._angle %= 360

        self.image = pygame.transform.rotozoom(self.original_image, self._angle, 1)
        self.rect = self.image.get_rect()
        self.rect.center = (self._x, self._y)

    def rotate_turret(self, angle):
        """rotates the tank turret by a given angle"""
        self._turret.rotate(angle)

    @property
    def turret(self):
        return self._turret

    @property
    def hp_bar(self):
        return self._hp_bar

    @property
    def x(self):
        return self._x

    @x.setter
    def x(self, value):
        self._x = value

    @property
    def y(self):
        return self._y

    @y.setter
    def y(self, value):
        self._y = value

    @property
    def speed(self):
        return self._speed

    @speed.setter
    def speed(self, speed):
        self._speed = speed

    @property
    def angle(self):
        return self._angle

    @property
    def hp(self):
        return self._hp

    @property
    def max_hp(self):
        return self._max_hp

    def offset_hp(self, value):
        self._hp += value
        self._hp_bar.update_hp()

    def check_y_move(self, value):
        if self._y + value < 0 or self._y + value > constants.window_height:
            self.offset_hp(-constants.object_collision_damage)
            return False
        return True

    def check_x_move(self, value):
        if self._x + value < 0 or self._x + value > constants.window_width:
            # Needs to know what are dimensions of tank, for now square, later circle
            self.offset_hp(-constants.object_collision_damage)
            return False
        return True
