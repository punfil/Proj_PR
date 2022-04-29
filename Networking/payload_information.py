from ctypes import *


# Goal - receive C-like structs via socket
class PayloadInformation(Structure):
    _fields_ = [
        ("action", c_char),
        ("type_of", c_char),
        ("player_id", c_uint32),
        ("x_location", c_uint32),
        ("y_location", c_uint32),
        ("tank_angle", c_float),
        ("hp", c_float),
        ("turret_angle", c_float),
    ]