import pygame


class HPBar(pygame.sprite.Sprite):
    """
    Represents the HP Bar displayed under the tank showing it's HP
    Attributes:
        _tank: Tank object it is attached to
        ...other
    """
    def __init__(self, tank, max_hp, width, height, y_offset, filled_color, empty_color):
        super().__init__()

        self._tank = tank
        self._max_hp = max_hp
        self._filled_color = filled_color
        self._empty_color = empty_color

        self._width = width
        self._height = height
        self._y_offset = y_offset

        self.image = pygame.surface.Surface((self._width, self._height))
        self.image.fill(self._filled_color)

        self.rect = self.image.get_rect()
        self.rect.center = (self._tank.x, self._tank.y)

    def update(self):
        """
        Updates the location of the bar on the screen according to the tank's position
        :return: None
        """
        self.rect.center = (self._tank.x, self._tank.y - self._y_offset)

    def update_hp(self, new_hp):
        """
        Update the bar according to the tank's HP
        :param float new_hp: New HP value to be displayed
        :return: None
        """
        hp_fraction = new_hp / self._max_hp

        self.image.fill(self._filled_color)
        self.image.fill(self._empty_color, (hp_fraction * self._width, 0, self._width, self._height))
