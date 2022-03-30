class Board:
    def __init__(self, width, height):
        self._height = height
        self._width = width
        self._board = [[None for x in range(height)] for y in range(width)]  # Initialize the board

    def setUpPile(self, x, y, pile):
        self._board[x][y] = pile

    def getPile(self, x, y):
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
