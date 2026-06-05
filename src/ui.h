#pragma once

#include <string>
#include <utility>
#include <vector>

std::pair<int, int> getCenteredCoords(int textLen, int offsetY = 0);
void initializeColors();
int runMainMenu();
bool runConfirmExit();
int runLocationMenu(const std::vector<std::string> &names);
void showLoadingScreen(const std::string &message);
void showErrorScreen(const std::string &message);