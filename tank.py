import constants


class Tank:
    def __init__(self, player_no, x, y):
        self.__player_no = player_no
        self._x = x  # Should be float
        self._y = y  # Should be float
        self._speed = constants.default_movement_speed
        #self.__texture !! add load texture

    @property
    def x(self):
        return self.__X

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
