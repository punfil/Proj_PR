import pygame
from client.tile import Tile


class BackgroundBoard:
    """
    Represents the board with elements of the map like houses, trees, paths
    Attributes:
        _game: Game object
        _width: width of the board scaled
        _height: height of the board scaled
        ...other
    """
    def __init__(self, game, width, height, scale):
        self._game = game

        self._width = width // scale
        self._height = height // scale
        self._scale = scale
        self._background_board = [[None for _ in range(height)] for _ in range(width)]
        self._background_surface = pygame.Surface((width, height))
        self._updated_tiles = []

    def set_tile(self, x, y, tile):
        """
        Sets tile at grid position (x,y). Also updates the background surface
        :param int x: X coordinate of the tile to be set
        :param int y: Y coordinate of the tile to be set
        :param Tile tile: New Tile object to be set at given location
        :return: None
        """
        self._background_board[x][y] = tile
        self._background_surface.blit(tile.get_attribute("texture"), self.get_screen_position(x, y))
        self._updated_tiles.append(tile)

    def get_tile(self, x, y):
        """
        Returns tile at grid position (x,y)
        :param int x: X coordinate of the tile to be returned
        :param int y: Y coordinate of the tile to be returned
        :return: Tile at grid position (x, y)
        :rtype: Tile
        """
        return self._background_board[x][y]

    def get_screen_position(self, x, y):
        """
        Returns the screen position (in pixels) corresponding to the tile grid position (x,y)
        :param int x: X coordinate of the position on the grid
        :param int y: Y coordinate of the position on the grid
        :return: Position on the screen
        :rtype: (int, int)
        """
        # todo, I'm not sure if this method should be here - the background board logic doesn't have much to do with screen and pixels
        pos = (x * self._scale, y * self._scale)
        return pos

    def draw(self, screen, draw_all=False):
        """
        Draws the background board surface on screen.
        By default, draws only the tiles that were updated since the last draw. With draw_all=True, draws everything.
        :param screen: Screen we blit the board on
        :param draw_all: If the function should draw all the tiles no matter if they were update since last function call
        :type draw_all: bool or None
        :return: None
        """
        if draw_all:
            screen.blit(self._background_surface, (0, 0))
        else:
            for tile in self._updated_tiles:
                # we could do something fancy here, e.g. not blitting individual textures,
                # but instead blitting parts of self._background_surface, but I don't think it's necessary
                screen.blit(tile.get_attribute("texture"), self.get_screen_position(tile.x, tile.y))

        self._updated_tiles = []

    def serialize(self):
        """
        Serializes the board into a dict object
        :return: Dictionary containing the board
        :rtype: dict
        """

        board_data = {
            "width": self._width,
            "height": self._height,
        }
        tiles_to_chars = {}
        # dict containing {"tile_filename" : "C", ...} (C is a char representing the tile in tiles_string)

        tiles_string = ""  # string representing all the tiles on the board

        tile_char = 32  # decimal value of tile char

        for y in range(self._height):
            for x in range(self._width):
                tile_type = self._background_board[x][y].get_attribute("resource_name")
                char = tiles_to_chars.get(tile_type)
                if char is None:
                    if tile_char == 127:
                        raise OverflowError("Too many different tiles")
                    char = chr(tile_char)
                    tiles_to_chars[tile_type] = char
                    tile_char += 1
                tiles_string += char

        chars_to_tiles = {value: key for key, value in tiles_to_chars.items()}  # inverting the dict

        board_data["tiles"] = chars_to_tiles
        board_data["tiles_string"] = tiles_string

        return board_data

    def deserialize(self, board_data):
        """
        Deserializes the board from a dict object
        :param dict board_data: Dictionary containing serialized board
        :return: None
        """

        self._width = board_data["width"]
        self._height = board_data["height"]

        for x in range(self._width):
            for y in range(self._height):
                char = board_data["tiles_string"][x + y * self._width]
                tile_file = board_data["tiles"][char]
                tile_attributes = self._game.load_resource(tile_file)
                tile = Tile(x, y, tile_attributes)
                self.set_tile(x, y, tile)

    @property
    def width(self):
        return self._width

    @width.setter
    def width(self, value):
        self._width = value

    @property
    def height(self):
        return self._height

    @height.setter
    def height(self, value):
        self._height = value

    @property
    def scale(self):
        return self._scale

    @scale.setter
    def scale(self, value):
        self._scale = value

    @property
    def background_surface(self):
        return self._background_surface
