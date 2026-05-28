#include <ncurses.h>
#include <string>
#include <utility>
#include <vector>

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

/// @brief Prompts the user to confirm exiting.
///
/// @return True if the user confirms exit. False otherwise.
bool runConfirmExit() {
    std::vector<std::string> options = {"YES", "NO"};

    // Default to NO
    int currentSelection = 1;

    while (true) {
        clear();

        std::string warnTitle = "Exit Game?";
        std::string warnDesc = "Are you sure you want to quit?";

        // Print the title and description
        auto [titleY, titleX] = getCenteredCoords(warnTitle.size(), -2);
        auto [descY, descX] = getCenteredCoords(warnDesc.size(), -1);
        mvaddstr(titleY, titleX, warnTitle.c_str());
        mvaddstr(descY, descX, warnDesc.c_str());

        // Add dynamic ">" marker next to selections
        for (int i = 0; i < (int)options.size(); i++) {
            std::string label =
                (i == currentSelection) ? "> " + options[i] : "  " + options[i];
            auto [y, x] = getCenteredCoords(label.size(), 1 + i);

            // Highlight the selection by swapping the FG and BG
            if (i == currentSelection) {
                attron(A_REVERSE);
                mvaddstr(y, x, label.c_str());
                attroff(A_REVERSE);
            } else {
                mvaddstr(y, x, label.c_str());
            }
        }

        refresh();
        int key = getch();

        if (key == KEY_UP || key == 'w' || key == 'W')
            currentSelection = std::max(0, currentSelection - 1);
        else if (key == KEY_DOWN || key == 's' || key == 'S')
            currentSelection =
                std::min((int)options.size() - 1, currentSelection + 1);
        else if (key == KEY_ENTER || key == 10 || key == 13)
            return currentSelection == 0;
    }
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
