import constants
import pygame
from math import sin, cos, pi


class Tank(pygame.sprite.Sprite):
    def __init__(self, player_no, x, y, attributes):
        super().__init__()
        self._player_no = player_no
        self._x = x
        self._y = y

        self._hp = attributes["hp"]
        self._max_speed = attributes["max_speed"]
        self._acceleration = attributes["acceleration"]
        self._deceleration = attributes["deceleration"]
        self._drag = attributes["drag"]
        self._turn_rate = attributes["turn_rate"]
        self._driftiness = attributes["driftiness"]

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
        self.keys = keys

    def update(self, delta_time):
        velocity_changed = False
        if self.keys[pygame.K_UP]:
            self.accelerate(self._acceleration * delta_time)
            velocity_changed = True
        if self.keys[pygame.K_DOWN]:
            self.accelerate(-self._deceleration * delta_time)
            velocity_changed = True

        if not velocity_changed:
            self.drag(self._drag * delta_time)

        if self.keys[pygame.K_LEFT]:
            self.rotate(self._turn_rate * delta_time)
        if self.keys[pygame.K_RIGHT]:
            self.rotate(-self._turn_rate * delta_time)

        self._move_angle = self._angle  # drifting doesn't work yet :<

        dx = -self._speed * sin(self._move_angle * (pi / 180)) * delta_time
        dy = -self._speed * cos(self._move_angle * (pi / 180)) * delta_time

        if self.check_x_move(dx):
            self._x += dx
        if self.check_y_move(dy):
            self._y += dy

        self.rect.center = (self._x, self._y)

    def accelerate(self, acceleration):
        self._speed += acceleration
        self._speed = min(self._speed, self._max_speed)
        self._speed = max(self._speed, -self._max_speed)  # limit speed when driving backwards

    def drag(self, drag):
        start_speed = self._speed
        if self._speed > 0:
            self._speed -= drag
        else:
            self._speed += drag

        if start_speed * self._speed < 0:  # aka if speeds before and after applying drag were in opposite directions
            self._speed = 0

    def rotate(self, angle):
        self._angle += angle
        self._angle %= 360

        self.image = pygame.transform.rotozoom(self.original_image, self._angle, 1)
        self.rect = self.image.get_rect()
        self.rect.center = (self._x, self._y)

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
    def hp(self):
        return self._hp

    def offset_hp(self, value):
        self._hp += value

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
