#include "ui.h"

#include <algorithm>
#include <cstdio>
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
        else if (key == 27)
            return false;
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

        showControls("[W][S] or [UP][DOWN] to select, [ENTER] to confirm");
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
        else if (key == 27)
            return -1;
        else if (key == KEY_RESIZE)
            clear();
    }
}

/// @brief Draws and handles the options menu.
///
/// @param settings The settings object to read from and write to.
void runOptionsMenu(Settings &settings) {
    int currentSelection = 0;
    const int numOptions = 2;

    while (true) {
        clear();
        std::string title = "OPTIONS";
        auto [titleY, titleX] = getCenteredCoords(title.size(), -4);
        mvaddstr(titleY, titleX, title.c_str());

        std::string modeVal = settings.darkMode ? "ON" : "OFF";
        std::string densityVal =
            std::to_string(settings.patrollerDensity).substr(0, 4);
        std::string opts[numOptions] = {
            "Dark Mode:           < " + modeVal + " >",
            "Patroller Density:   < " + densityVal + " >",
        };

        for (int i = 0; i < (int)numOptions; i++) {
            auto [y, x] = getCenteredCoords(opts[i].size(), -1 + i);

            // Highlight the selection
            if (i == currentSelection) {
                attron(A_REVERSE);
                mvaddstr(y, x, opts[i].c_str());
                attroff(A_REVERSE);
            } else {
                mvaddstr(y, x, opts[i].c_str());
            }
        }

        showControls("[W][S] to select, [A][D] to adjust, [ENTER] to confirm, "
                     "[ESC] to return");
        refresh();

        int key = getch();

        if (key == KEY_UP || key == 'w' || key == 'W')
            currentSelection = std::max(0, currentSelection - 1);
        else if (key == KEY_DOWN || key == 's' || key == 'S')
            currentSelection = std::min(numOptions - 1, currentSelection + 1);
        else if (key == KEY_LEFT || key == 'a' || key == 'A') {
            if (currentSelection == 0) {
                settings.darkMode = !settings.darkMode;
                initializeColors(settings.darkMode);
            } else if (currentSelection == 1)
                settings.patrollerDensity =
                    std::max(0.01, settings.patrollerDensity - 0.01);
        } else if (key == KEY_RIGHT || key == 'd' || key == 'D') {
            if (currentSelection == 0) {
                settings.darkMode = !settings.darkMode;
                initializeColors(settings.darkMode);
            } else if (currentSelection == 1)
                settings.patrollerDensity =
                    std::min(0.15, settings.patrollerDensity + 0.01);
        } else if (key == KEY_ENTER || key == 10 || key == 13)
            return;
        else if (key == 27)
            return;
        else if (key == KEY_RESIZE)
            clear();
    }
}

/// @brief Draws and handles the How to Play screen.
void runHowToPlay() {
    clear();
    std::string title = "HOW TO PLAY";
    auto [titleY, titleX] = getCenteredCoords(title.size(), -9);
    attron(A_BOLD);
    mvaddstr(titleY, titleX, title.c_str());
    attroff(A_BOLD);

    auto printBold = [&](const std::string &s, int offset) {
        auto [y, x] = getCenteredCoords(s.size(), offset);
        attron(A_BOLD);
        mvaddstr(y, x, s.c_str());
        attroff(A_BOLD);
    };
    auto printLine = [&](const std::string &s, int offset) {
        auto [y, x] = getCenteredCoords(s.size(), offset);
        mvaddstr(y, x, s.c_str());
    };

    printBold("1. NAVIGATE", -7);
    printLine(
        "Use [W][A][S][D] or arrow keys to move through the streets.", -6);
    printBold("2. EVADE", -4);
    printLine(
        "Avoid patrollers [P]. Magenta tiles show their detection range.", -3);
    printLine("If a patroller reaches an adjacent tile, you are caught.", -2);
    printBold("3. ESCAPE", 0);
    printLine("Reach the exit tile [E] to escape and win.", 1);

    printBold("MAP LEGEND", 3);
    printLine("@  Player          P  Patroller      E  Exit", 4);
    printLine("#  Building        .  Road", 5);
    printLine("~  Water           ^  Forest", 6);

    showControls("Press any key to return.");
    refresh();
    getch();
}

