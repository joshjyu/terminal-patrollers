#pragma once

#include <utility>
#include <vector>

bool isValidMove(const std::vector<std::vector<char>> &mapGrid, int targetY,
                 int targetX);

std::pair<int, int> calculateNewPos(int key, int currentY, int currentX);
