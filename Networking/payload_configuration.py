from ctypes import *


# Goal - receive C-like structs via socket
class PayloadConfiguration(Structure):
    _fields_ = [
        ("width", c_uint32),
        ("height", c_uint32),
        ("background_scale", c_uint32),
        ("player_count", c_uint32),  # Is that necessary?
        ("player_id", c_uint32),  # Is that necessary?
        ("tank_spawn_x", c_uint32),
        ("tank_spawn_y", c_uint32),
        ("map_number", c_uint32),
    ]