/// @brief Draws and handles the main menu.
///
/// @return Index of the selected option.
int runMainMenu() {
    std::vector<std::string> options = {
        "PLAY", "HOW TO PLAY", "OPTIONS", "QUIT"};
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

        showControls("[W][S] or [UP][DOWN] to select, [ENTER] to confirm");
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

/// @brief Draws and handles the pause menu.
///
/// @param settings The settings object passed to the options menu.
/// @return A PauseResult indicating what to do after closing.
PauseResult runPauseMenu(Settings &settings) {
    std::vector<std::string> options = {
        "RESUME GAME", "EXIT TO MAIN MENU", "EXIT GAME"};
    int currentSelection = 0;

    while (true) {
        clear();

        std::string title = "PAUSED";
        auto [titleY, titleX] = getCenteredCoords(title.size(), -5);
        attron(A_BOLD);
        mvaddstr(titleY, titleX, title.c_str());
        attroff(A_BOLD);

        for (int i = 0; i < (int)options.size(); i++) {
            std::string label =
                (i == currentSelection) ? "> " + options[i] : "  " + options[i];
            auto [y, x] = getCenteredCoords(label.size(), -2 + (i * 2));
            if (i == currentSelection) {
                attron(A_REVERSE);
                mvaddstr(y, x, label.c_str());
                attroff(A_REVERSE);
            } else {
                mvaddstr(y, x, label.c_str());
            }
        }

        showControls("[W][S] or [UP][DOWN] to select, [ENTER] to confirm");
        refresh();

        int key = getch();

        if (key == 27)
            return PauseResult::Resume;
        else if (key == KEY_UP || key == 'w' || key == 'W')
            currentSelection = std::max(0, currentSelection - 1);
        else if (key == KEY_DOWN || key == 's' || key == 'S')
            currentSelection =
                std::min((int)options.size() - 1, currentSelection + 1);
        else if (key == KEY_ENTER || key == 10 || key == 13) {
            if (currentSelection == 0) return PauseResult::Resume;
            if (currentSelection == 1) return PauseResult::ExitToMenu;
            if (currentSelection == 2) return PauseResult::ExitGame;
        } else if (key == KEY_RESIZE)
            clear();
    }
}

/// @brief Renders the game HUD on the bottom of the game screen.
///
/// @param patrollerDensity The current patroller density setting.
/// @param elapsedSeconds Seconds elapsed since the game session started.
void renderHUD(double patrollerDensity, int elapsedSeconds) {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    int minutes = elapsedSeconds / 60;
    int seconds = elapsedSeconds % 60;

    std::string timer = std::to_string(minutes) + ":" +
                        (seconds < 10 ? "0" : "") + std::to_string(seconds);

    std::string hud = "[WASD/Arrows] Move, [ESC] Pause, Patroller Density: " +
                      std::to_string(patrollerDensity).substr(0, 4) +
                      "  |  Time: " + timer;

    std::string display = hud.size() > (size_t)maxX ? hud.substr(0, maxX) : hud;
    mvaddstr(maxY - 1, (maxX - (int)display.size()) / 2, display.c_str());
}

/// @brief Handles a generic game over screen.
///
/// @param title The text to be displayed as the title of the game over screen.
static EndResult runEndScreen(const std::string &title) {
    std::vector<std::string> options = {
        "RESTART", "RETURN TO MAIN MENU", "EXIT GAME"};
    int currentSelection = 0;
    napms(1000);

    while (true) {
        clear();
        auto [titleY, titleX] = getCenteredCoords(title.size(), -5);
        attron(A_BOLD);
        mvaddstr(titleY, titleX, title.c_str());
        attroff(A_BOLD);

        for (int i = 0; i < (int)options.size(); i++) {
            std::string label =
                (i == currentSelection) ? "> " + options[i] : "  " + options[i];
            auto [y, x] = getCenteredCoords(label.size(), -2 + (i * 2));

            if (i == currentSelection) {
                attron(A_REVERSE);
                mvaddstr(y, x, label.c_str());
                attroff(A_REVERSE);
            } else {
                mvaddstr(y, x, label.c_str());
            }
        }

        showControls("[W][S] or [UP][DOWN] to select, [ENTER] to confirm");
        refresh();

        int key = getch();

        if (key == KEY_UP || key == 'w' || key == 'W')
            currentSelection = std::max(0, currentSelection - 1);
        else if (key == KEY_DOWN || key == 's' || key == 'S')
            currentSelection =
                std::min((int)options.size() - 1, currentSelection + 1);
        else if (key == KEY_ENTER || key == 10 || key == 13) {
            if (currentSelection == 0) return EndResult::Restart;
            if (currentSelection == 1) return EndResult::ExitToMenu;
            if (currentSelection == 2) return EndResult::ExitGame;
        } else if (key == KEY_RESIZE)
            clear();
    }
}
EndResult showCaughtScreen() { return runEndScreen("YOU WERE CAUGHT"); }
EndResult showEscapedScreen() { return runEndScreen("YOU ESCAPED"); }

/// @brief Displays a username entry screen and returns the entered name.
///
/// @return The username string entered by the user.
std::string runUsernameEntry() {
    std::string username;

    while (true) {
        clear();

        std::string title = "WELCOME TO TERMINAL PATROLLERS";
        std::string prompt = "Enter a username to get started:";
        std::string input = "> " + username + "_";

        auto [titleY, titleX] = getCenteredCoords(title.size(), -3);
        auto [promptY, promptX] = getCenteredCoords(prompt.size(), -1);
        auto [inputY, inputX] = getCenteredCoords(input.size(), 1);

        attron(A_BOLD);
        mvaddstr(titleY, titleX, title.c_str());
        attroff(A_BOLD);
        mvaddstr(promptY, promptX, prompt.c_str());
        mvaddstr(inputY, inputX, input.c_str());

        showControls("[ENTER] to confirm");
        refresh();

        int key = getch();

        if (key == KEY_ENTER || key == 10 || key == 13) {
            if (!username.empty()) return username;
        } else if (key == KEY_BACKSPACE || key == 127 || key == 8) {
            if (!username.empty()) username.pop_back();
        } else if (key >= 32 && key <= 126 && (int)username.size() < 20) {
            username += (char)key;
        } else if (key == KEY_RESIZE) {
            clear();
        }
    }
}

/// @brief Displays the top scores leaderboard for a location.
///
/// @param locationName The location name shown in the title.
/// @param entries Ranked entries from getTopScores(), best time first.
void showLeaderboard(
    const std::string &locationName, const std::vector<LeaderEntry> &entries) {
    clear();

    std::string title = "TOP 10 - " + locationName;
    auto [titleY, titleX] = getCenteredCoords(title.size(), -6);
    attron(A_BOLD);
    mvaddstr(titleY, titleX, title.c_str());
    attroff(A_BOLD);

    std::string header = "  #   Player               Time  ";
    auto [hY, hX] = getCenteredCoords(header.size(), -4);
    attron(A_UNDERLINE);
    mvaddstr(hY, hX, header.c_str());
    attroff(A_UNDERLINE);

    if (entries.empty()) {
        std::string none = "No scores recorded yet.";
        auto [nY, nX] = getCenteredCoords(none.size(), -2);
        mvaddstr(nY, nX, none.c_str());
    }

    for (int i = 0; i < (int)entries.size(); i++) {
        int mins = entries[i].seconds / 60;
        int secs = entries[i].seconds % 60;
        std::string timeStr = std::to_string(mins) + ":" +
                              (secs < 10 ? "0" : "") + std::to_string(secs);

        // Truncate player name to fit column width
        std::string name = entries[i].player.substr(0, 20);
        // Pad to fixed width for alignment
        name.resize(20, ' ');

        char line[40];
        std::snprintf(line,
            sizeof(line),
            "  %-2d  %s  %s  ",
            i + 1,
            name.c_str(),
            timeStr.c_str());

        auto [y, x] = getCenteredCoords(std::string(line).size(), -2 + i);
        mvaddstr(y, x, line);
    }

    showControls("Press any key to continue.");
    refresh();
    getch();
}

/// @brief Initializes color pairs and sets the window background.
///
/// @param darkMode Boolean that enables or disables dark mode.
void initializeColors(bool darkMode) {
    start_color();

    // If user's terminal can change colors, define white and black
    if (can_change_color()) {
        init_color(COLOR_BLACK, 0, 0, 0);
        init_color(COLOR_WHITE, 1000, 1000, 1000);
    }

    int fg = darkMode ? COLOR_WHITE : COLOR_BLACK;
    int bg = darkMode ? COLOR_BLACK : COLOR_WHITE;

    // Pair 1: Map elements and standard text
    init_pair(1, fg, bg);
    // Pair 2: Player avatar @
    init_pair(2, COLOR_GREEN, bg);
    // Pair 3: Patroller avatar P
    init_pair(3, COLOR_RED, bg);
    // Pair 4: Patroller detection radius
    init_pair(4, COLOR_MAGENTA, bg);
    // Pair 5: Exit tile E
    init_pair(5, COLOR_CYAN, bg);

    bkgd(COLOR_PAIR(1));
}
