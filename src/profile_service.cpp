#include "profile_service.h"

#include <cpr/cpr.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

static const std::string kProfileUrl = "http://localhost:8003";
static const std::string kAppId = "terminal_patrollers";
static const std::string kUserIdFile = "user_id.txt";
static const std::string kPassword = "local";

/// @brief Reads the stored user ID from disk.
///
/// @return The user ID string, or empty if no file exists.
std::string loadUserId() {
    std::ifstream file(kUserIdFile);
    if (!file.is_open()) return "";
    std::string userId;
    std::getline(file, userId);
    return userId;
}

/// @brief Writes the user ID to disk.
///
/// @param userId The user ID to persist.
void saveUserId(const std::string &userId) {
    std::ofstream file(kUserIdFile);
    file << userId;
}

/// @brief Creates a new user profile on microprofile.
///
/// @param username The desired username.
/// @return The new user ID string.
/// @throws std::runtime_error on failure or duplicate username.
std::string createProfile(const std::string &username) {
    nlohmann::json payload;
    payload["username"] = username;
    payload["password"] = kPassword;

    auto res = cpr::Post(cpr::Url{kProfileUrl + "/profiles"},
        cpr::Body{payload.dump()},
        cpr::Header{{"Content-Type", "application/json"}});

    if (res.status_code == 409)
        throw std::runtime_error("Username already taken.");
    if (res.status_code != 201)
        throw std::runtime_error("Failed to create profile: " + res.text);

    return nlohmann::json::parse(res.text)["id"];
}

/// @brief Loads saved app data for a user from microprofile.
///
/// @param userId The user's ID.
/// @param username The user's username.
/// @return A ProfileData struct with stored or default values.
/// @throws std::runtime_error if the request fails unexpectedly.
ProfileData loadProfileData(
    const std::string &userId, const std::string &username) {
    ProfileData data;
    data.userId = userId;
    data.username = username;

    auto res = cpr::Get(
        cpr::Url{kProfileUrl + "/profiles/" + userId + "/apps/" + kAppId});

    if (res.status_code == 404) return data;
    if (res.status_code != 200)
        throw std::runtime_error("Failed to load profile data: " + res.text);

    auto json = nlohmann::json::parse(res.text);
    if (json.contains("darkMode")) data.darkMode = json["darkMode"];
    if (json.contains("patrollerDensity"))
        data.patrollerDensity = json["patrollerDensity"];
    if (json.contains("bestTimes"))
        for (auto &[key, val] : json["bestTimes"].items())
            data.bestTimes[key] = val.get<int>();

    return data;
}

/// @brief Saves app data for a user to microprofile.
///
/// @param data The ProfileData to persist.
/// @throws std::runtime_error if the request fails.
void saveProfileData(const ProfileData &data) {
    nlohmann::json payload;
    payload["darkMode"] = data.darkMode;
    payload["patrollerDensity"] = data.patrollerDensity;
    payload["bestTimes"] = data.bestTimes;

    auto res = cpr::Put(
        cpr::Url{kProfileUrl + "/profiles/" + data.userId + "/apps/" + kAppId},
        cpr::Body{payload.dump()},
        cpr::Header{{"Content-Type", "application/json"}});

    if (res.status_code != 200)
        throw std::runtime_error("Failed to save profile data: " + res.text);
}