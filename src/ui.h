#pragma once

#include "leaderboard_service.h"
#include <string>
#include <utility>
#include <vector>

std::pair<int, int> getCenteredCoords(int textLen, int offsetY = 0);
void showLoadingScreen(const std::string &message);
void showErrorScreen(const std::string &message);
void initializeColors(bool darkMode);
int runMainMenu();
bool runConfirmExit();
void showControls(const std::string &text);
int runLocationMenu(const std::vector<std::string> &names);
struct Settings {
    bool darkMode = false;
    double patrollerDensity = 0.02;
};
void runOptionsMenu(Settings &settings);
void runHowToPlay();
enum class PauseResult { Resume, ExitToMenu, ExitGame };
enum class EndResult { Restart, ExitToMenu, ExitGame };
PauseResult runPauseMenu(Settings &settings);
EndResult showCaughtScreen();
EndResult showEscapedScreen();
void renderHUD(double patrollerDensity, int elapsedSeconds);
std::string runUsernameEntry();
void showLeaderboard(
    const std::string &locationName, const std::vector<LeaderEntry> &entries);