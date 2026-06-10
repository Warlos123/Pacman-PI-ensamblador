#include "GamePlay.hpp"
#include <algorithm>

namespace {
    constexpr float MARGIN = 20.f;
    constexpr int   GHOST_STEP_MS = 300; // cadencia de los fantasmas

    const sf::Color FLOOR_COLOR(18, 18, 40);
    const sf::Color WALL_COLOR(40, 60, 200);
    const sf::Color PELLET_COLOR(250, 230, 180);
    const sf::Color POWER_PELLET_COLOR(255, 170, 60);
    const sf::Color JUMP_WALL_COLOR(80, 230, 120);
    const sf::Color PORTAL_COLOR(200, 80, 230);
    const sf::Color PACMAN_COLOR(255, 235, 0);
    const sf::Color SCARED_COLOR(40, 70, 255);

    const sf::Color GHOST_COLORS[4] = {
        sf::Color(230, 50, 50),
        sf::Color(255, 150, 220),
        sf::Color(0, 220, 230),
        sf::Color(255, 165, 60)
    };
}

GamePlay::GamePlay(const sf::Font& font, unsigned int winW, unsigned int winH)
    : font_(font) {
    game_.init();
    computeLayout(winW, winH);
    ghostClock_.restart();
}

void GamePlay::computeLayout(unsigned int winW, unsigned int winH){
    float availW = static_cast<float>(winW) - 2.f * MARGIN;
    float availH = static_cast<float>(winH) - hudHeight_ - 2.f * MARGIN;
    cellSize_ = std::min(availW / COLS, availH / ROWS);

    float gridW = cellSize_ * COLS;
    float gridH = cellSize_ * ROWS;
    originX_ = (static_cast<float>(winW) - gridW) / 2.f;
    originY_ = hudHeight_ + (static_cast<float>(winH) - hudHeight_ - gridH) / 2.f;
}

int GamePlay::directionToTarget(int currentNode, int dRow, int dCol) const{
    int row = currentNode / COLS;
    int col = currentNode % COLS;
    int nr = row + dRow;
    int nc = col + dCol;
    if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
        return -1;
    return nr * COLS + nc;
}

sf::Vector2f GamePlay::nodeTopLeft(int node) const{
    int row = node / COLS;
    int col = node % COLS;
    return { originX_ + col * cellSize_, originY_ + row * cellSize_ };
}

sf::Vector2f GamePlay::nodeCenter(int node) const{
    sf::Vector2f tl = nodeTopLeft(node);
    return { tl.x + cellSize_ / 2.f, tl.y + cellSize_ / 2.f };
}

void GamePlay::handleKey(sf::Keyboard::Key key, bool jumpModifier){
    if (game_.getState() != GameState::PLAYING)
        return;

    using K = sf::Keyboard::Key;
    int dr = 0, dc = 0;
    if      (key == K::Up    || key == K::W) dr = -1;
    else if (key == K::Down  || key == K::S) dr =  1;
    else if (key == K::Left  || key == K::A) dc = -1;
    else if (key == K::Right || key == K::D) dc =  1;
    else return;

    int current = game_.getPacman().getNodeIndex();
    int target  = directionToTarget(current, dr, dc);
    if (target < 0)
        return;

    if (jumpModifier)
        game_.useJumpWall(target);
    else
        game_.movePacman(target);
}

void GamePlay::update(){
    if (game_.getState() != GameState::PLAYING)
        return;
    if (ghostClock_.getElapsedTime().asMilliseconds() >= GHOST_STEP_MS){
        game_.moveGhosts();
        ghostClock_.restart();
    }
}

void GamePlay::draw(sf::RenderWindow& window){
    drawMaze(window);
    drawEntities(window);
    drawHUD(window);
}

