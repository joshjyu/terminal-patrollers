#include "game.h"
#include "map_service.h"
#include "ui.h"
#include <functional>
#include <mutex>
#include <ncurses.h>
#include <thread>

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
            const auto &locations = getLocations();
            std::vector<std::string> names;
            for (const auto &loc : locations)
                names.push_back(loc.name);

            int locSelection = runLocationMenu(names);
            showLoadingScreen("Loading map...");

            std::vector<std::string> map;
            try {
                map = fetchMap(locations[locSelection]);
            } catch (const std::exception &e) {
                showErrorScreen(std::string("Failed to load map: ") + e.what());
                continue;
            }

            int maxY, maxX;

            // Generate patrollers
            std::vector<Patroller> patrollers = {{5, 5}, {15, 15}, {25, 3}};
            std::mutex patrollerMtx;
            std::atomic<bool> patrollerRunning{true};

            Player player = {2, 2}; // Player's avatar origin

            std::thread patrollerThread(runPatrollers,
                                        std::ref(patrollers),
                                        std::ref(patrollerMtx),
                                        std::ref(map),
                                        std::ref(patrollerRunning),
                                        std::ref(player));

            // getch() times out every 100ms
            // Otherwise getch() blocks the main thread until a key is pressed
            // And so patroller movements wouldn't render until a key is pressed
            timeout(100);

            // Main game loop
            while (true) {
                getmaxyx(stdscr, maxY, maxX);
                int originY = (maxY - (int)map.size()) / 2;
                int originX = (maxX - (int)map[0].size()) / 2;

                erase();

                renderMap(map, originY, originX);

                // Lock patroller thread to write patroller movement
                {
                    std::lock_guard<std::mutex> lock(patrollerMtx);
                    for (const auto &p : patrollers) {
                        attron(COLOR_PAIR(3));
                        mvaddch(originY + p.y, originX + p.x, 'P');
                        attroff(COLOR_PAIR(3));
                    }
                }

                // Render avatar
                attron(COLOR_PAIR(2));
                mvaddch(originY + player.y, originX + player.x, '@');
                attroff(COLOR_PAIR(2));

                refresh();

                int key = getch();
                if (key == 27) break; // ESC returns to main menu
                if (key == KEY_RESIZE || key == ERR) continue;

                // New avatar position after step
                auto [newY, newX] = calculateNewPos(key, player.y, player.x);
                if (isValidMove(map, newY, newX)) {
                    // Lock thread to check collision with patroller
                    std::lock_guard<std::mutex> lock(patrollerMtx);
                    bool blocked = false;
                    for (const auto &p : patrollers) {
                        if (p.y == newY && p.x == newX) blocked = true;
                    }
                    if (!blocked) player = {newY, newX};
                }
            }

            timeout(-1); // Restore blocking after hitting ESC
            patrollerRunning = false;
            patrollerThread.join();

        } else if (selection == 1) {
            if (runConfirmExit()) break;
        }
    }

    endwin();
    return 0;
}
