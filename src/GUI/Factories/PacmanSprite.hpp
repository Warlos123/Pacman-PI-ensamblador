#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

// Dibuja a Pac-Man como una cuna (circulo con boca) animada y orientada.
class PacmanSprite {
public:
    // center  : centro en pixeles
    // radius  : radio
    // dr,dc   : direccion (fila,col) hacia donde mira
    // moving  : si esta avanzando (anima la boca)
    // time    : segundos, para el "nam nam"
    static void draw(sf::RenderWindow& window, sf::Vector2f center, float radius,
                     int dr, int dc, bool moving, float time,
                     sf::Color color = sf::Color(255, 235, 0)) {
        const float PI = 3.14159265f;

        // Angulo segun direccion (en pantalla, +y es hacia abajo)
        float facing = 0.f;
        if      (dc > 0) facing = 0.f;     // derecha
        else if (dc < 0) facing = 180.f;   // izquierda
        else if (dr > 0) facing = 90.f;    // abajo
        else if (dr < 0) facing = 270.f;   // arriba

        // Apertura de la boca (medio angulo en grados)
        float half;
        if (moving) {
            float openness = 0.5f * (1.f + std::sin(time * 12.f)); // 0..1
            half = 6.f + openness * 34.f;   // entre 6 y 40 grados
        } else {
            half = 28.f;                    // boca fija cuando esta quieto
        }

        const int seg = 28;
        float start = facing + half;
        float end   = facing + 360.f - half;

        sf::ConvexShape pac;
        pac.setPointCount(seg + 2);
        pac.setPoint(0, center);            // vertice de la boca
        for (int i = 0; i <= seg; ++i) {
            float ang = (start + (end - start) * (i / float(seg))) * PI / 180.f;
            pac.setPoint(i + 1, { center.x + radius * std::cos(ang),
                                  center.y + radius * std::sin(ang) });
        }
        pac.setFillColor(color);
        window.draw(pac);
    }

    // Animacion de muerte: la boca se abre hasta consumir todo el circulo.
    // progress va de 0 (vivo) a 1 (desaparecido).
    static void drawDying(sf::RenderWindow& window, sf::Vector2f center, float radius,
                          float progress, sf::Color color = sf::Color(255, 235, 0)) {
        const float PI = 3.14159265f;
        float half = 10.f + progress * 170.f;   // 10 -> 180 grados
        if (half >= 179.f) return;               // ya desaparecio
        float facing = 270.f;                    // mira hacia arriba
        float start = facing + half;
        float end   = facing + 360.f - half;
        const int seg = 28;
        sf::ConvexShape pac;
        pac.setPointCount(seg + 2);
        pac.setPoint(0, center);
        for (int i = 0; i <= seg; ++i) {
            float ang = (start + (end - start) * (i / float(seg))) * PI / 180.f;
            pac.setPoint(i + 1, { center.x + radius * std::cos(ang),
                                  center.y + radius * std::sin(ang) });
        }
        pac.setFillColor(color);
        window.draw(pac);
    }
};