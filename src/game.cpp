#include "game.h"

#include <algorithm>
#include <chrono>
#include <ncurses.h>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>

/// @brief Renders the map grid centered on the terminal.
///
/// @param mapGrid The 2D map to render.
/// @param originY The Y coord int at which the map begins rendering.
/// @param originX The X coord int at which the map begins rendering.
void renderMap(
    const std::vector<std::string> &mapGrid, int originY, int originX) {
    // Renders the map
    for (int row = 0; row < (int)mapGrid.size(); row++) {
        for (int col = 0; col < (int)mapGrid[0].size(); col++) {
            mvaddch(originY + row, originX + col, mapGrid[row][col]);
        }
    }
}

/// @brief Returns all road tile positions in the map.
///
/// @param mapGrid The 2D map to scan.
/// @return A vector of (row, col) pairs for every road tile.
std::vector<std::pair<int, int>> getRoadTiles(
    const std::vector<std::string> &mapGrid) {
    std::vector<std::pair<int, int>> roads;
    for (int row = 0; row < (int)mapGrid.size(); row++) {
        for (int col = 0; col < (int)mapGrid[0].size(); col++) {
            if (mapGrid[row][col] == '.') roads.push_back({row, col});
        }
    }
    return roads;
}

/// @brief Finds the largest connected group of road tiles using BFS.
///
/// @param mapGrid The 2D map to scan.
/// @return Road tile positions belonging to the largest connected component.
std::vector<std::pair<int, int>> getLargestRoadComponent(
    const std::vector<std::string> &mapGrid) {
    int rows = mapGrid.size();
    int cols = rows > 0 ? (int)mapGrid[0].size() : 0;

    std::vector<std::vector<bool>> visited(
        rows, std::vector<bool>(cols, false));

    const int dy[] = {-1, 1, 0, 0};
    const int dx[] = {0, 0, -1, 1};

    std::vector<std::pair<int, int>> largest;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (mapGrid[row][col] != '.' || visited[row][col]) continue;

            std::vector<std::pair<int, int>> component;
            std::queue<std::pair<int, int>> q;
            q.push({row, col});
            visited[row][col] = true;

            while (!q.empty()) {
                auto [r, c] = q.front();
                q.pop();
                component.push_back({r, c});

                for (int d = 0; d < 4; d++) {
                    int nr = r + dy[d];
                    int nc = c + dx[d];
                    if (nr >= 0 && nr < rows && nc >= 0 && nc < cols &&
                        mapGrid[nr][nc] == '.' && !visited[nr][nc]) {
                        visited[nr][nc] = true;
                        q.push({nr, nc});
                    }
                }
            }

            if (component.size() > largest.size())
                largest = std::move(component);
        }
    }

    return largest;
}

/// @brief Generates player and patroller starting positions on road tiles.
///
/// @param mapGrid The 2D map to scan for road tiles.
/// @param density Patrollers per road tile (e.g. 0.02 = 2 per 100 tiles).
/// @return Entities struct containing the player and patroller positions.
/// @throws std::runtime_error if there are not enough road tiles.
Entities generateEntities(
    const std::vector<std::string> &mapGrid, double density) {
    // getLargestRoadComponent to determine roads that are connected.
    // Prevents considering roads that are isolated from the rest of the map.
    auto roads = getLargestRoadComponent(mapGrid);

    int numPatrollers = std::max(1, (int)(roads.size() * density));
    if ((int)roads.size() < numPatrollers + 1)
        throw std::runtime_error("Not enough road tiles to place entities.");

    std::mt19937 rng(std::random_device{}());
    std::shuffle(roads.begin(), roads.end(), rng);

    Entities entities;
    entities.player = {roads[0].first, roads[0].second};
    for (int i = 1; i <= numPatrollers; i++)
        entities.patrollers.push_back({roads[i].first, roads[i].second});

    // Exit square on road tile furthest from the player by Manhattan distance
    int farthestDist = -1;
    for (int i = numPatrollers + 1; i < (int)roads.size(); i++) {
        int dist = abs(roads[i].first - entities.player.y) +
                   abs(roads[i].second - entities.player.x);
        if (dist > farthestDist) {
            farthestDist = dist;
            entities.exit = roads[i];
        }
    }

    return entities;
}

/// @brief Checks if the target coordinates are a valid movement space.
///
/// @param mapGrid The 2d map matrix
/// @param targetY Target row index
/// @param targetX Target column index
/// @return True if the move is valid. False otherwise.
bool isValidMove(
    const std::vector<std::string> &mapGrid, int targetY, int targetX) {
    int maxY = mapGrid.size();
    int maxX = maxY > 0 ? (int)mapGrid[0].size() : 0;

    bool inBounds =
        targetY >= 0 && targetY < maxY && targetX >= 0 && targetX < maxX;

    // Valid if inBounds and target movement space is not an obstacle
    return inBounds && mapGrid[targetY][targetX] == '.';
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
/// @param player The player avatar to prevent player and patroller collision.
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

/// @brief Checks if any patroller is adjacent to the player.
///
/// @param patrollers The list of active patrollers.
/// @param player The current player position.
/// @return True if any patroller is within Manhattan distance of 1.
bool checkDetection(
    const std::vector<Patroller> &patrollers, const Player &player) {
    for (const auto &p : patrollers)
        if (abs(p.y - player.y) + abs(p.x - player.x) == 1) return true;

    return false;
}