#include "GamePlay.hpp"
#include <algorithm>
#include "../Factories/PacmanSprite.hpp"
#include "../Factories/GhostSprite.hpp"

namespace {
    constexpr float MARGIN = 20.f;
    constexpr int   GHOST_STEP_MS  = 300; // cadencia de los fantasmas
    constexpr int   PACMAN_STEP_MS = 150; // cadencia de avance de Pacman

    const sf::Color FLOOR_COLOR(18, 18, 40);
    const sf::Color WALL_COLOR(40, 60, 200);
    const sf::Color PELLET_COLOR(250, 230, 180);
    const sf::Color POWER_PELLET_COLOR(255, 170, 60);
    const sf::Color JUMP_WALL_COLOR(80, 230, 120);
    const sf::Color PORTAL_COLOR(200, 80, 230);
    const sf::Color PACMAN_COLOR(255, 235, 0);
    const sf::Color SCARED_COLOR(40, 70, 255);

    const sf::Color GHOST_COLORS[4] = {
        sf::Color(255,   0,   0), // Blinky - rojo
        sf::Color(255, 184, 255), // Pinky  - rosado
        sf::Color(0,   255, 255), // Inky   - cyan
        sf::Color(255, 184,  82)  // Clyde  - naranja
    };

    constexpr int DEATH_MS = 1100; // duracion de la animacion de muerte
}

