#include <memory>
#include <string>
#include "Factories/Button.hpp"
#include "Factories/Text.hpp"
#include "ConstantsGUI.hpp"
#include "GameState.hpp"
#include "GamePlay/GamePlay.hpp"

namespace {
    const std::string PLAY_LABEL   = "Jugar";
    const std::string WIN_TEXT     = "GANASTE!";
    const std::string LOSE_TEXT    = "GAME OVER";
    const std::string HINT_TEXT    = "Enter: jugar de nuevo    Esc: salir";

    constexpr unsigned int FPS_LIMIT  = 60;
    constexpr unsigned int TITLE_SIZE = 64;
    constexpr unsigned int END_SIZE   = 48;
    constexpr unsigned int HINT_SIZE  = 24;

    constexpr float TITLE_Y     = 120.f;
    constexpr float RESULT_OFF  = 40.f; // desplazamiento vertical del texto final
    constexpr float HINT_OFF    = 30.f;

    const sf::Vector2f BUTTON_SIZE{220.f, 70.f};
    const sf::Color    BACKGROUND_COLOR{10, 10, 25};

    const float CENTER_X = Constants::WINDOW_WIDTH  / 2.f;
    const float CENTER_Y = Constants::WINDOW_HEIGHT / 2.f;
}

class GUI{
public:
    sf::RenderWindow window{sf::VideoMode({Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT}), "Pac-Man"};
    ScreenState state = ScreenState::MAINSCREEN;
    std::unique_ptr<GamePlay> gamePlay;

    Button playButton{Constants::font, PLAY_LABEL, BUTTON_SIZE,
        {CENTER_X - BUTTON_SIZE.x / 2.f, CENTER_Y - BUTTON_SIZE.y / 2.f},
        Constants::BUTTON_NOMRAL, Constants::BUTTON_HOVER};

    int run(){
        window.setFramerateLimit(FPS_LIMIT);
        playButton.setVisible(true);
        while(window.isOpen()){
            processEvents();
            update();
            render();
        }
        return 0;
    }

private:
    void startGame(){
        gamePlay.reset(); // libera el SerialReader (y el puerto COM) del juego anterior antes de crear el nuevo
        gamePlay = std::make_unique<GamePlay>(Constants::font, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
        state = ScreenState::PLAYING;
    }

    void processEvents(){
        while(const std::optional event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()){ window.close(); return; }

            if(state == ScreenState::MAINSCREEN){
                if(const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()){
                    if(playButton.isClicked({float(mb->position.x), float(mb->position.y)}))
                        startGame();
                }
            }
            else if(state == ScreenState::PLAYING){
                if(const auto* kp = event->getIf<sf::Event::KeyPressed>()){
                    bool jump = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)
                             || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
                    gamePlay->handleKey(kp->code, jump);
                }
            }
            else if(state == ScreenState::VICTORY){
                if(const auto* kp = event->getIf<sf::Event::KeyPressed>()){
                    if(kp->code == sf::Keyboard::Key::Enter)       startGame();
                    else if(kp->code == sf::Keyboard::Key::Escape) window.close();
                }
            }
        }
    }

    void update(){
        if(state == ScreenState::MAINSCREEN){
            sf::Vector2i mp = sf::Mouse::getPosition(window);
            playButton.update({float(mp.x), float(mp.y)});
        }
        else if(state == ScreenState::PLAYING && gamePlay){
            gamePlay->update();
            GameState gs = gamePlay->getState();
            if(gs == GameState::WIN || gs == GameState::LOSE)
                state = ScreenState::VICTORY;
        }
    }

    void render(){
        window.clear(BACKGROUND_COLOR);
        if(state == ScreenState::MAINSCREEN)        drawMainScreen();
        else if(state == ScreenState::PLAYING)      { if(gamePlay) gamePlay->draw(window); }
        else if(state == ScreenState::VICTORY)      drawVictory();
        window.display();
    }

    void drawMainScreen(){
        sf::Sprite bg(Constants::backGround);
        sf::Vector2u ts = Constants::backGround.getSize();
        bg.setScale({Constants::WINDOW_WIDTH / float(ts.x), Constants::WINDOW_HEIGHT / float(ts.y)});
        window.draw(bg);

        playButton.draw(window);
    }

    void drawVictory(){
        if(gamePlay) gamePlay->draw(window);
        bool win = gamePlay && gamePlay->getState() == GameState::WIN;

        Text result(Constants::font,
            std::string(win ? WIN_TEXT : LOSE_TEXT) + "  Score: "
            + std::to_string(gamePlay ? gamePlay->getScore() : 0),
            END_SIZE, {0.f, 0.f}, win ? sf::Color::Green : sf::Color::Red);
        result.centerX(Constants::WINDOW_WIDTH, CENTER_Y - RESULT_OFF);

        Text hint(Constants::font, HINT_TEXT, HINT_SIZE, {0.f, 0.f}, sf::Color::White);
        hint.centerX(Constants::WINDOW_WIDTH, CENTER_Y + HINT_OFF);

        result.draw(window);
        hint.draw(window);
    }
};

int main(){
    GUI gui;
    return gui.run();
}