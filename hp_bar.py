import pygame
import constants


class HPBar(pygame.sprite.Sprite):
    def __init__(self, tank):
        super().__init__()

        self._tank = tank

        self._width = constants.hp_bar_width
        self._height = constants.hp_bar_height
        self._y_offset = constants.hp_bar_y_offset

        self.image = pygame.surface.Surface((self._width, self._height))
        self.image.fill("#00ff00")

        self.rect = self.image.get_rect()
        self.rect.center = (self._tank.x, self._tank.y)

    def update(self):
        self.rect.center = (self._tank.x, self._tank.y + self._y_offset)

    def update_hp(self):
        hp_fraction = self._tank.hp / self._tank.max_hp

        self.image.fill("#00ff00")
        self.image.fill("#ff0000", (hp_fraction * self._width, 0, self._width, self._height))