GamePlay::GamePlay(const sf::Font& font, unsigned int winW, unsigned int winH)
    : font_(font) {
    game_.init();
    computeLayout(winW, winH);
    ghostClock_.restart();
    pacClock_.restart();

    // Estado inicial para la animacion
    pacPrev_ = pacCur_ = game_.getPacman().getNodeIndex();
    auto& gs = game_.getGhosts();
    ghostPrev_.resize(gs.size());
    for (size_t i = 0; i < gs.size(); ++i)
        ghostPrev_[i] = gs[i].getNodeIndex();
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

sf::Vector2f GamePlay::interpCenter(int prevNode, int curNode, float t) const{
    sf::Vector2f a = nodeCenter(prevNode);
    sf::Vector2f b = nodeCenter(curNode);

    int dr = (prevNode / COLS) - (curNode / COLS); if (dr < 0) dr = -dr;
    int dc = (prevNode % COLS) - (curNode % COLS); if (dc < 0) dc = -dc;

    // Solo desliza entre casillas vecinas; portal/reaparicion -> salta al destino.
    if (dr + dc != 1)
        return b;

    return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
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

    if (jumpModifier){
        // Shift + direccion: usa un JumpWall para cruzar una pared (una vez)
        int current = game_.getPacman().getNodeIndex();
        int target  = directionToTarget(current, dr, dc);
        if (target >= 0 && game_.useJumpWall(target)){
            curDR_ = dr; curDC_ = dc; // sigue avanzando en esa direccion
        }
        return;
    }

    // Direccion normal: solo registra la direccion deseada.
    // El avance continuo lo hace stepPacman() en cada tick.
    wantDR_ = dr; wantDC_ = dc;
}

void GamePlay::stepPacman(){
    int current = game_.getPacman().getNodeIndex();

    // Si hay una direccion deseada y es posible, gira hacia ella.
    if (wantDR_ != 0 || wantDC_ != 0){
        int t = directionToTarget(current, wantDR_, wantDC_);
        if (t >= 0 && game_.getGraph().hasEdge(current, t)){
            curDR_ = wantDR_; curDC_ = wantDC_;
        }
    }

    // Todavia sin direccion: Pacman queda quieto.
    if (curDR_ == 0 && curDC_ == 0){
        pacPrev_ = pacCur_ = current;
        return;
    }

    // Avanza una casilla en la direccion actual si no hay pared.
    int target = directionToTarget(current, curDR_, curDC_);
    if (target >= 0 && game_.getGraph().hasEdge(current, target)){
        game_.movePacman(target);
        pacPrev_ = current;
        pacCur_  = game_.getPacman().getNodeIndex(); // puede saltar por un portal
    } else {
        // Pared: se detiene y conserva su direccion.
        pacPrev_ = pacCur_ = current;
    }
}

void GamePlay::update(){
    // Durante la muerte, todo se congela hasta que termina la animacion.
    if (dying_){
        if (deathClock_.getElapsedTime().asMilliseconds() >= DEATH_MS){
            dying_ = false;
            if (game_.getPacman().getLives() > 0)
                respawnAfterDeath();
            // si no quedan vidas, al salir getState() devolvera LOSE
        }
        return;
    }

    if (game_.getState() != GameState::PLAYING)
        return;

    int livesBefore = game_.getPacman().getLives();

    // Pacman avanza solo, a su propia cadencia.
    if (pacClock_.getElapsedTime().asMilliseconds() >= PACMAN_STEP_MS){
        stepPacman();
        pacClock_.restart();
    }

    if (ghostClock_.getElapsedTime().asMilliseconds() >= GHOST_STEP_MS){
        auto& gs = game_.getGhosts();
        ghostPrev_.resize(gs.size());
        for (size_t i = 0; i < gs.size(); ++i)
            ghostPrev_[i] = gs[i].getNodeIndex();
        game_.moveGhosts();
        ghostClock_.restart();
    }

    // Si Pacman perdio una vida en este paso, arranca la animacion de muerte.
    if (game_.getPacman().getLives() < livesBefore){
        dying_ = true;
        deathClock_.restart();
        deathNode_ = game_.getPacman().getNodeIndex();
    }
}

void GamePlay::draw(sf::RenderWindow& window){
    drawMaze(window);

    if (dying_){
        // Animacion de muerte: solo Pacman abriendo la boca, sin fantasmas.
        float prog = std::min(1.f,
            deathClock_.getElapsedTime().asMilliseconds() / float(DEATH_MS));
        float r = cellSize_ * 0.42f;
        PacmanSprite::drawDying(window, nodeCenter(deathNode_), r, prog);
        drawHUD(window);
        return;
    }

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
        float r = cellSize_ * 0.42f;
        float tp = std::min(1.f,
            pacClock_.getElapsedTime().asMilliseconds() / float(PACMAN_STEP_MS));
        sf::Vector2f pos = interpCenter(pacPrev_, pacCur_, tp);
        bool moving = (pacPrev_ != pacCur_);
        int fdr = curDR_, fdc = curDC_;
        if (fdr == 0 && fdc == 0) fdc = 1; // por defecto mira a la derecha
        PacmanSprite::draw(window, pos, r, fdr, fdc, moving,
                           animClock_.getElapsedTime().asSeconds(), PACMAN_COLOR);
    }

    // Fantasmas
    auto& ghosts = game_.getGhosts();
    int pacNode = game_.getPacman().getNodeIndex();
    for (size_t i = 0; i < ghosts.size(); i++){
        float r = cellSize_ * 0.40f;
        float tg = std::min(1.f,
            ghostClock_.getElapsedTime().asMilliseconds() / float(GHOST_STEP_MS));
        int curNode  = ghosts[i].getNodeIndex();
        int prevNode = (i < ghostPrev_.size()) ? ghostPrev_[i] : curNode;
        sf::Vector2f pos = interpCenter(prevNode, curNode, tg);

        // Mirada: hacia donde se mueve; si esta quieto, hacia Pacman.
        int lookDx = (curNode % COLS) - (prevNode % COLS);
        int lookDy = (curNode / COLS) - (prevNode / COLS);
        if (lookDx == 0 && lookDy == 0){
            int ddx = (pacNode % COLS) - (curNode % COLS);
            int ddy = (pacNode / COLS) - (curNode / COLS);
            int adx = ddx < 0 ? -ddx : ddx;
            int ady = ddy < 0 ? -ddy : ddy;
            if (adx >= ady){ lookDx = (ddx > 0) - (ddx < 0); lookDy = 0; }
            else           { lookDy = (ddy > 0) - (ddy < 0); lookDx = 0; }
        } else {
            lookDx = (lookDx > 0) - (lookDx < 0);
            lookDy = (lookDy > 0) - (lookDy < 0);
        }

        sf::Color col = ghosts[i].isScared() ? SCARED_COLOR : GHOST_COLORS[i % 4];
        GhostSprite::draw(window, pos, r, col, lookDx, lookDy, ghosts[i].isScared());
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
    if (dying_)
        return GameState::PLAYING; // no cambiar de pantalla durante la muerte
    return game_.getState();
}

int GamePlay::getScore(){
    return game_.getPacman().getScore();
}

void GamePlay::respawnAfterDeath(){
    int center = (ROWS / 2) * COLS + (COLS / 2);
    game_.getPacman().setNodeIndex(center);

    auto& gs = game_.getGhosts();
    int corners[4] = { 0, COLS - 1, (ROWS - 1) * COLS, (ROWS - 1) * COLS + COLS - 1 };
    for (size_t i = 0; i < gs.size(); ++i){
        gs[i].setNodeIndex(corners[i % 4]);
        gs[i].setScared(false);
    }

    curDR_ = curDC_ = 0;
    wantDR_ = wantDC_ = 0;
    pacPrev_ = pacCur_ = center;
    ghostPrev_.resize(gs.size());
    for (size_t i = 0; i < gs.size(); ++i)
        ghostPrev_[i] = gs[i].getNodeIndex();

    pacClock_.restart();
    ghostClock_.restart();
}