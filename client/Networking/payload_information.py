from ctypes import *


class PayloadInformation(Structure):
    """
    Represents the information received from the server
    """
    _fields_ = [
        ("action", c_char),
        ("type_of", c_char),
        ("player_id", c_int32),
        ("x_location", c_float),
        ("y_location", c_float),
        ("tank_angle", c_float),
        ("hp", c_float),
        ("turret_angle", c_float),
    ]
