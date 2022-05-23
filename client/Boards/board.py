class TankBoard:
    """
    Represents the board tanks are located on.
    Attributes:
        _height: height of the board
        _width: width of the board
        _board: 2D Array
    """
    def __init__(self, width, height):
        self._height = height
        self._width = width
        self._board = [[None for _ in range(height)] for _ in range(width)]  # Initialize the board

    def setUpTile(self, x, y, tile):
        """
        Sets the tile at a given position to tile object given
        :param int x: X coordinate of the tile to be set
        :param int y: Y coordinate of the tile to be set
        :param Tile tile: Tile object to be set at the given location
        :return: None
        """
        self._board[x][y] = tile

    def getTile(self, x, y):
        """
        Returns tile at a given position
        :param int x: X coordinate of the tile to be returned
        :param int y: Y coordinate of the tile to be returned
        :return: Tile at the given location
        :rtype: Tile
        """
        return self._board[x][y]

    @property
    def height(self):
        return self._height

    @height.setter
    def height(self, value):
        self._height = value

    @property
    def width(self):
        return self._width
