#include <memory>
#include <string>
#include "Factories/Button.hpp"
#include "Factories/Text.hpp"
#include "ConstantsGUI.hpp"
#include "GameState.hpp"
#include "GamePlay/GamePlay.hpp"
#include "Scoreboard/Scoreboard.hpp"

namespace {
    const std::string PLAY_LABEL   = "Jugar";
    const std::string WIN_TEXT     = "GANASTE!";
    const std::string LOSE_TEXT    = "GAME OVER";
    const std::string SAVE_HINT    = "Escribe tu nombre y presiona Enter";
    const std::string REPLAY_HINT    = "Enter: jugar de nuevo    Esc: salir";

    constexpr unsigned int FPS_LIMIT  = 60;
    constexpr unsigned int END_SIZE   = 56;
    constexpr unsigned int SCORE_SIZE = 32;
    constexpr unsigned int NAME_SIZE  = 34;
    constexpr unsigned int LIST_SIZE  = 24;
    constexpr unsigned int HINT_SIZE  = 22;

    constexpr std::size_t  MAX_NAME_LEN = 12;

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

    Scoreboard  scoreboard_{"scores.txt"};
    std::string playerName_;
    bool nameSaved_ = false;

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
        gamePlay = std::make_unique<GamePlay>(Constants::font, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, scoreboard_.highScore());
        state = ScreenState::PLAYING;
    }

    void goToGameOver(){
        playerName_.clear();
        nameSaved_ = false;
        state = ScreenState::GAMEOVER;
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
            else if(state == ScreenState::GAMEOVER){
                if(!nameSaved_){
                    //escribir el nombre
                    if(const auto* te = event->getIf<sf::Event::TextEntered>()){
                        char32_t u = te->unicode;
                        if(u == 8){ // backspace
                            if(!playerName_.empty()) playerName_.pop_back();
                        }
                        else if(u >= 32 && u < 127 && u != ';' && playerName_.size() < MAX_NAME_LEN){
                            playerName_.push_back(static_cast<char>(u));
                        }
                    }
                    if(const auto* kp = event->getIf<sf::Event::KeyPressed>()){
                        if(kp->code == sf::Keyboard::Key::Enter){
                            scoreboard_.add(playerName_, gamePlay ? gamePlay->getScore() : 0);
                            nameSaved_ = true;
                        }
                    }
                }


                else{
                    //ya guardado, reiniciar o salir
                    if(const auto* kp = event->getIf<sf::Event::KeyPressed>()){
                        if(kp->code == sf::Keyboard::Key::Enter)       startGame();
                        else if(kp->code == sf::Keyboard::Key::Escape) window.close();
                    }
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
            if(gs == GameState::WIN || gs == GameState::LOSE){
                gamePlay->stopGhostSound(); 
                goToGameOver();
            }

        }
    }

    void render(){
       if(state == ScreenState::GAMEOVER) 
            window.clear(sf::Color::Black);

        else
            window.clear(BACKGROUND_COLOR);

        if(state == ScreenState::MAINSCREEN)    
            drawMainScreen();

        else if(state == ScreenState::PLAYING){ 
            if(gamePlay) gamePlay->draw(window); 
        }

        else if(state == ScreenState::GAMEOVER) 
            drawGameOver();

        window.display();
    }


    void drawCentered(const sf::Font& font, const std::string& s, unsigned size, float y, sf::Color color){
        Text t(font, s, size, {0.f, 0.f}, color);
        t.centerX(Constants::WINDOW_WIDTH, y);
        t.draw(window);
    }


    void drawMainScreen(){
        sf::Sprite bg(Constants::backGround);
        sf::Vector2u ts = Constants::backGround.getSize();
        bg.setScale({Constants::WINDOW_WIDTH / float(ts.x), Constants::WINDOW_HEIGHT / float(ts.y)});
        window.draw(bg);
        playButton.draw(window);
    }

    void drawGameOver(){
        bool win = gamePlay && gamePlay->getState() == GameState::WIN;
        int  score = gamePlay ? gamePlay->getScore() : 0;

        drawCentered(Constants::font, win ? WIN_TEXT : LOSE_TEXT, END_SIZE, 90.f, win ? sf::Color::Green : sf::Color::Red);
        drawCentered(Constants::hudFont, "SCORE  " + std::to_string(score), SCORE_SIZE, 190.f, sf::Color::White);
        int shownHigh = std::max(scoreboard_.highScore(), score);
        drawCentered(Constants::hudFont, "HIGH SCORE  " + std::to_string(shownHigh), SCORE_SIZE, 240.f, sf::Color(255, 235, 0));
        
        if(!nameSaved_){
            if(score > 0 && score >= scoreboard_.highScore())
                drawCentered(Constants::font, "NUEVO RECORD!", 30, 300.f, sf::Color(255, 235, 0));

            drawCentered(Constants::hudFont, "Nombre: " + playerName_ + "_", NAME_SIZE, 360.f, sf::Color::White);
            drawCentered(Constants::font, SAVE_HINT, HINT_SIZE, 430.f, sf::Color(180, 180, 190));
        }

        else{
            drawCentered(Constants::font, "BEST SCORES", 28, 310.f, sf::Color::White);
            float y = 350.f;
            int rank = 1;
            for(const auto& e : scoreboard_.top(5)){
                drawCentered(Constants::hudFont, std::to_string(rank) + ".  " + e.name + "   " + std::to_string(e.score), LIST_SIZE, y, sf::Color::White);
                y += 32.f;
                rank++;
            }

            drawCentered(Constants::font, REPLAY_HINT, HINT_SIZE, y + 20.f, sf::Color::White);
        }
    }
};

int main(){
    GUI gui;
    return gui.run();
}