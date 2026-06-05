#include "map_service.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

static const std::string kMicrogeoUrl = "http://localhost:8000";
static const std::string kMicromapUrl = "http://localhost:8001";

static const std::vector<Location> kLocations = {
    {"Los Angeles, CA", "Los Angeles, California, USA"},
    {"Portland, OR", "Portland, Oregon, USA"},
};

/// @brief Returns the hardcoded list of selectable game locations.
///
/// @return A const reference to the list of available Location entries.
const std::vector<Location> &getLocations() { return kLocations; }

/// @brief Maps a micromap terrain string to a game tile character.
///
/// @param terrain The terrain type string returned by micromap.
/// @return A single character representing the terrain tile.
static char terrainToChar(const std::string &terrain) {
    if (terrain == "building") return '#';
    if (terrain == "road") return '.';
    if (terrain == "water") return '~';
    if (terrain == "grass") return ',';
    if (terrain == "forest") return '^';
    return '.';
}

/// @brief Fetches a real-world map via microgeo and micromap.
///
/// @param location The location to fetch.
/// @return A 2D tile grid as a vector of strings, ready for renderMap().
/// @throws std::runtime_error if any HTTP call fails.
std::vector<std::string> fetchMap(const Location &location) {
    // 1. MICROGEO - Fetch map data
    auto searchRes =
        cpr::Get(cpr::Url{kMicrogeoUrl + "/v1/search"},
                 cpr::Parameters{{"q", location.query}, {"limit", "1"}});
    if (searchRes.status_code != 200)
        throw std::runtime_error("microgeo search failed: " + searchRes.text);

    auto searchJson = nlohmann::json::parse(searchRes.text);

    // Coordinates are stored as [longitude, latitude]
    double lat = searchJson["features"][0]["geometry"]["coordinates"][1];
    double lon = searchJson["features"][0]["geometry"]["coordinates"][0];

    auto featRes = cpr::Get(cpr::Url{kMicrogeoUrl + "/v1/features/point"},
                            cpr::Parameters{
                                {"lat", std::to_string(lat)},
                                {"lon", std::to_string(lon)},
                                {"radius", "1000"} // in meters
                            });
    if (featRes.status_code != 200)
        throw std::runtime_error("microgeo features failed: " + featRes.text);

    auto geoJson = nlohmann::json::parse(featRes.text);

    // 2. MICROMAP - Convert map data to tileable features
    nlohmann::json payload;
    payload["osm"] = geoJson;
    payload["tile_size_m"] = 10;
    payload["default_terrain"] = "grass";

    auto convertRes = cpr::Post(
        cpr::Url{kMicromapUrl + "/v1/convert"},
        cpr::Body{payload.dump()}, // Serializes JSON to a string for POST body
        cpr::Header{{"Content-Type", "application/json"}});
    if (convertRes.status_code != 200)
        throw std::runtime_error("micromap convert failed: " + convertRes.text);

    auto mapJson = nlohmann::json::parse(convertRes.text);
    std::vector<std::string> result;
    const auto &grid = mapJson["grid"];

    // grid[0] is south edge; reverse for north-at-top rendering
    for (int row = (int)grid.size() - 1; row >= 0; row--) {
        std::string rowStr;
        for (const auto &cell : grid[row])
            rowStr += terrainToChar(cell.get<std::string>());
        result.push_back(rowStr);
    }

    return result;
}