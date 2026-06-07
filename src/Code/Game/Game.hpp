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

enum class Algorithm{ 
    DFS, 
    BFS 
};
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

    void populate(const std::unordered_set<int>& reserved);
    void checkGhostCollision();

public:
    Game();
    void init(Algorithm a);

    bool movePacman(int targetNode);
    bool useJumpWall(int targetNode);
    void moveGhosts();

    GameState getState() const;
    Pacman& getPacman();
    std::vector<Ghost>& getGhosts();
    Graph& getGraph();
};