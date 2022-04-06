# from game import Game
#
# my_game = Game()
# my_game.setup()
# my_game.play()
from Networking.connection import *

my_connection = Connection()
my_connection.establish_connection()
my_connection.send_single_payload()
my_connection.close_connection()