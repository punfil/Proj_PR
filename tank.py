import constants

# Will this class be even required?

class Tank:
    def __init__(self, player_no, x, y):
        self.__player_no = player_no
        self._x = x  # Should be float
        self._y = y  # Should be float
        self._speed = constants.default_movement_speed
        self._hp = constants.default_HP

    @property
    def x(self):
        return self._x

    @x.setter
    def x(self, value):
        self._x = value

    def offsetX(self, value):
        self._x += value

    @property
    def y(self):
        return self._y

    @y.setter
    def y(self, value):
        self._y = value

    def offsetY(self, value):
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
        self._jp += value