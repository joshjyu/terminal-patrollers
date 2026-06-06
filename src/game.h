#pragma once

#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <utility>
#include <vector>

bool isValidMove(
    const std::vector<std::string> &mapGrid, int targetY, int targetX);

std::pair<int, int> calculateNewPos(int key, int currentY, int currentX);

void renderMap(
    const std::vector<std::string> &mapGrid, int originY, int originX);

std::vector<std::pair<int, int>> getRoadTiles(
    const std::vector<std::string> &mapGrid);

std::vector<std::pair<int, int>> getLargestRoadComponent(
    const std::vector<std::string> &mapGrid);

struct Player {
    int y;
    int x;
};

struct Patroller {
    int y;
    int x;
};

struct Entities {
    Player player;
    std::vector<Patroller> patrollers;
    std::pair<int, int> exit;
};

Entities generateEntities(
    const std::vector<std::string> &mapGrid, double density);

void runPatrollers(std::vector<Patroller> &patrollers,
    std::mutex &mtx,
    const std::vector<std::string> &mapGrid,
    std::atomic<bool> &running,
    const Player &player);