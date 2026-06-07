#pragma once
#include <SFML/Graphics.hpp>

namespace Constants{
    inline const unsigned WINDOW_WIDTH = 1200;
    inline const unsigned WINDOW_HEIGHT = 800;

    inline const sf::Color BUTTON_NOMRAL{184, 135, 70};
    inline const sf::Color BUTTON_HOVER{212, 161, 93}; 

    inline const sf::Color TEXTBOXNORMAL{209, 201, 201};
    inline const sf::Color TEXTBOXHOVER{176, 169, 169};

    inline const sf::Color TEXTCOLOR{246, 231, 193};

    inline sf::Font font{"./Resource/PAC-FONT.ttf"};
    inline sf::Texture backGround{"./Resource/MainScreen.png"};
    inline sf::Texture subWindow{"./Resource/blurredMainScreen.jpg"};
    

    
}
    



