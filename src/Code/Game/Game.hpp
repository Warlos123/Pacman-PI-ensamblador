#pragma once
#include <random>
#include <vector>
#include <unordered_set>
#include "../Graph/Graph.hpp"
#include "../MazeGenerator/MazeGenerator.hpp"
#include "../Path/Path.hpp"
#include "../Pacman/Pacman.hpp"
#include "../Ghost/Ghost.hpp"
#include "../Constants.hpp"

enum class GameState{ 
    PLAYING, 
    WIN, 
    LOSE 
};


class Game{
private:
    Graph graph_;
    MazeGenerator generator_;
    Path path_;
    Pacman pacman_;
    std::vector<Ghost> ghosts_;
    GameState state_;
    int pelletsRemaining_;
    std::mt19937 random_;
    bool powerPelletEaten_ = false;
    bool pelletEaten_ = false;
    bool jumpWallEaten_ = false;
    bool ghostEaten_ = false; 
    bool jumpWallUsed_ = false;
    int lifeBonus_ = 0;

    void populate(const std::unordered_set<int>& reserved);
    void checkGhostCollision();

public:
    Game();
    void init();

    bool movePacman(int targetNode);
    bool useJumpWall(int targetNode);
    void moveGhosts();

    GameState getState() const;
    Pacman& getPacman();
    std::vector<Ghost>& getGhosts();
    Graph& getGraph();
    bool consumePowerPelletEaten();
    bool consumePelletEaten();
    bool consumeJumpWallEaten();
    bool consumeGhostEaten();    
    bool consumeJumpWallUsed();
    int  getLifeBonus() const;
};