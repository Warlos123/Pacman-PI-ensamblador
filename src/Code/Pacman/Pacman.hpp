#pragma once
#include <vector>
#include <algorithm>
#include "../Constants.hpp"


class Pacman{
private:
    int nodeIndex_;
    std::vector<PowerUpType> powerUps_;
    int score_;
    int lives_;

public:
    Pacman(int node, int lives);

    int getNodeIndex() const;
    void setNodeIndex(int node);

    int getScore() const;
    void addScore(int points);

    int getLives() const;
    void loseLife();

    void addPowerUp(PowerUpType powerUp);
    bool removePowerUp(PowerUpType powerUp);
    const std::vector<PowerUpType>& getPowerUps();
};