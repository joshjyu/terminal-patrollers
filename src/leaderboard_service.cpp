#include "leaderboard_service.h"

#include <cctype>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

static const std::string kLeaderUrl = "http://localhost:8002";

/// @brief Converts a location name to a stable client_app_id slug.
///
/// @param locationName Human-readable location name.
/// @return Lowercase alphanumeric slug with underscores.
static std::string locationToAppId(const std::string &locationName) {
    std::string slug = "terminal_patrollers_";
    for (char c : locationName)
        slug += std::isalnum((unsigned char)c)
                    ? (char)std::tolower((unsigned char)c)
                    : '_';
    return slug;
}

/// @brief Submits a completed run's time to the leaderboard.
///
/// @param locationName The location the run was played on.
/// @param player The player's display name.
/// @param seconds Elapsed seconds for the run.
/// @throws std::runtime_error if the request fails.
void submitScore(
    const std::string &locationName, const std::string &player, int seconds) {
    nlohmann::json payload;
    payload["client_app_id"] = locationToAppId(locationName);
    payload["player"] = player;
    payload["score"] = -seconds;

    auto res = cpr::Post(cpr::Url{kLeaderUrl + "/leaderboard/scores"},
        cpr::Body{payload.dump()},
        cpr::Header{{"Content-Type", "application/json"}});

    if (res.status_code != 200 && res.status_code != 201)
        throw std::runtime_error("microleader submit failed: " + res.text);
}

/// @brief Fetches the top scores for a location from the leaderboard.
///
/// @param locationName The location to query.
/// @param limit Max number of entries to return.
/// @return A vector of LeaderEntry sorted best-time first.
/// @throws std::runtime_error if the request fails.
std::vector<LeaderEntry> getTopScores(
    const std::string &locationName, int limit) {
    auto res = cpr::Get(cpr::Url{kLeaderUrl + "/leaderboard/" +
                                 locationToAppId(locationName) + "/top"},
        cpr::Parameters{{"limit", std::to_string(limit)}});

    if (res.status_code != 200)
        throw std::runtime_error("microleader fetch failed: " + res.text);

    std::vector<LeaderEntry> entries;
    for (const auto &item : nlohmann::json::parse(res.text))
        entries.push_back({item["player"], -item["score"].get<int>()});

    return entries;
}