#include "Game.hpp"

Game::Game() : graph_(ROWS, COLS), path_(graph_), pacman_(0,0), state_(GameState::PLAYING), pelletsRemaining_(0) {
    std::random_device rd;
    random_ = std::mt19937(rd());
}


void Game::init(Algorithm a) {
    if (a == Algorithm::DFS){
        generator_.DFS(graph_);
    }
    else{
        generator_.BFS(graph_);
    }

    //Spawn Pacman in random node
    std::uniform_int_distribution<int> dist(0, ROWS * COLS - 1);
    int pacmanNode = dist(random_);
    pacman_.setNodeIndex(pacmanNode);

    //4 ghosts in corners, each have their own speed
    ghosts_.clear();
    ghosts_.push_back(Ghost(0, GHOST_SPEED_1));
    ghosts_.push_back(Ghost(COLS - 1, GHOST_SPEED_2));
    ghosts_.push_back(Ghost((ROWS - 1) * COLS, GHOST_SPEED_3));
    ghosts_.push_back(Ghost((ROWS - 1) * COLS + COLS - 1, GHOST_SPEED_4));


    std::unordered_set<int> reserved;
    reserved.insert(pacmanNode);
    for(auto& g : ghosts_)
        reserved.insert(g.getNodeIndex());

    populate(reserved);


    for (int i = 0; i < ROWS * COLS; i++){
        if (graph_.getCell(i).hasPellet)
            pelletsRemaining_++;
    }
}


