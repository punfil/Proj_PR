"""Graphics variables"""
bar_height = 0  # If 0 then removed
window_height = 600
window_width = 800
background_scale = 50
target_fps = 60
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
default_game_server_ip = "192.168.135.125"
configuration_receive_timeout = 1
configuration_receive_error = -1
socket_timeout = 100.00
full_server = -99
default_cache_save_file = "game_cookies.txt"

"""For information.action"""
information_update = 'u'
information_create = 'c'
information_disconnect = 'd'
information_death = 'i'

"""For information.type_of"""
information_tank = 't'
information_projectile = 'p'

"""Map editor"""
default_map_editor_tile = "./client/resources/grass.json"
map_editor_tiles = ["./client/resources/grass.json", "./client/resources/asphalt.json", "./client/resources/house.json"]
spawn_point_rotation_angle = 22.5
maps = {
    0: "./client/maps/tank_prix.json",
    1: "./client/maps/city.json",
    2: "./client/maps/all_your_base_are_belong_to_us.json",
    3: "./client/maps/flower.json",
    2137: "./client/maps/2137.json"
}
