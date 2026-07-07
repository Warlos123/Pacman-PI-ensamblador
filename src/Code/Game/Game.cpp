#include "Game.hpp"

Game::Game() : graph_(ROWS, COLS), path_(graph_), pacman_(0,PACMAN_LIVES), state_(GameState::PLAYING), pelletsRemaining_(0) {
    std::random_device rd;
    random_ = std::mt19937(rd());
}


void Game::init() {

    generator_.DFS(graph_);

    //Spawn Pacman in center node
    int pacmanNode = (ROWS / 2) * COLS + (COLS / 2);
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



void Game::populate(const std::unordered_set<int>& reserved){
    int total = ROWS * COLS;

    std::vector<int> candidates;
    candidates.reserve(total);
    for (int i = 0; i < total; i++){
        Cell& cell = graph_.getCell(i);

        if (reserved.count(i)){
            cell.hasPellet = false;
            continue;
        }

        cell.hasPellet = true;
        candidates.push_back(i);
    }

    //Baraja para que los power-ups queden repartidos por todo el mapa.
    std::shuffle(candidates.begin(), candidates.end(), random_);

    std::uniform_int_distribution<int> rateDist(1, 100);
    int powerPelletCount = 0;
    int jumpWallCount    = 0;

    for (int i : candidates){
        Cell& cell = graph_.getCell(i);


        if (powerPelletCount < MAX_POWER_PELLETS && rateDist(random_) <= POWER_PELLET_SPAWN_RATE){
            cell.powerUp = PowerUpType::POWER_PELLET;
            cell.hasPellet = false;
            powerPelletCount++;
        }

        else if (jumpWallCount < MAX_JUMP_WALLS && rateDist(random_) <= POWER_SPAWN_RATE){
            cell.powerUp = PowerUpType::JUMP_WALL;
            cell.hasPellet = false;
            jumpWallCount++;
        }
    }


    std::vector<int> freeCells;
    freeCells.reserve(total);
    for (int i = 0; i < total; i++){
        if (reserved.count(i)) continue;
        if (graph_.getCell(i).isPortal) continue;
        if (graph_.getCell(i).powerUp != PowerUpType::NONE) continue;
        freeCells.push_back(i);
    }

    for (int p = 0; p < PORTAL_PAIRS; p++){
        if (freeCells.size() < 2)
            break;

        std::uniform_int_distribution<int> pick(0, freeCells.size() - 1);

        int aIdx = pick(random_);
        int a = freeCells[aIdx];
        std::swap(freeCells[aIdx], freeCells.back());
        freeCells.pop_back();

        int bIdx = pick(random_);
        int b = freeCells[bIdx];
        std::swap(freeCells[bIdx], freeCells.back());
        freeCells.pop_back();

        Cell& ca = graph_.getCell(a);
        Cell& cb = graph_.getCell(b);
        ca.isPortal = true;
        ca.portalTarget = b;
        ca.portalId = p;
        ca.hasPellet = false;
        cb.isPortal = true;
        cb.portalTarget = a;
        cb.portalId = p;
        cb.hasPellet = false;
    }
}


bool Game::movePacman(int targetNode){
    int current = pacman_.getNodeIndex();

    bool hasEdge = graph_.hasEdge(current, targetNode);
    if(!hasEdge)
        return false;

    pacman_.setNodeIndex(targetNode);
    Cell& cell = graph_.getCell(targetNode);

    //Portal
    if(cell.isPortal){
        pacman_.setNodeIndex(cell.portalTarget);
        cell.hasPellet = false;
    }

    //Eat pellet
    if(cell.hasPellet){
        cell.hasPellet = false;
        pacman_.addScore(PELLET_POINTS);
        pelletsRemaining_--;
        pelletEaten_ = true;

        if(pelletsRemaining_ == 0){
            state_ = GameState::WIN;
            return true;
        }
    }


    //PowerUp
    if(cell.powerUp == PowerUpType::POWER_PELLET){
        for(auto& ghost : ghosts_){
            ghost.setScared(true);
        }
        pacman_.addScore(POWER_PELLET_POINTS);
        powerPelletEaten_ = true;
        cell.powerUp = PowerUpType::NONE;
    }

    else if(cell.powerUp == PowerUpType::JUMP_WALL){
        pacman_.addPowerUp(PowerUpType::JUMP_WALL);
        jumpWallEaten_ = true;
        cell.powerUp = PowerUpType::NONE;
    }

    checkGhostCollision();

    return true;
}



void Game::checkGhostCollision(){
    for (int i = 0; i < ghosts_.size(); i++) {
        if (ghosts_[i].getNodeIndex() != pacman_.getNodeIndex()) //Ghost no esta en la celda de Pacman
            continue;

        if (ghosts_[i].isScared()){ //Pacman CAN eat ghosts
            pacman_.addScore(GHOST_POINTS);
            ghosts_[i].setScared(false);
            ghostEaten_ = true;

            // Al ser comido, el fantasma reaparece en el centro (como la casa de fantasmas).
            ghosts_[i].setNodeIndex((ROWS / 2) * COLS + (COLS / 2));

            continue;
        }

        //Pacman CAN'T eat ghosts, lose life
        pacman_.loseLife();
        if(pacman_.getLives() == 0)
            state_ = GameState::LOSE;
            
        return;
    }
}



bool Game::useJumpWall(int targetNode){
    if (pacman_.getPowerUps().empty())
        return false;

    std::vector<int> gridNeighbors = graph_.getNeighbors(pacman_.getNodeIndex());
    auto it = std::find(gridNeighbors.begin(), gridNeighbors.end(), targetNode);
    if (it == gridNeighbors.end())
        return false;

    if(graph_.hasEdge(pacman_.getNodeIndex(), targetNode))
        return false;
        
    pacman_.setNodeIndex(targetNode);
    pacman_.removePowerUp(PowerUpType::JUMP_WALL);
    checkGhostCollision();
    return true;
}



void Game::moveGhosts(){
    for(auto& ghost : ghosts_){
        ghost.move(graph_, path_, pacman_.getNodeIndex());
    }
    checkGhostCollision();
}



GameState Game::getState() const{
    return state_;
}

Pacman& Game::getPacman(){
    return pacman_;
}

std::vector<Ghost>& Game::getGhosts(){
    return ghosts_;
}

Graph& Game::getGraph(){
    return graph_;
}

bool Game::consumePowerPelletEaten(){
    bool v = powerPelletEaten_;
    powerPelletEaten_ = false;
    return v;
}


bool Game::consumePelletEaten(){
    bool v = pelletEaten_;
    pelletEaten_ = false;
    return v;
}


bool Game::consumeJumpWallEaten(){
    bool v = jumpWallEaten_;
    jumpWallEaten_ = false;
    return v;
}


bool Game::consumeGhostEaten(){
    bool v = ghostEaten_;
    ghostEaten_ = false;
    return v;
}