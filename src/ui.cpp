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

/// @brief Displays an error message and waits for a keypress.
///
/// @param message The error message to display.
void showErrorScreen(const std::string &message) {
    clear();
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    // Truncate error message
    std::string display =
        message.size() > (size_t)maxX ? message.substr(0, maxX) : message;

    auto [y, x] = getCenteredCoords(display.size(), -1);
    mvaddstr(y, x, display.c_str());

    std::string prompt = "Press any key to continue.";
    auto [promptY, promptX] = getCenteredCoords(prompt.size(), 1);
    mvaddstr(promptY, promptX, prompt.c_str());

    refresh();
    getch();
}

/// @brief Renders a controls hint at the bottom of the screen.
///
/// @param text The instruction text to display.
void showControls(const std::string &text) {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    std::string display =
        text.size() > (size_t)maxX ? text.substr(0, maxX) : text;
    mvaddstr(maxY - 1, (maxX - (int)display.size()) / 2, display.c_str());
}

/// @brief Displays a loading message and refreshes the screen.
///
/// @param message The message to display.
void showLoadingScreen(const std::string &message) {
    clear();
    auto [y, x] = getCenteredCoords(message.size());
    mvaddstr(y, x, message.c_str());
    refresh();
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

        showControls("[W][S] or [UP][DOWN] to select  |  [ENTER] to confirm");
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

/// @brief Draws and handles the location selection menu.
///
/// @param names Display names for each selectable location.
/// @return Index of the selected location.
int runLocationMenu(const std::vector<std::string> &names) {
    int currentSelection = 0;

    while (true) {
        clear();

        std::string title = "SELECT A LOCATION";
        auto [titleY, titleX] = getCenteredCoords(title.size(), -4);
        mvaddstr(titleY, titleX, title.c_str());

        // Add '>' marker next to selection; highlight selection
        for (int i = 0; i < (int)names.size(); i++) {
            std::string label =
                (i == currentSelection) ? "> " + names[i] : "  " + names[i];
            auto [y, x] = getCenteredCoords(label.size(), -1 + i);

            // Highlight the selection
            if (i == currentSelection) {
                attron(A_REVERSE); // Swaps FG and BG
                mvaddstr(y, x, label.c_str());
                attroff(A_REVERSE);
            } else {
                mvaddstr(y, x, label.c_str());
            }
        }

        showControls("[W][S] or [UP][DOWN] to select  |  [ENTER] to confirm");
        refresh();
        int key = getch();

        // Handles WASD and Arrow Keys and Enter keyboard inputs
        if (key == KEY_UP || key == 'w' || key == 'W')
            currentSelection = std::max(0, currentSelection - 1);
        else if (key == KEY_DOWN || key == 's' || key == 'S')
            currentSelection =
                std::min((int)names.size() - 1, currentSelection + 1);
        else if (key == KEY_ENTER || key == 10 || key == 13)
            return currentSelection;
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
        std::string desc = "A stealth game mapped to real locations!";
        std::string costNote =
            "NOTE: Requires active microservice network connections.";

        auto [titleY, titleX] = getCenteredCoords(title.size(), -4);
        auto [descY, descX] = getCenteredCoords(desc.size(), -2);
        auto [noteY, noteX] = getCenteredCoords(costNote.size(), 0);
        mvaddstr(titleY, titleX, title.c_str());
        mvaddstr(descY, descX, desc.c_str());
        mvaddstr(noteY, noteX, costNote.c_str());

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

        showControls("[W][S] or [UP][DOWN] to select  |  [ENTER] to confirm");
        refresh();
        int key = getch();

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

/// @brief Displays the caught/game over screen.
void showCaughtScreen() {
    clear();
    std::string title = "YOU WERE CAUGHT";
    std::string prompt = "Press any key to return to the menu.";
    auto [titleY, titleX] = getCenteredCoords(title.size(), -1);
    auto [promptY, promptX] = getCenteredCoords(title.size(), 1);

    attron(A_BOLD);
    mvaddstr(titleY, titleX, title.c_str());
    attroff(A_BOLD);
    mvaddstr(promptY, promptX, prompt.c_str());
    refresh();
    napms(1000); // Sleep for 1 second to prevent accidental exit
    getch();
}

/// @brief Displays the escaped/win screen.
void showEscapedScreen() {
    clear();
    std::string title = "YOU ESCAPED!";
    std::string prompt = "Press any key to return to the menu.";
    auto [titleY, titleX] = getCenteredCoords(title.size(), -1);
    auto [promptY, promptX] = getCenteredCoords(title.size(), 1);

    attron(A_BOLD);
    mvaddstr(titleY, titleX, title.c_str());
    attroff(A_BOLD);
    mvaddstr(promptY, promptX, prompt.c_str());
    refresh();
    napms(1000); // Sleep for 1 second to prevent accidental exit
    getch();
}

/// @brief Initializes color pairs and sets the window background.
void initializeColors() {
    start_color();

    // If user's terminal can change colors,
    // define white and black
    if (can_change_color()) {
        init_color(COLOR_BLACK, 0, 0, 0);
        init_color(COLOR_WHITE, 1000, 1000, 1000);
    }

    // Pair 1: Map elements and standard text
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    // Pair 2: Player avatar @
    init_pair(2, COLOR_GREEN, COLOR_WHITE);
    // Pair 3: Patroller avatar P
    init_pair(3, COLOR_RED, COLOR_WHITE);
    // Pair 4: Patroller detection radius
    init_pair(4, COLOR_RED, COLOR_WHITE);
    // Pair 5: Exit tile E
    init_pair(5, COLOR_CYAN, COLOR_WHITE);

    bkgd(COLOR_PAIR(1));
}
