#include "ui.h"

#include <ncurses.h>

/// @brief App entry point.
///
/// @return Exit status code.
int main() {
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    initializeColors();

    while (true) {
        int selection = runMainMenu();
        if (selection == 0) {
            // Game Loop Placeholder
        } else if (selection == 1) {
            if (runConfirmExit())
                break;
        }
    }

    endwin();
    return 0;
}
