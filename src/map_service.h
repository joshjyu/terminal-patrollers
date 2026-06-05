#pragma once

#include <string>
#include <vector>

struct Location {
    std::string name;
    std::string query;
};

const std::vector<Location> &getLocations();

std::vector<std::string> fetchMap(const Location &location);