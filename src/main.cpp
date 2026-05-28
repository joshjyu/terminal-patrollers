#include <ncurses.h>
#include <utility>

/// @brief Calculates Y and X coordinates to center text on the screen.
///
/// @param textLen Length of the text to be centered.
/// @param offsetY Vertical offset from the absolute center.
/// @return A pair of (y, x) screen coordinates.
std::pair<int, int> getCenteredCoords(int textLen, int offsetY = 0) {
    int maxY, maxX;

    // Gets size of the stdscr window
    getmaxyx(stdscr, maxY, maxX);

    // Calculates the x and y coordinates to center text
    int x = (maxX - textLen) / 2;
    int y = (maxY / 2) + offsetY;

    return {y, x};
}

/// @brief Initializes color pairs and sets the window background.
void initializeColors() {
    start_color();
    use_default_colors();

    // Pair 1: Map elements and standard text
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    // Pair 2: Player avatar @
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    // Pair 3: Patroller avatar P
    init_pair(3, COLOR_RED, COLOR_BLACK);

    bkgd(COLOR_PAIR(1));
}

/// @brief App entry point.
///
/// @return Exit status code.
int main() {
    initscr();
    curs_set(0);
    keypad(stdscr, TRUE);
    initializeColors();
    endwin();
    return 0;
}
