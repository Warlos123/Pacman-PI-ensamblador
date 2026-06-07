#include "Factories/Button.hpp"
#include "Factories/Text.hpp"
#include "ConstantsGUI.hpp"
#include "GameState.hpp"


class GUI{
    public:
        sf::RenderWindow window{sf::VideoMode({Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT}), "Maze Craze"};
        GameState state = GameState::MAINSCREEN; 
        GameState ctx; //Context shares info between gamestates 

    
        int run(){
            while(window.isOpen()){
                
            }
        }
    

};