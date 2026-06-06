#include "game.h"
#include "map_service.h"
#include "ui.h"
#include <functional>
#include <mutex>
#include <ncurses.h>
#include <stdexcept>
#include <thread>

/// @brief App entry point.
///
/// @return Exit status code.
int main() {
    set_escdelay(25); // Make ESC more responsive
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    Settings settings;
    initializeColors(settings.darkMode);

    bool exitGame = false;
    while (!exitGame) {
        int selection = runMainMenu();

        // 0 = PLAY
        // 1 = HOW TO PLAY
        // 2 = OPTIONS
        // 3 = QUIT
        if (selection == 0) {
            // Location selection
            const auto &locations = getLocations();
            std::vector<std::string> names;
            for (const auto &loc : locations)
                names.push_back(loc.name);

            int locSelection = runLocationMenu(names);
            if (locSelection == -1) continue;
            showLoadingScreen("Loading map...");

            std::vector<std::string> map;
            try {
                map = fetchMap(locations[locSelection]);
            } catch (const std::exception &e) {
                showErrorScreen(std::string("Failed to load map: ") + e.what());
                continue;
            }

            bool restart = true;
            while (restart && !exitGame) {
                restart = false;
                // Generate entities on road tiles
                Entities entities;
                try {
                    entities = generateEntities(map, settings.patrollerDensity);
                } catch (const std::exception &e) {
                    showErrorScreen(e.what());
                    break;
                }

                Player player = entities.player;
                std::vector<Patroller> patrollers = entities.patrollers;
                auto exitPos = entities.exit;
                GameResult result = GameResult::Playing;

                int maxY, maxX;

                std::mutex patrollerMtx;
                std::atomic<bool> patrollerRunning{true};

                std::thread patrollerThread(runPatrollers,
                    std::ref(patrollers),
                    std::ref(patrollerMtx),
                    std::ref(map),
                    std::ref(patrollerRunning),
                    std::ref(player));

                // getch() times out every 100ms
                // Otherwise getch() blocks the main thread until a key is
                // pressed And so patroller movements wouldn't render until a
                // key is pressed
                timeout(100);

                const int dy[] = {-1, 1, 0, 0};
                const int dx[] = {0, 0, -1, 1};

                // Main game loop
                while (result == GameResult::Playing) {
                    getmaxyx(stdscr, maxY, maxX);

                    // Static camera with map centered
                    int originY = (maxY - (int)map.size()) / 2;
                    int originX = (maxX - (int)map[0].size()) / 2;

                    erase();

                    renderMap(map, originY, originX);

                    {
                        // Lock patroller thread to write patroller movement
                        std::lock_guard<std::mutex> lock(patrollerMtx);
                        for (const auto &p : patrollers) {
                            for (int d = 0; d < 4; d++) {
                                int ny = p.y + dy[d];
                                int nx = p.x + dx[d];
                                if (ny >= 0 && ny < (int)map.size() &&
                                    nx >= 0 && nx < (int)map[0].size() &&
                                    map[ny][nx] == '.') {
                                    // Highlight patroller adjacent danger
                                    // squares
                                    attron(COLOR_PAIR(4) | A_REVERSE);
                                    mvaddch(originY + ny,
                                        originX + nx,
                                        map[ny][nx]);
                                    attroff(COLOR_PAIR(4) | A_REVERSE);
                                }
                            }

                            // LOSE CONDITION
                            if (checkDetection(patrollers, player))
                                result = GameResult::Caught;

                            attron(COLOR_PAIR(3) | A_REVERSE);
                            mvaddch(originY + p.y, originX + p.x, 'P');
                            attroff(COLOR_PAIR(3) | A_REVERSE);
                        }
                    }

                    // Render player avatar
                    attron(COLOR_PAIR(2) | A_BOLD | A_REVERSE);
                    mvaddch(originY + player.y, originX + player.x, '@');
                    attroff(COLOR_PAIR(2) | A_BOLD | A_REVERSE);

                    // Render exit tile
                    attron(COLOR_PAIR(5) | A_BOLD | A_REVERSE);
                    mvaddch(
                        originY + exitPos.first, originX + exitPos.second, 'E');
                    attroff(COLOR_PAIR(5) | A_BOLD | A_REVERSE);

                    refresh();

                    // Win check
                    if (player.y == exitPos.first && player.x == exitPos.second)
                        result = GameResult::Escaped;

                    int key = getch();

                    // Pause menu
                    if (key == 27) {
                        timeout(-1);
                        PauseResult pause = runPauseMenu(settings);
                        timeout(100);
                        if (pause == PauseResult::ExitToMenu) break;
                        if (pause == PauseResult::ExitGame) {
                            exitGame = true;
                            break;
                        }
                        continue;
                    }
                    if (key == KEY_RESIZE || key == ERR) continue;

                    // New avatar position after step
                    auto [newY, newX] =
                        calculateNewPos(key, player.y, player.x);
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

                timeout(-1);
                patrollerRunning = false;
                patrollerThread.join();

                EndResult endResult = EndResult::ExitToMenu;
                if (!exitGame) {
                    if (result == GameResult::Caught)
                        endResult = showCaughtScreen();
                    else if (result == GameResult::Escaped)
                        endResult = showEscapedScreen();
                }
                if (endResult == EndResult::Restart)
                    restart = true;
                else if (endResult == EndResult::ExitGame)
                    exitGame = true;
            }

        } else if (selection == 1) {
            runHowToPlay();
        } else if (selection == 2) {
            runOptionsMenu(settings);
        } else if (selection == 3) {
            if (runConfirmExit()) break;
        }

        if (exitGame) break;
    }

    endwin();
    return 0;
}
