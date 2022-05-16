bar_height = 0  # If 0 then removed
window_height = 600
window_width = 800

background_scale = 50

object_collision_damage = 0.5
object_collision_cooldown = 0.5
object_collision_speed_multiplier = 0.5

max_projectile_count = 20

hp_bar_width = 50
hp_bar_height = 5
hp_bar_y_offset = -25
hp_bar_filled_color = "#00ff00"
hp_bar_empty_color = "#ff0000"

shield_bar_width = 50
shield_bar_height = 5
shield_bar_y_offset = -30
shield_bar_filled_color = "#0077ff"
shield_bar_empty_color = "#cccccc"

# Networking variables
game_port = 2137
default_game_server_ip = "192.168.0.21"
information_update = 'u'
information_create = 'c'
information_disconnect = 'd'
information_tank = 't'
information_projectile = 'p'
information_turret = 'r'
projectile_exists = 1
projectile_not_exists = 0
receiver_sleep_time = 0.001
configuration_receive_timeout = 1
configuration_receive_error = -1
socket_timeout = 100.00
main_loop_per_second = 30 # Remember to change COMMUNICATION_INTERVAL ON SERVER

spawn_point_rotation_angle = 22.5
default_map_editor_tile = "./resources/grass.json"
map_editor_tiles = ["./resources/grass.json", "./resources/asphalt.json", "./resources/house.json"]
