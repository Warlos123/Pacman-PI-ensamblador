#pragma once

//GRID
constexpr int ROWS = 10;
constexpr int COLS = 20;

//MAZE
constexpr int EXTRA_EDGE_PROB = 10; //Out of 100

//POWER_UPS
constexpr int POWER_SPAWN_RATE = 8; //Out of 100, per cell
constexpr int POWER_PELLET_SPAWN_RATE = 10; //Out of 100, per cell
constexpr int MAX_POWER_PELLETS = 10; //Max per map
constexpr int MAX_JUMP_WALLS    = 10; //Max per map

enum class PowerUpType{
    NONE,
    JUMP_WALL,
    POWER_PELLET
};

//PACMAN
constexpr int PACMAN_LIVES = 3;

//POINTS
constexpr int PELLET_POINTS = 10; 
constexpr int POWER_PELLET_POINTS = 50; 
constexpr int GHOST_POINTS = 200; 
constexpr int LIVES_POINTS = 500;

//GHOST
constexpr int GHOST_BFS_RATE = 50; //Out of 100

constexpr double GHOST_SPEED_1 = 1.0;
constexpr double GHOST_SPEED_2 = 1.0;
constexpr double GHOST_SPEED_3 = 1.0;
constexpr double GHOST_SPEED_4 = 1.0;

constexpr double SCARED_TIMER = 8.0;


//PORTALS
constexpr int PORTAL_PAIRS = 2; //Fixed number of portal pairs per map


