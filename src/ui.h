#pragma once

#include <string>
#include <utility>
#include <vector>

std::pair<int, int> getCenteredCoords(int textLen, int offsetY = 0);
void showLoadingScreen(const std::string &message);
void showErrorScreen(const std::string &message);
void initializeColors();
int runMainMenu();
bool runConfirmExit();
void showControls(const std::string &text);
int runLocationMenu(const std::vector<std::string> &names);
void showCaughtScreen();
void showEscapedScreen();