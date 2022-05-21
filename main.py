from game import Game


def main():
    my_game = Game()
    if my_game.setup():
        my_game.play()
    else:
        print("INFO: Error related to server connection\n")


if __name__ == "__main__":
    main()
