from pile import Pile


class Asphalt(Pile):
    def __init__(self, x, y, object, texture):
        super().__init__(x, y, object, texture)  # !Add asphalt loaded from file
