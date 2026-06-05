#include "game.h"

#include <chrono>
#include <ncurses.h>
#include <random>
#include <string>
#include <thread>

// Temporary test map
static const std::vector<std::string> kTestMap = {
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................",
    "................................................................"};

// Returns kTestMap
const std::vector<std::string> &getTestMap() { return kTestMap; }

/// @brief Renders the map grid centered on the terminal.
///
/// @param mapGrid The 2D map to render.
/// @param originY The Y coord int at which the map begins rendering.
/// @param originX The X coord int at which the map begins rendering.
void renderMap(const std::vector<std::string> &mapGrid,
               int originY,
               int originX) {
    // Renders the map
    for (int row = 0; row < (int)mapGrid.size(); row++) {
        for (int col = 0; col < (int)mapGrid[0].size(); col++) {
            mvaddch(originY + row, originX + col, mapGrid[row][col]);
        }
    }
}

/// @brief Checks if the target coordinates are a valid movement space.
///
/// @param mapGrid The 2d map matrix
/// @param targetY Target row index
/// @param targetX Target column index
/// @return True if the move is valid. False otherwise.
bool isValidMove(const std::vector<std::string> &mapGrid,
                 int targetY,
                 int targetX) {
    int maxY = mapGrid.size();
    int maxX = maxY > 0 ? (int)mapGrid[0].size() : 0;

    bool inBounds =
        targetY >= 0 && targetY < maxY && targetX >= 0 && targetX < maxX;

    // Valid if inBounds and target movement space is not an obstacle
    return inBounds && mapGrid[targetY][targetX] != '#' &&
           mapGrid[targetY][targetX] != 'X';
}

/// @brief Calculates new coordinates based on the movement key pressed.
///
/// @param key The key code from getch().
/// @param currentY Current row coordinate.
/// @param currentX Current column coordinate.
/// @return A pair of updated (y, x) coordinates.
std::pair<int, int> calculateNewPos(int key, int currentY, int currentX) {
    int newY = currentY;
    int newX = currentX;

    // Handles WASD and Arrow Keys inputs
    if (key == KEY_UP || key == 'w' || key == 'W')
        newY--;
    else if (key == KEY_DOWN || key == 's' || key == 'S')
        newY++;
    else if (key == KEY_LEFT || key == 'a' || key == 'A')
        newX--;
    else if (key == KEY_RIGHT || key == 'd' || key == 'D')
        newX++;

    return {newY, newX};
}

/// @brief Moves all the patrollers autonomously on a background thread.
///
/// @param patrollers Shared list of patroller positions.
/// @param mtx Mutex guarding patrollers.
/// @param mapGrid The map used for collision checking.
/// @param running The loop exits when the atomic flag is set to false.
void runPatrollers(std::vector<Patroller> &patrollers,
                   std::mutex &mtx,
                   const std::vector<std::string> &mapGrid,
                   std::atomic<bool> &running,
                   const Player &player) {

    // Generate random int [0,3]
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dir(0, 3);

    // Directions Up, Down, Left, Right
    const int dy[] = {-1, 1, 0, 0};
    const int dx[] = {0, 0, -1, 1};

    while (running) {
        // Sleep before lock to prevent holding a mutex while sleeping
        std::this_thread::sleep_for(std::chrono::milliseconds(600));

        std::lock_guard<std::mutex> lock(mtx);
        for (auto &p : patrollers) {
            int d = dir(rng);
            int newY = p.y + dy[d];
            int newX = p.x + dx[d];

            if (!isValidMove(mapGrid, newY, newX)) continue;
            if (newY == player.y && newX == player.x) continue;

            bool occupied = false;
            for (const auto &other : patrollers) {
                if (&other != &p && other.y == newY && other.x == newX)
                    occupied = true;
            }

            if (!occupied) {
                p.y = newY;
                p.x = newX;
            }
        }
    }
}