from game import Game

my_game = Game()
if my_game.setup():
    my_game.play()
else:
    print("INFO: Error related to server connection\n")
