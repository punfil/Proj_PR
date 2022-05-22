import json

from game import Game
from tile import Tile
from Boards.background_board import BackgroundBoard
import pygame
import constants


class MapEditor(Game):
    def __init__(self):
        super().__init__()

        self._cursor_image = None
        self._cursors = None
        self._cursors_sprites_group = None

        self._spawn_point_image = None
        self._spawn_points = None
        self._spawn_points_sprites_group = None

    def setup(self):
        """initializes all variables, loads data from server"""

        self._width = constants.window_width  # Those values need to be downloaded from socket
        self._height = constants.window_height
        self._background_scale = constants.background_scale

        pygame.init()
        pygame.display.set_caption("Project - Distracted Programming")

        self._screen = pygame.display.set_mode((self._width, self._height))
        self._clock = pygame.time.Clock()

        self._background_board = BackgroundBoard(self, self._width, self._height, self._background_scale)
        self.fill_board(constants.default_map_editor_tile)

        self._cursor_image = pygame.image.load("pictures/editor_cursor.png")
        cursor_names = ["normal", "x_symmetry", "y_symmetry", "point_symmetry"]
        self._cursors = {}
        self._cursors_sprites_group = pygame.sprite.Group()
        for i in range(4):
            cursor = pygame.sprite.Sprite()
            cursor.image = self._cursor_image
            cursor.rect = self._cursor_image.get_rect()
            self._cursors[cursor_names[i]] = cursor
            self._cursors_sprites_group.add(cursor)

        self._spawn_point_image = pygame.image.load("pictures/spawn_point.png")
        self._spawn_points = []
        self._spawn_points_sprites_group = pygame.sprite.Group()

    def fill_board(self, tile_filename):
        """fills entire board with a specific tile"""

        for x in range(self._background_board.width):
            for y in range(self._background_board.height):
                tile_attributes = self.load_resource(tile_filename)
                self._background_board.set_tile(x, y, Tile(x, y, tile_attributes))

    def save(self, filename):
        """saves the map to the specified file"""

        save_data = {
            "spawn_points": self._spawn_points,
            "map_data": self._background_board.serialize()
        }

        with open(filename, 'w') as file:
            json.dump(save_data, file)

    def set_cursor_positions(self, grid_x, grid_y, symmetry_x, symmetry_y):
        """sets all cursor positions to all four possible combinations of grid (x, y) and symmetry (x,y)"""
        scale = self._background_scale
        self._cursors["normal"].rect.topleft = grid_x*scale, grid_y*scale
        self._cursors["x_symmetry"].rect.topleft = symmetry_x*scale, grid_y*scale
        self._cursors["y_symmetry"].rect.topleft = grid_x*scale, symmetry_y*scale
        self._cursors["point_symmetry"].rect.topleft = symmetry_x*scale, symmetry_y*scale

    def hide_cursor(self, cursor_name):
        """hides cursor with the specified name. If it was already hidden, nothing happens"""
        self._cursors[cursor_name].kill()

    def show_cursor(self, cursor_name):
        """shows cursor with the specified name. If it was already visible, nothing happens"""
        if not self._cursors[cursor_name].alive():
            self._cursors_sprites_group.add(self._cursors[cursor_name])

    def toggle_cursor(self, cursor_name):
        """toggles the cursor visibility: visible->hidden or hidden->visible"""
        if not self._cursors[cursor_name].alive():
            self.show_cursor(cursor_name)
        else:
            self.hide_cursor(cursor_name)

    def toggle_spawn_point(self, x, y):
        """adds a new spawn point if the given position didn't contain one already. If it did - removes it"""
        exists = False
        for i in range(len(self._spawn_points)):
            sp = self._spawn_points[i]
            if sp[0] == x and sp[1] == y:
                exists = True
                self._spawn_points.pop(i)
                self._spawn_points_sprites_group.sprites()[i].kill()
                break

        if not exists:
            new_spawn_point = [x, y, 0]
            self._spawn_points.append(new_spawn_point)
            sp_sprite = pygame.sprite.Sprite()
            sp_sprite.image = self._spawn_point_image
            sp_sprite.rect = sp_sprite.image.get_rect()
            sp_sprite.rect.center = (x * self._background_scale + self._background_scale/2,
                                     y * self._background_scale + self._background_scale/2)
            self._spawn_points_sprites_group.add(sp_sprite)

    def rotate_spawn_point(self, x, y, angle):
        """rotates the spawn point at position (x, y) by a given angle.
        Does nothing if no spawn point is in that position"""

        for i in range(len(self._spawn_points)):
            sp = self._spawn_points[i]
            if sp[0] == x and sp[1] == y:
                self._spawn_points[i][2] += angle
                sprite = self._spawn_points_sprites_group.sprites()[i]
                sprite.image = pygame.transform.rotozoom(self._spawn_point_image, self._spawn_points[i][2], 1)
                sprite.rect = sprite.image.get_rect()
                sprite.rect.center = (x * self._background_scale + self._background_scale / 2,
                                      y * self._background_scale + self._background_scale / 2)

    def run(self):
        """runs the map editor"""

        self._background_board.draw(self._screen, draw_all=True)

        x_symmetry = False
        y_symmetry = False
        point_symmetry = False

        self.hide_cursor("x_symmetry")
        self.hide_cursor("y_symmetry")
        self.hide_cursor("point_symmetry")

        tile_index = 0
        current_tile = constants.map_editor_tiles[tile_index]

        while True:
            self._clock.tick(60)

            self._background_board.draw(self._screen)

            pygame.display.set_caption("Project - Distracted Programming (Map Editor)"
                                       + str(int(self._clock.get_fps())) + " fps")

            mouse_position = pygame.mouse.get_pos()

            grid_x, grid_y = self.screen_position_to_grid_position(mouse_position[0], mouse_position[1])
            symmetry_x = self._background_board.width - grid_x - 1
            symmetry_y = self._background_board.height - grid_y - 1
            self.set_cursor_positions(grid_x, grid_y, symmetry_x, symmetry_y)

            for ev in pygame.event.get():
                if ev.type == pygame.QUIT or (ev.type == pygame.KEYDOWN and ev.key == pygame.K_ESCAPE):
                    self.exit_game(False)
                if ev.type == pygame.KEYDOWN:
                    if ev.key == pygame.K_x:
                        x_symmetry = not x_symmetry
                        point_symmetry = False
                        self.toggle_cursor("x_symmetry")
                    elif ev.key == pygame.K_y:
                        y_symmetry = not y_symmetry
                        point_symmetry = False
                        self.toggle_cursor("y_symmetry")
                    elif ev.key == pygame.K_p:
                        point_symmetry = not point_symmetry
                        self.toggle_cursor("point_symmetry")
                        if point_symmetry:
                            x_symmetry = False
                            y_symmetry = False
                            self.hide_cursor("x_symmetry")
                            self.hide_cursor("y_symmetry")

                    elif ev.key == pygame.K_c:
                        tile_index = (tile_index + 1) % len(constants.map_editor_tiles)
                        current_tile = constants.map_editor_tiles[tile_index]

                    elif ev.key == pygame.K_t:
                        self.toggle_spawn_point(grid_x, grid_y)
                    elif ev.key == pygame.K_r:
                        self.rotate_spawn_point(grid_x, grid_y, constants.spawn_point_rotation_angle)

                    elif ev.key == pygame.K_s:
                        self.save("save.json")

            if (x_symmetry and y_symmetry) or point_symmetry:
                self.show_cursor("point_symmetry")
            else:
                self.hide_cursor("point_symmetry")

            if pygame.mouse.get_pressed()[0]:
                tile = Tile(grid_x, grid_y, self.load_resource(current_tile))
                self._background_board.set_tile(grid_x, grid_y, tile)

                if x_symmetry:
                    tile = Tile(symmetry_x, grid_y, self.load_resource(current_tile))
                    self._background_board.set_tile(symmetry_x, grid_y, tile)
                if y_symmetry:
                    tile = Tile(grid_x, symmetry_y, self.load_resource(current_tile))
                    self._background_board.set_tile(grid_x, symmetry_y, tile)
                if (x_symmetry and y_symmetry) or point_symmetry:
                    tile = Tile(symmetry_x, symmetry_y, self.load_resource(current_tile))
                    self._background_board.set_tile(symmetry_x, symmetry_y, tile)

            self._spawn_points_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._cursors_sprites_group.clear(self._screen, self._background_board.background_surface)
            self._spawn_points_sprites_group.draw(self._screen)
            self._cursors_sprites_group.draw(self._screen)

            pygame.display.flip()


if __name__ == "__main__":
    editor = MapEditor()
    editor.setup()
    editor.run()
