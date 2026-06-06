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