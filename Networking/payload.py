from ctypes import *


# Goal - receive C-like structs via socket
class Payload(Structure):
    _fields_ = [
        ("player_id", c_uint32),
        ("x_location", c_uint32),
        ("y_location", c_uint32),
    ]