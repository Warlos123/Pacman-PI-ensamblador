#pragma once

//GRID
constexpr int ROWS = 10;
constexpr int COLS = 20;

//MAZE
constexpr int EXTRA_EDGE_PROB = 5; //Out of 100

//POWER_UPS
constexpr int POWER_SPAWN_RATE = 8; //Out of 100, per cell
constexpr int POWER_UP_TYPE_COUNT = 4; //JUMP_WALL, DOUBLE_PLAY, CONTROL_ENEMY, CHANGE_LOCATION

//PACMAN
constexpr int PACMAN_LIVES = 3;

//GHOST
constexpr int GHOST_BFS_RATE = 40;
constexpr int GHOST_SPEED_1 = 10;
constexpr int GHOST_SPEED_2 = 10;
constexpr int GHOST_SPEED_3 = 10;
constexpr int GHOST_SPEED_4 = 10;

enum class PowerUpType{
    NONE,
    JUMP_WALL,
    POWER_PELLET
};

//PORTALS
constexpr int PORTAL_PAIRS = 2; //Fixed number of portal pairs per map


