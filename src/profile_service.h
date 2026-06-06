#pragma once

#include <map>
#include <string>

struct ProfileData {
    std::string userId;
    std::string username;
    bool darkMode = false;
    double patrollerDensity = 0.02;
    std::map<std::string, int> bestTimes;
};

std::string loadUserId();
void saveUserId(const std::string &userId);
std::string createProfile(const std::string &username);
ProfileData loadProfileData(
    const std::string &userId, const std::string &username);
void saveProfileData(const ProfileData &data);