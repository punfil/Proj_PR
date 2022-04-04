class Tile:
    def __init__(self, x, y, attributes):
        self._x = x
        self._y = y
        self._attributes = attributes  # dict containing the texture, movement speed, visibility, etc, of a tile

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

    def get_attribute(self, attribute_name):
        return self._attributes[attribute_name]  # todo maybe replace with self._attributes.get(attribute_name)

    def set_attribute(self, attribute_name, new_value):
        self._attributes[attribute_name] = new_value
        # may cause problems with changing all tiles of the same type at once, should probably copy the dict when doing that
