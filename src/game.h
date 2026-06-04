#pragma once

#include <string>
#include <utility>
#include <vector>

bool isValidMove(const std::vector<std::string> &mapGrid, int targetY,
                 int targetX);

std::pair<int, int> calculateNewPos(int key, int currentY, int currentX);

const std::vector<std::string> &getTestMap();
void renderMap(const std::vector<std::string> &mapGrid, int originY,
               int originX);

struct Player {
    int y;
    int x;
};