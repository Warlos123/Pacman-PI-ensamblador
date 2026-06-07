#pragma once 
#include <SFML/Graphics.hpp>
#include <string>

class Text{
    private: 
        sf::Text text;
        
    public:
        Text(const sf::Font& font, const std::string& content,
             unsigned int size, sf::Vector2f pos, sf::Color color)
             : text (font, content, size){
                text.setFillColor(color);
                text.setPosition(pos);
                text.setOutlineThickness(2.f);
             }

        void centerX(unsigned int windowWidth, float y){
            sf::FloatRect tb = text.getLocalBounds();
            text.setPosition({
                (windowWidth - tb.size.x) / 2.f - tb.position.x, y
            });
        }

        void draw(sf::RenderWindow& window){
            window.draw(text);
        }

        void centerOn(sf::Vector2f point){      //Center text on main screen
            sf::FloatRect txt = text.getLocalBounds();
            text.setOrigin({
                txt.position.x + txt.size.x / 2.f,
                txt.position.y + txt.size.y / 2.f
            });
            text.setPosition(point);
        }
};
