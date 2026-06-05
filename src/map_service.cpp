#include "map_service.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

static const std::vector<Location> kLocations = {
    {"Los Angeles", "Los Angeles, California, USA"},
    {"Portland", "Portland, Oregon, USA"},
};

const std::vector<Location> &getLocations() { return kLocations; }

static char terrainToChar(const std::string &terrain) {
    if (terrain == "building") return '#';
    if (terrain == "road") return '.';
    if (terrain == "water") return '~';
    if (terrain == "grass") return ',';
    if (terrain == "forest") return '^';
    return '.';
}