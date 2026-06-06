cmake_minimum_required(VERSION 3.15)
project(terminal_patrollers LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)

include(FetchContent)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.14.2
)
set(CPR_USE_SYSTEM_CURL ON)
FetchContent_MakeAvailable(cpr)

add_executable(terminal_patrollers
    src/main.cpp
    src/ui.cpp
    src/game.cpp
    src/map_service.cpp
    src/profile_service.cpp
    src/leaderboard_service.cpp)
target_link_libraries(terminal_patrollers ncurses Threads::Threads cpr::cpr nlohmann_json::nlohmann_json)