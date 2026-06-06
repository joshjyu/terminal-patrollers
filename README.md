# Terminal Patrollers

A terminal stealth game where you navigate real-world city streets and escape autonomous patrollers. Every level is generated on the fly from live OpenStreetMap (OSM) data. 

**Several features are dependent on local microservices (a total of four) because I needed practice building them when I learned about microservices in school.**

## Features

- **Real-world maps:** levels are built from live OSM data via a local map microservice
- **Enemy patrollers:** enemies move independently on a background thread; adjacent tiles show their detection range
- **User profiles:** username, settings, and best times are saved between sessions
- **Best times leaderboard:** escape times are submitted after each win; user's top 10 scores are shown for that location
- **Adjustable difficulty:** patroller density is configurable from the options menu
- **Dark mode:** theme toggle that persists to your profile

## Prerequisites

This game depends on four locally-running microservices. All must be active before launching.

| Service | Port | Role |
|---|---|---|
| microgeo | 8000 | Reverse geocoding + OSM feature fetch |
| micromap | 8001 | Convert OSM data to game tiles |
| microleader | 8002 | Score submission and leaderboard |
| microprofile | 8003 | User profiles and persistent data |

Start each with:
```
uv run uvicorn <module>:app --port <port>
```

**Build dependencies:** `cmake`, `ninja`, `ncurses`, `libcurl`, `nlohmann-json`

## Build

```bash
cmake -B build -G Ninja
cmake --build build
```

Or use the CMake Tools extension in VSCode (F7).

Binary is output to `build/terminal_patrollers`.

## Run

```bash
./build/terminal_patrollers
```

On first launch you will be prompted to create a username. Your profile is stored via microprofile and your user ID is cached locally in `user_id.txt`.

## How to Play

| Key | Action |
|---|---|
| `W` / `↑` | Move up |
| `S` / `↓` | Move down |
| `A` / `←` | Move left |
| `D` / `→` | Move right |
| `ESC` | Pause |

- Avoid patrollers (`P`). If one reaches an adjacent tile, you are caught.
- Magenta-highlighted tiles show each patroller's detection range.
- Reach the exit tile (`E`) to escape and post your time to the leaderboard.

## Map Legend

| Symbol | Terrain |
|---|---|
| `@` | Player |
| `P` | Patroller |
| `E` | Exit |
| `#` | Building / wall (impassable) |
| `.` | Road / path (walkable) |
| `~` | Water (impassable) |
| `^` | Forest (impassable) |

## Available Locations

- MacArthur Park, Los Angeles
- Oregon State University
