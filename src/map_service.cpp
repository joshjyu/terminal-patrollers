#include "map_service.h"

#include <cmath>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

static const std::string kMicrogeoUrl = "http://localhost:8000";
static const std::string kMicromapUrl = "http://localhost:8001";
static const int kRadius = 250;
static const int kTileSizeM = 15;
static const std::string kDefaultTerrain = "building";

static const std::vector<Location> kLocations = {
    {"MacArthur Park, Los Angeles", "34.059556, -118.274917"}};

/// @brief Returns the hardcoded list of selectable game locations.
///
/// @return A const reference to the list of available Location entries.
const std::vector<Location> &getLocations() { return kLocations; }

/// @brief Maps a micromap terrain string to a game tile character.
///
/// @param terrain The terrain type string returned by micromap.
/// @return A single character representing the terrain tile.
static char terrainToChar(const std::string &terrain) {
    if (terrain == "wall") return 'X';
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
        throw std::runtime_error("microgeo search failed..." + searchRes.text);

    auto searchJson = nlohmann::json::parse(searchRes.text);

    // Coordinates are stored as [longitude, latitude]
    double lat = searchJson["features"][0]["geometry"]["coordinates"][1];
    double lon = searchJson["features"][0]["geometry"]["coordinates"][0];

    // Calculate strict bounding box to prevent grid expansion
    const double PI = 3.14159265358979323846;
    // 1 degree lat is about 111,320 meters
    double latOffset = kRadius / 111320.0;
    // Lon shrinks based on lat, adjust using cos()
    double lonOffset = kRadius / (111320.0 * std::cos(lat * PI / 180.0));

    double minLat = lat - latOffset;
    double maxLat = lat + latOffset;
    double minLon = lon - lonOffset;
    double maxLon = lon + lonOffset;

    auto featRes = cpr::Get(cpr::Url{kMicrogeoUrl + "/v1/features/point"},
                            cpr::Parameters{
                                {"lat", std::to_string(lat)},
                                {"lon", std::to_string(lon)},
                                {"radius", std::to_string(kRadius)} // in meters
                            });
    if (featRes.status_code != 200)
        throw std::runtime_error("microgeo features failed..." + featRes.text);

    auto geoJson = nlohmann::json::parse(featRes.text);
    geoJson["metadata"] = {{"bbox", {minLon, minLat, maxLon, maxLat}}};

    // 2. MICROMAP - Convert map data to tileable features
    nlohmann::json payload;
    payload["osm"] = geoJson;
    payload["tile_size_m"] = kTileSizeM;
    payload["default_terrain"] = kDefaultTerrain;

    // Custom hierarchy for detailed features
    payload["mapping"] = nlohmann::json::array(
        {// Unpassable hazards
         {{"terrain", "water"},
          {"priority", 90},
          {"match",
           {{"natural", {"water"}},
            {"waterway", {"canal", "river"}},
            {"amenity", {"fountain"}}}}},
         {{"terrain", "wall"},
          {"priority", 85},
          {"match",
           {{"barrier", {"wall", "fence", "hedge", "retaining_wall"}}}}},

         // Carve out walkable paths explicitly (Priority 80 overrides default
         // buildings)
         {{"terrain", "road"},
          {"priority", 80},
          {"match",
           {{"highway",
             {"primary",
              "secondary",
              "tertiary",
              "residential",
              "footway",
              "path",
              "pedestrian",
              "service",
              "living_street",
              "cycleway",
              "steps",
              "track"}}}}},

         // Carve out open hiding/navigable areas
         {{"terrain", "grass"},
          {"priority", 75},
          {"match",
           {{"leisure", {"park", "garden", "pitch", "playground"}},
            {"landuse", {"grass", "meadow", "recreation_ground"}},
            {"amenity", {"parking"}}}}},

         // Mapped buildings (Reinforces the default solid mass)
         {{"terrain", "building"},
          {"priority", 70},
          {"match",
           {{"building",
             {"yes",
              "commercial",
              "retail",
              "apartments",
              "house",
              "university",
              "office",
              "hotel",
              "civic",
              "industrial",
              "residential",
              "public"}}}}}});

    auto convertRes = cpr::Post(
        cpr::Url{kMicromapUrl + "/v1/convert"},
        cpr::Body{payload.dump()}, // Serializes JSON to a string for POST body
        cpr::Header{{"Content-Type", "application/json"}});
    if (convertRes.status_code != 200)
        throw std::runtime_error("micromap convert failed..." +
                                 convertRes.text);

    auto mapJson = nlohmann::json::parse(convertRes.text);
    std::vector<std::string> result;
    const auto &grid = mapJson["grid"];

    // grid[0] is south edge; reverse for north-at-top rendering
    for (int row = (int)grid.size() - 1; row >= 0; row--) {
        std::string rowStr;
        for (const auto &cell : grid[row]) {
            char tile = terrainToChar(cell.get<std::string>());
            // Fix terminal aspect ratio - multiple horizontal spaces per cell
            rowStr += tile;
            rowStr += tile;
        }

        result.push_back(rowStr);
    }

    return result;
}