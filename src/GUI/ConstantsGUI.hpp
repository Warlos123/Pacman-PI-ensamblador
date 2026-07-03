#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

namespace Constants{
    inline const unsigned WINDOW_WIDTH = 1280;
    inline const unsigned WINDOW_HEIGHT = 720;

    inline const sf::Color BUTTON_NOMRAL{184, 135, 70};
    inline const sf::Color BUTTON_HOVER{212, 161, 93}; 

    inline const sf::Color TEXTBOXNORMAL{209, 201, 201};
    inline const sf::Color TEXTBOXHOVER{176, 169, 169};

    inline const sf::Color TEXTCOLOR = sf::Color::White;

    inline sf::Font    font{"./Resource/PAC-FONT.TTF"};
    inline sf::Texture backGround{"./Resource/MainScreen.png"};
    inline sf::Font hudFont{"./Resource/HUD-FONT.ttf"};
    
    inline sf::Music       introMusic{"./Resource/pacman_intro.ogg"};

    inline sf::SoundBuffer wakaBuffer{"./Resource/pacman_waka.wav"};
    constexpr int WAKA_VOICES = 2;
    inline std::vector<sf::Sound> makeWakaVoices(){
        std::vector<sf::Sound> v;
        for (int i = 0; i < WAKA_VOICES; i++)
            v.emplace_back(wakaBuffer);
        return v;
    }

    inline std::vector<sf::Sound> wakaVoices = makeWakaVoices();
    inline void playWaka(){
        static size_t next = 0;
        wakaVoices[next].play();
        next = (next + 1) % wakaVoices.size();
    }

    inline sf::SoundBuffer ghostMoveBuffer{"./Resource/ghost_move.wav"};
    inline sf::SoundBuffer ghostScaredBuffer{"./Resource/ghost_scared.wav"};

    inline sf::Sound makeLoopingSound(sf::SoundBuffer& buf){
        sf::Sound s(buf);
        s.setLooping(true);
        return s;
    }

    inline sf::Sound ghostMoveSound   = makeLoopingSound(ghostMoveBuffer);
    inline sf::Sound ghostScaredSound = makeLoopingSound(ghostScaredBuffer);


    inline sf::SoundBuffer ghostEatenBuffer{"./Resource/ghost_eaten.wav"};
    constexpr int GHOST_EATEN_VOICES = 2;
    inline std::vector<sf::Sound> makeGhostEatenVoices(){
        std::vector<sf::Sound> v;
        for (int i = 0; i < GHOST_EATEN_VOICES; i++)
            v.emplace_back(ghostEatenBuffer);
        return v;
    }
    inline std::vector<sf::Sound> ghostEatenVoices = makeGhostEatenVoices();
    inline void playGhostEaten(){
        static size_t next = 0;
        ghostEatenVoices[next].play();
        next = (next + 1) % ghostEatenVoices.size();
    }


    inline sf::SoundBuffer deathBuffer{"./Resource/pacman_death.wav"};
    inline sf::Sound deathSound{deathBuffer};
}
    

    



