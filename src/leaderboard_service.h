#pragma once

#include <string>
#include <vector>

struct LeaderEntry {
    std::string player;
    int seconds;
};

void submitScore(
    const std::string &locationName, const std::string &player, int seconds);

std::vector<LeaderEntry> getTopScores(
    const std::string &locationName, int limit = 10);