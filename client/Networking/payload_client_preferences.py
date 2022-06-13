from ctypes import *


class PayloadClientPreferences(Structure):
    """
    Represents the configuration that is received from the server
    """
    _fields_ = [
        ("tank_version", c_uint32),
        ("tank_max_hp", c_float),
    ]
