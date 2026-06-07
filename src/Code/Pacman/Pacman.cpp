#include "Pacman.hpp"

Pacman::Pacman(int node, int lives) : nodeIndex_(node){
    lives_ = PACMAN_LIVES;
}

int Pacman::getNodeIndex() const{
    return nodeIndex_;
}

void Pacman::setNodeIndex(int node){
    nodeIndex_ = node;
}


int Pacman::getScore() const{
    return score_;
}

void Pacman::addScore(int points){
    score_+=points;
}


int Pacman::getLives() const{
    return lives_;
}

void Pacman::loseLife(){
    lives_--;
}


void Pacman::addPowerUp(PowerUpType powerUp){
    powerUps_.push_back(powerUp);
}

bool Pacman::removePowerUp(PowerUpType powerUp){
    auto it = std::find(powerUps_.begin(), powerUps_.end(), powerUp);
    if(it != powerUps_.end()){
        powerUps_.erase(it);
        return true;
    }

    return false;
}

const std::vector<PowerUpType>& Pacman::getPowerUps(){
    return powerUps_;
}