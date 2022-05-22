"""Graphics variables"""
bar_height = 0  # If 0 then removed
window_height = 600
window_width = 800
background_scale = 50
# HP BAR
hp_bar_width = 50
hp_bar_height = 5
hp_bar_y_offset = -25
hp_bar_filled_color = "#00ff00"
hp_bar_empty_color = "#ff0000"
# SHIELD BAR
shield_bar_width = 50
shield_bar_height = 5
shield_bar_y_offset = -30
shield_bar_filled_color = "#0077ff"
shield_bar_empty_color = "#cccccc"

death_screen_display_time_sec = 3
server_full_or_busy_screen_display_time_sec = 2

"""Physics constants"""
object_collision_damage = 0.5
object_collision_cooldown = 0.5
object_collision_speed_multiplier = 0.5

"""Projectiles constants"""
max_projectile_count = 10
projectile_exists = 1
projectile_not_exists = 0

"""Networking variables"""""
game_port = 2137
default_game_server_ip = "192.168.0.21"
configuration_receive_timeout = 1
configuration_receive_error = -1
socket_timeout = 100.00
full_server = -99

"""For information.action"""
information_update = 'u'
information_create = 'c'
information_disconnect = 'd'
information_death = 'i'

"""For information.type_of"""
information_tank = 't'
information_projectile = 'p'

"""Map editor"""
default_map_editor_tile = "./resources/grass.json"
map_editor_tiles = ["./resources/grass.json", "./resources/asphalt.json", "./resources/house.json"]
spawn_point_rotation_angle = 22.5
