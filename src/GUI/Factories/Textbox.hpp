#pragma once 
#include <SFML/Graphics.hpp>
#include <sstream>
#include <string> 
#include "../ConstantsGUI.hpp"

class Textbox{

    private:
        sf::RectangleShape shape;
        sf::Text textbox; 
        std::ostringstream text;
        sf::Color normalColor;
        sf::Color hoverColor;


        int limit; 
        bool isVisible = false; 
        bool isSelected = false; 


    public: 
        Textbox(sf::Vector2f size, sf::Vector2f pos, int limit ,
            sf::Color normal, sf::Color hover, const sf::Font& font) : 
            shape(size),textbox(font) ,normalColor(normal), hoverColor(hover),limit(limit){

            shape.setPosition(pos);
            shape.setFillColor(normal);
            shape.setOutlineThickness(2.f);
            shape.setOutlineColor(sf::Color::Black);

            textbox.setCharacterSize(20);
            textbox.setFillColor(sf::Color::Black);
            textbox.setPosition(sf::Vector2f(pos.x + 5.f, pos.y + 5.f));
            

        }

        void update(sf::Vector2f mousePos){
            if(shape.getGlobalBounds().contains(sf::Vector2f(mousePos))){
                shape.setFillColor(hoverColor);
            }else{
                shape.setFillColor(normalColor);
            }
        }

        void draw(sf::RenderWindow& window){
            if(isVisible){
                window.draw(shape);
                window.draw(textbox);
            }
        }

        bool isClicked(sf::Vector2f clickPos){
            if(!isVisible){
                return false;
            }
            bool clicked = shape.getGlobalBounds().contains(clickPos);
            isSelected = clicked; 
            return clicked; 
        }

        void setVisible(bool visible){
            isVisible = visible;
        }

        void setSelected(bool selected){
            isSelected = selected;
        }

        bool getSelected(){
            return isSelected;
        }

        //Handle text in textbox
        void handleInput(const sf::Event& event){
            if(!isVisible || !isSelected) return;

            if(const auto* textEvent = event.getIf<sf::Event::TextEntered>()){
                char32_t unicode = textEvent->unicode; 

                if(unicode == 8){
                    std::string current = text.str();
                    if(!current.empty()){
                        current.pop_back();
                        text.str("");
                        text.clear();
                        text << current;
                    }
                }
                else if(unicode == 13 || unicode == 27){
                    isSelected = false;
                }

                else if(unicode >= 32 && unicode < 128){
                    if((int)text.str().size() < limit){
                        text << static_cast<char>(unicode);
                    }
                }
                textbox.setString(text.str());
            }
        }

        std::string getText() const{
            return text.str();
        }

        void clearText(){
            text.str("");
            text.clear();
            textbox.setString("");
        }
}; 