#pragma once 
#include <SFML/Graphics.hpp>
#include <string>
#include "../../Code/Game/Game.hpp"
#include "../Factories/Text.hpp"
#include "../GameState.hpp"
#include "../ConstantsGUI.hpp"


class GamePlay {
private:
    const sf::Font& font_;
    Game game_;

    float cellSize_  = 0.f;
    float originX_   = 0.f;
    float originY_   = 0.f;
    float hudHeight_ = 60.f;

    sf::Clock ghostClock_;

    void computeLayout(unsigned int winW, unsigned int winH);
    int  directionToTarget(int currentNode, int dRow, int dCol) const;
    sf::Vector2f nodeTopLeft(int node) const;
    sf::Vector2f nodeCenter(int node) const;

    void drawMaze(sf::RenderWindow& window);
    void drawEntities(sf::RenderWindow& window);
    void drawHUD(sf::RenderWindow& window);

public:
    GamePlay(const sf::Font& font, unsigned int winW, unsigned int winH);

    void handleKey(sf::Keyboard::Key key, bool jumpModifier);
    void update();
    void draw(sf::RenderWindow& window);

    GameState getState() const;
    int getScore();
};