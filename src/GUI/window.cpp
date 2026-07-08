#include <memory>
#include <string>
#include "Factories/Button.hpp"
#include "Factories/Text.hpp"
#include "ConstantsGUI.hpp"
#include "GameState.hpp"
#include "GamePlay/GamePlay.hpp"
#include "Scoreboard/Scoreboard.hpp"

namespace {
    const std::string PLAY_LABEL   = "START";
    const std::string WIN_TEXT     = "YOU WIN!";
    const std::string LOSE_TEXT    = "GAME OVER";
    const std::string SAVE_HINT    = "Escribe tu nombre y presiona Enter";
    const std::string REPLAY_HINT    = "Enter: jugar de nuevo    Esc: salir";
    const std::string TITLE_TEXT  = "PACMAN";

    constexpr unsigned int FPS_LIMIT  = 60;
    constexpr unsigned int END_SIZE   = 56;
    constexpr unsigned int SCORE_SIZE = 32;
    constexpr unsigned int NAME_SIZE  = 34;
    constexpr unsigned int LIST_SIZE  = 24;
    constexpr unsigned int HINT_SIZE  = 22;
    constexpr unsigned int TITLE_SIZE = 96;

    constexpr float TITLE_Y = 200.f;
    constexpr float BUTTON_Y = 420.f; 

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

    Button playButton{Constants::hudFont, PLAY_LABEL, BUTTON_SIZE,
        {CENTER_X - BUTTON_SIZE.x / 2.f, BUTTON_Y},
        sf::Color::Black, sf::Color(30, 30, 30)};

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
                            int finalScore = gamePlay ? gamePlay->getScore() + gamePlay->getLifeBonus() : 0;
                            scoreboard_.add(playerName_, finalScore);
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
        if(state == ScreenState::MAINSCREEN || state == ScreenState::GAMEOVER)
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
        drawCentered(Constants::font, TITLE_TEXT, TITLE_SIZE, TITLE_Y, sf::Color(255, 235, 0));
        playButton.draw(window);
    }

    void drawGameOver(){
        bool win   = gamePlay && gamePlay->getState() == GameState::WIN;
        int  score = gamePlay ? gamePlay->getScore() : 0;   
        int  bonus = win ? gamePlay->getLifeBonus() : 0;   
        int  finalScore = score + bonus;                   

        drawCentered(Constants::font, win ? WIN_TEXT : LOSE_TEXT, END_SIZE, 90.f, win ? sf::Color::Green : sf::Color::Red);

        {
            std::string scoreStr = "SCORE  " + std::to_string(score);
            std::string bonusStr = bonus > 0 ? ("   +" + std::to_string(bonus)) : "";

            Text scoreText(Constants::hudFont, scoreStr, SCORE_SIZE, {0.f, 0.f}, sf::Color::White);
            Text bonusText(Constants::hudFont, bonusStr, SCORE_SIZE, {0.f, 0.f}, sf::Color(255, 235, 0));

            float total  = scoreText.getWidth() + bonusText.getWidth();
            float startX = (Constants::WINDOW_WIDTH - total) / 2.f;
            float y      = 190.f;

            scoreText.setPosition({startX, y});
            bonusText.setPosition({startX + scoreText.getWidth(), y});

            scoreText.draw(window);
            if(bonus > 0) bonusText.draw(window);
        }

        int shownHigh = std::max(scoreboard_.highScore(), finalScore);
        drawCentered(Constants::hudFont, "HIGH SCORE  " + std::to_string(shownHigh), SCORE_SIZE, 240.f, sf::Color(255, 235, 0));

        if(!nameSaved_){
            if(finalScore > 0 && finalScore >= scoreboard_.highScore())
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