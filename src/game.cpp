#include "game.h"

#include <ncurses.h>

/// @brief Checks if the target coordinates are a valid movement space.
///
/// @param mapGrid The 2d map matrix
/// @param targetY Target row index
/// @param targetX Target column index
/// @return True if the move is valid. False otherwise.
bool isValidMove(const std::vector<std::vector<char>> &mapGrid, int targetY,
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
/// @return A pair of updated (x, y) coordinates.
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