void GamePlay::drawMaze(sf::RenderWindow& window){
    Graph& g = game_.getGraph();
    int total = ROWS * COLS;
    float t = std::max(2.f, cellSize_ * 0.10f);

    for (int i = 0; i < total; i++){
        sf::Vector2f tl = nodeTopLeft(i);

        sf::RectangleShape floor({cellSize_, cellSize_});
        floor.setPosition(tl);
        floor.setFillColor(FLOOR_COLOR);
        window.draw(floor);

        int row = i / COLS;
        int col = i % COLS;
        const int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};

        for (auto& d : dirs){
            int nr = row + d[0];
            int nc = col + d[1];
            bool blocked = (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                         ? true
                         : !g.hasEdge(i, nr * COLS + nc);
            if (!blocked) continue;

            sf::RectangleShape wall;
            wall.setFillColor(WALL_COLOR);
            if      (d[0] == -1){ wall.setSize({cellSize_, t}); wall.setPosition(tl); }
            else if (d[0] ==  1){ wall.setSize({cellSize_, t}); wall.setPosition({tl.x, tl.y + cellSize_ - t}); }
            else if (d[1] == -1){ wall.setSize({t, cellSize_}); wall.setPosition(tl); }
            else                { wall.setSize({t, cellSize_}); wall.setPosition({tl.x + cellSize_ - t, tl.y}); }
            window.draw(wall);
        }
    }
}

void GamePlay::drawEntities(sf::RenderWindow& window){
    Graph& g = game_.getGraph();
    int total = ROWS * COLS;

    for (int i = 0; i < total; i++){
        Cell& cell = g.getCell(i);
        sf::Vector2f c = nodeCenter(i);

        if (cell.isPortal){
            float r = cellSize_ * 0.28f;
            sf::CircleShape ring(r);
            ring.setOrigin({r, r});
            ring.setPosition(c);
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineThickness(std::max(2.f, cellSize_ * 0.08f));
            ring.setOutlineColor(PORTAL_COLOR);
            window.draw(ring);
        }
        else if (cell.powerUp == PowerUpType::POWER_PELLET){
            float r = cellSize_ * 0.22f;
            sf::CircleShape dot(r);
            dot.setOrigin({r, r}); dot.setPosition(c);
            dot.setFillColor(POWER_PELLET_COLOR);
            window.draw(dot);
        }
        else if (cell.powerUp == PowerUpType::JUMP_WALL){
            float s = cellSize_ * 0.30f;
            sf::RectangleShape sq({s, s});
            sq.setOrigin({s/2.f, s/2.f}); sq.setPosition(c);
            sq.setFillColor(JUMP_WALL_COLOR);
            window.draw(sq);
        }
        else if (cell.hasPellet){
            float r = cellSize_ * 0.08f;
            sf::CircleShape dot(r);
            dot.setOrigin({r, r}); dot.setPosition(c);
            dot.setFillColor(PELLET_COLOR);
            window.draw(dot);
        }
    }

    // Pacman
    {
        float r = cellSize_ * 0.36f;
        sf::CircleShape p(r);
        p.setOrigin({r, r});
        p.setPosition(nodeCenter(game_.getPacman().getNodeIndex()));
        p.setFillColor(PACMAN_COLOR);
        window.draw(p);
    }

    // Fantasmas
    auto& ghosts = game_.getGhosts();
    for (size_t i = 0; i < ghosts.size(); i++){
        float r = cellSize_ * 0.32f;
        sf::CircleShape gho(r);
        gho.setOrigin({r, r});
        gho.setPosition(nodeCenter(ghosts[i].getNodeIndex()));
        gho.setFillColor(ghosts[i].isScared() ? SCARED_COLOR : GHOST_COLORS[i % 4]);
        window.draw(gho);
    }
}

void GamePlay::drawHUD(sf::RenderWindow& window){
    Pacman& p = game_.getPacman();
    std::string hud = "Score: " + std::to_string(p.getScore())
                    + "    Vidas: " + std::to_string(p.getLives())
                    + "    JumpWall: " + std::to_string(p.getPowerUps().size());
    Text text(font_, hud, 24, {originX_, 15.f}, sf::Color::White);
    text.draw(window);
}

GameState GamePlay::getState() const{
    return game_.getState();
}

int GamePlay::getScore(){
    return game_.getPacman().getScore();
}