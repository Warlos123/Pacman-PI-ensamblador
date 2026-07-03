#pragma once 
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "../../Code/Game/Game.hpp"
#include "../Factories/Text.hpp"
#include "../GameState.hpp"
#include "../ConstantsGUI.hpp"
#include "../SerialReader.hpp"


class GamePlay {
private:
    SerialReader joystick_{"COM3"}; //Puerto
    const sf::Font& font_;
    Game game_;

    float cellSize_  = 0.f;
    float originX_   = 0.f;
    float originY_   = 0.f;
    float hudHeight_ = 60.f;

    sf::Clock ghostClock_;
    sf::Clock pacClock_;
    sf::Clock animClock_;
    sf::Clock frightClock_;          // cronometro del modo asustado
    bool      frightActive_ = false; // modo asustado con temporizador activo

    int curDR_ = 0, curDC_ = 0;   // direccion actual de Pacman (fila, col)
    int wantDR_ = 0, wantDC_ = 0; // ultima direccion marcada por el jugador

    int pacPrev_ = 0, pacCur_ = 0;   // nodos previo/actual de Pacman (para animar)
    std::vector<int> ghostPrev_;     // nodo previo de cada fantasma (para animar)

    bool      dying_ = false;        // animacion de muerte en curso
    bool introActive_ = true;

    sf::Clock deathClock_;
    int       deathNode_ = 0;        // donde murio Pacman

    void stepPacman();            // avanza Pacman una casilla en su direccion
    void respawnAfterDeath();     // reubica Pacman y fantasmas tras morir
    // posicion en pixeles interpolada entre dos nodos (t de 0 a 1)
    sf::Vector2f interpCenter(int prevNode, int curNode, float t) const;

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
    void applyJoystick();
    void triggerJump();
    void updateGhostSound();
    void stopGhostSound(); 
};
