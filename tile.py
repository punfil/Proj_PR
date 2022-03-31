from abc import ABC


class Tile(ABC):
    def __init__(self, x, y, texture):
        # todo change texture to file and/or set all parameters from json file as self variables (not sure if necessary)
        self._x = x
        self._y = y
        self._texture = texture

    # Getter of x
    @property
    def x(self):
        return self._x

    @x.setter
    def x(self, value):
        self._x = value

    # Getter of y
    @property
    def y(self):
        return self._y

    @y.setter
    def y(self, value):
        self._y = value

    @property
    def texture(self):
        return self._texture

