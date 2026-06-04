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
            getmaxyx(stdscr, maxY, maxX);
            // Centers the map in the terminal
            int originY = (maxY - (int)map.size()) / 2;
            int originX = (maxX - (int)map[0].size()) / 2;

            Player player = {2, 2}; // Player's avatar origin

            while (true) {
                erase();
                renderMap(map, originY, originX);

                // Render avatar
                attron(COLOR_PAIR(2));
                mvaddch(originY + player.y, originX + player.x, '@');
                attroff(COLOR_PAIR(2));

                refresh();

                int key = getch();
                if (key == 27) break; // ESC returns to main menu

                if (key == KEY_RESIZE) {
                    getmaxyx(stdscr, maxY, maxX);
                    originY = (maxY - (int)map.size()) / 2;
                    originX = (maxX - (int)map[0].size()) / 2;
                    continue;
                }

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
