import pygame


class BackgroundBoard:
    def __init__(self, width, height, scale):
        self._width = width // scale
        self._height = height // scale
        self._scale = scale
        self._background_board = [[None for _ in range(height)] for _ in range(width)]
        self._background_surface = pygame.Surface((width, height))
        self._updated_tiles = []

    def set_tile(self, x, y, tile):
        """sets tile at grid position (x,y). Also updates the background surface"""
        self._background_board[x][y] = tile
        self._background_surface.blit(tile.get_attribute("texture"), self.get_screen_position(x, y))
        self._updated_tiles.append(tile)

    def get_tile(self, x, y):
        """returns tile at grid position (x,y)"""
        return self._background_board[x][y]

    def get_screen_position(self, x, y):
        """returns the screen position (in pixels) corresponding to the tile grid position (x,y)"""
        # todo, I'm not sure if this method should be here - the background board logic doesn't have much to do with screen and pixels
        pos = (x*self._scale, y*self._scale)
        return pos

    def draw(self, screen, draw_all=False):
        """draws the background board surface on screen.
        By default, draws only the tiles that were updated since the last draw. With draw_all=True, draws everything.
        """
        if draw_all:
            screen.blit(self._background_surface, (0, 0))
        else:
            for tile in self._updated_tiles:
                # we could do something fancy here, eg. not blitting individual textures,
                # but instead blitting parts of self._background_surface, but I don't think it's necessary
                screen.blit(tile.get_attribute("texture"), self.get_screen_position(tile.x, tile.y))

        self._updated_tiles = []

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
