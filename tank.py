import constants


class Tank:
    def __init__(self, player_no, x, y):
        self.__player_no = player_no
        self._x = x
        self._y = y
        self._speed = constants.default_movement_speed
        self._hp = constants.default_HP

    @property
    def x(self):
        return self._x

    @x.setter
    def x(self, value):
        self._x = value

    def offsetX(self, value):
        if self.check_x_move(value):
            self._x += value

    @property
    def y(self):
        return self._y

    @y.setter
    def y(self, value):
        self._y = value

    def offsetY(self, value):
        if self.check_y_move(value):
            self._y += value

    @property
    def speed(self):
        return self._speed

    @speed.setter
    def speed(self, speed):
        self._speed = speed

    @property
    def hp(self):
        return self._hp

    def offsetHP(self, value):
        self._hp += value

    def check_y_move(self, value):
        if self._y + value < 0 or self._y + value + constants.background_scale > constants.window_height:
            self.offsetHP(-constants.object_collision_damage)
            return False
        return True

    def check_x_move(self, value):
        if self._x + value < 0 or self._x + value + constants.background_scale > constants.window_width:
            # Needs to know what are dimensions of tank, for now square, later circle
            self.offsetHP(-constants.object_collision_damage)
            return False
        return True
