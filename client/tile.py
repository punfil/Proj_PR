class Tile:
    """
    Represents single tile on the map
    Attributes:
        _x: X coordinate of the tile's location
        _y: Y coordinate of the tile's location
        _attributes: Dict containing the texture, movement speed, visibility, etc of a tile
    """
    def __init__(self, x, y, attributes):
        self._x = x
        self._y = y
        self._attributes = attributes

    def get_attribute(self, attribute_name):
        """
        Returns attribute of a given name
        :param str attribute_name: Name of the attribute to be returned
        :return: Value of the attribute
        :rtype: float
        """
        return self._attributes.get(attribute_name)

    def set_attribute(self, attribute_name, new_value):
        """
        Assigns new value to the given attribute
        :param str attribute_name: Name of the attribute to be modified
        :param float new_value: New value of the attributed to be modified
        :return: None
        """
        self._attributes[attribute_name] = new_value
        # may cause problems with changing all tiles of the same type at once, should probably copy the dict when doing that

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

