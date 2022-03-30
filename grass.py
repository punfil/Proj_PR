from pile import Pile
import constants


class Grass(Pile):
    def __init__(self, x, y, object, texture):
        super().__init__(x, y, object, texture)  # !!!!Add texture for grass loaded from file
        self.__object_transparency = 100  # Object will be less visible in the grass by the enemy
        self.__movement_speed = constants.default_movement_speed - constants.grass_movement_difficulty

