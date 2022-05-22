from game import Game


def main():
    my_game = Game()
    if my_game.setup():
        my_game.play()


if __name__ == "__main__":
    main()
