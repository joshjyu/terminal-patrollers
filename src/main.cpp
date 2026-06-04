#include "game.h"
#include "ui.h"
#include <ncurses.h>

/// @brief App entry point.
///
/// @return Exit status code.
int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    initializeColors();

    while (true) {
        int selection = runMainMenu();

        // 0 = PLAY
        // 1 = QUIT
        if (selection == 0) {
            const auto &map = getTestMap();
            int maxY, maxX;

            Player player = {2, 2}; // Player's avatar origin

            while (true) {
                getmaxyx(stdscr, maxY, maxX);
                // Camera tracking, centers on player
                int originY = (maxY / 2) - player.y;
                int originX = (maxX / 2) - player.x;

                erase();

                renderMap(map, originY, originX);

                // Render avatar
                attron(COLOR_PAIR(2));
                mvaddch(originY + player.y, originX + player.x, '@');
                attroff(COLOR_PAIR(2));

                refresh();

                int key = getch();
                if (key == 27) break; // ESC returns to main menu
                if (key == KEY_RESIZE) continue;

                // New avatar position after step
                auto [newY, newX] = calculateNewPos(key, player.y, player.x);
                if (isValidMove(map, newY, newX)) {
                    player = {newY, newX};
                }
            }
        } else if (selection == 1) {
            if (runConfirmExit()) break;
        }
    }

    endwin();
    return 0;
}
