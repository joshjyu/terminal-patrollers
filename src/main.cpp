#include "game.h"
#include "ui.h"
#include <ncurses.h>

/// @brief App entry point.
///
/// @return Exit status code.
int main()
{
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    initializeColors();

    while (true)
    {
        int selection = runMainMenu();

        // 0 = PLAY
        // 1 = QUIT
        if (selection == 0)
        {
            const auto &map = getTestMap();
            while (true)
            {
                clear();
                renderMap(map);
                refresh();
                int key = getch();
                if (key == 27)
                    break; // ESC returns to main menu
            }
        }
        else if (selection == 1)
        {
            if (runConfirmExit())
                break;
        }
    }

    endwin();
    return 0;
}
