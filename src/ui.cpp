#include "ui.h"

#include <algorithm>
#include <ncurses.h>
#include <string>
#include <vector>

/// @brief Calculates Y and X coordinates to center text on the screen.
///
/// @param textLen Length of the text to be centered.
/// @param offsetY Vertical offset from the absolute center.
/// @return A pair of (y, x) screen coordinates.
std::pair<int, int> getCenteredCoords(int textLen, int offsetY) {
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

            // Highlight the selection
            if (i == currentSelection) {
                attron(A_REVERSE); // Swaps FG and BG
                mvaddstr(y, x, label.c_str());
                attroff(A_REVERSE);
            } else {
                mvaddstr(y, x, label.c_str());
            }
        }

        refresh();
        int key = getch();

        // Handles WASD and Arrow Keys and Enter keyboard inputs
        if (key == KEY_UP || key == 'w' || key == 'W')
            currentSelection = std::max(0, currentSelection - 1);
        else if (key == KEY_DOWN || key == 's' || key == 'S')
            currentSelection =
                std::min((int)options.size() - 1, currentSelection + 1);
        else if (key == KEY_ENTER || key == 10 || key == 13)
            // Returns True if select YES, otherwise False
            return currentSelection == 0;
        else if (key == KEY_RESIZE)
            clear();
    }
}

/// @brief Draws and handles the main menu.
///
/// @return Index of the selected option (0 for Play, 1 for Quit).
int runMainMenu() {
    std::vector<std::string> options = {"PLAY", "QUIT"};
    int currentSelection = 0; // Default PLAY

    while (true) {
        clear();

        std::string title = "TERMINAL PATROLLERS";
        std::string desc = "A stealth game mapped to real-world city streets!";
        std::string costNote =
            "NOTE: Requires an active microservice network connection.";

        // Print text
        auto [titleY, titleX] = getCenteredCoords(title.size(), -4);
        auto [descY, descX] = getCenteredCoords(desc.size(), -2);
        auto [noteY, noteX] = getCenteredCoords(costNote.size(), 0);
        mvaddstr(titleY, titleX, title.c_str());
        mvaddstr(descY, descX, desc.c_str());
        mvaddstr(noteY, noteX, costNote.c_str());

        // Add dynamic ">" marker next to selections
        for (int i = 0; i < (int)options.size(); i++) {
            std::string label =
                (i == currentSelection) ? "> " + options[i] : "  " + options[i];
            auto [y, x] = getCenteredCoords(label.size(), 3 + i);

            // Highlight the selection
            if (i == currentSelection) {
                attron(A_REVERSE); // Swaps FG and BG
                mvaddstr(y, x, label.c_str());
                attroff(A_REVERSE);
            } else {
                mvaddstr(y, x, label.c_str());
            }
        }

        refresh();
        int key = getch();

        // Handles WASD and Arrow Keys and Enter keyboard inputs
        if (key == KEY_UP || key == 'w' || key == 'W')
            currentSelection = std::max(0, currentSelection - 1);
        else if (key == KEY_DOWN || key == 's' || key == 'S')
            currentSelection =
                std::min((int)options.size() - 1, currentSelection + 1);
        else if (key == KEY_ENTER || key == 10 || key == 13)
            return currentSelection;
        else if (key == KEY_RESIZE)
            clear();
    }
}

/// @brief Initializes color pairs and sets the window background.
void initializeColors() {
    start_color();
    use_default_colors();

    // If user's terminal can change colors,
    // define white and black
    if (can_change_color()) {
        init_color(COLOR_BLACK, 0, 0, 0);
        init_color(COLOR_WHITE, 1000, 1000, 1000);
    }

    // Pair 1: Map elements and standard text
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    // Pair 2: Player avatar @
    init_pair(2, COLOR_YELLOW, COLOR_WHITE);
    // Pair 3: Patroller avatar P
    init_pair(3, COLOR_RED, COLOR_WHITE);

    bkgd(COLOR_PAIR(1));
}
