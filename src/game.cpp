#include "game.h"

#include <ncurses.h>
#include <string>

// Temporary test map
static const std::vector<std::string> kTestMap = {
    "################################################################",
    "#..............................................................#",
    "#...##########..............##########################.........#",
    "#...##########..............##########################.........#",
    "#...##########..............##########################...####..#",
    "#...##########..............##########################...####..#",
    "#........................................................####..#",
    "#........................................................####..#",
    "#.......###########............................................#",
    "#.......###########.........####.........###################...#",
    "#.......###########.........####.........###################...#",
    "#.......###########.........####.........###################...#",
    "#...........................####.........###################...#",
    "#..............................................................#",
    "#..............................................................#",
    "#...###################........................................#",
    "#...###################........#####################.....#######",
    "#...###################........#####################.....#######",
    "#...###################........#####################.....#######",
    "#...###################........#####################.....#######",
    "#..............................#####################...........#",
    "#..............................................................#",
    "#..............................................................#",
    "#........############...................................####...#",
    "#........############..........#################........####...#",
    "#........############..........#################........####...#",
    "#........############..........#################........####...#",
    "#..............................#################...............#",
    "#..............................#################...............#",
    "#..............................................................#",
    "#..............................................................#",
    "################################################################"};

// Returns kTestMap
const std::vector<std::string> &getTestMap() { return kTestMap; }

/// @brief Renders the map grid centered on the terminal.
///
/// @param mapGrid The 2D map to render.
/// @param originY The Y coord int at which the map begins rendering.
/// @param originX The X coord int at which the map begins rendering.
void renderMap(const std::vector<std::string> &mapGrid, int originY,
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
bool isValidMove(const std::vector<std::string> &mapGrid, int targetY,
                 int targetX) {
    int maxY = mapGrid.size();
    int maxX = maxY > 0 ? (int)mapGrid[0].size() : 0;

    bool inBounds =
        targetY >= 0 && targetY < maxY && targetX >= 0 && targetX < maxX;

    // Valid if inBounds and target movement space is not an obstacle
    return inBounds && mapGrid[targetY][targetX] != '#';
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
