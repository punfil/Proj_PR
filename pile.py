from abc import ABC


class Pile(ABC):
    def __init__(self, x, y, object, texture):
        self._x = x
        self._y = y
        self._object = object  # When task is here, set to reference to tank, otherwise None
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
    def object(self):
        return self._object

    @object.setter
    def object(self, different_object):
        self._object = different_object

    @property
    def texture(self):
        return self._texture

