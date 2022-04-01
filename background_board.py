class BackgroundBoard:
    def __init__(self, width, height, scale):
        self._width = int(width/scale)
        self._height = int(height/scale)
        self._scale = scale
        self._background_board = [[None for _ in range(height)] for _ in range(width)]

    def setUpTile(self, x, y, tile):
        self._background_board[x][y] = tile

    def getTile(self, x, y):
        return self._background_board[x][y]

    def getXForDrawing(self, x):
        return x*self._scale

    def getYForDrawing(self, y):
        return y*self._scale

    @property
    def scale(self):
        return self._scale

    @scale.setter
    def scale(self, value):
        self._scale = value


