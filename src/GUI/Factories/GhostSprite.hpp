#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

// Dibuja un fantasma clasico: domo + faldon ondulado + dos ojos.
class GhostSprite {
public:
    // center            : centro en pixeles
    // radius            : mitad del ancho
    // bodyColor         : color del cuerpo
    // lookDx, lookDy    : hacia donde miran los ojos (-1,0,1)
    // scared            : modo asustado (azul)
    static void draw(sf::RenderWindow& window, sf::Vector2f center, float radius,
                     sf::Color bodyColor, int lookDx, int lookDy, bool scared) {
        const float PI = 3.14159265f;
        float cx = center.x, cy = center.y;
        float left = cx - radius, right = cx + radius;
        float bottom = cy + radius;
        float skirtH = radius * 0.35f;
        float vy = bottom - skirtH;         // linea de los valles del faldon

        if (scared) bodyColor = sf::Color(33, 33, 222);

        // ----- Cuerpo: domo (arco superior) + rectangulo -----
        const int arcSeg = 20;
        sf::ConvexShape body;
        body.setPointCount(arcSeg + 1 + 2);
        for (int i = 0; i <= arcSeg; ++i) {
            float ang = (180.f + 180.f * (i / float(arcSeg))) * PI / 180.f;
            body.setPoint(i, { cx + radius * std::cos(ang),
                               cy + radius * std::sin(ang) });
        }
        body.setPoint(arcSeg + 1, { right, vy });
        body.setPoint(arcSeg + 2, { left,  vy });
        body.setFillColor(bodyColor);
        window.draw(body);

        // ----- Faldon: triangulos hacia abajo -----
        int tips = 4;
        float segW = (right - left) / tips;
        for (int i = 0; i < tips; ++i) {
            float x0 = left + i * segW;
            float x1 = x0 + segW;
            float xm = (x0 + x1) * 0.5f;
            sf::ConvexShape tri;
            tri.setPointCount(3);
            tri.setPoint(0, { x0, vy });
            tri.setPoint(1, { xm, bottom });
            tri.setPoint(2, { x1, vy });
            tri.setFillColor(bodyColor);
            window.draw(tri);
        }

        // ----- Cara asustada -----
        if (scared) {
            float er = radius * 0.16f;
            for (int s = -1; s <= 1; s += 2) {
                sf::CircleShape eye(er);
                eye.setOrigin({er, er});
                eye.setPosition({cx + s * radius * 0.38f, cy - radius * 0.12f});
                eye.setFillColor(sf::Color(235, 235, 255));
                window.draw(eye);
            }
            // boca ondulada (zigzag) blanca
            float my   = cy + radius * 0.30f;
            float mw   = radius * 1.30f;
            float mh   = radius * 0.16f;
            int   zig  = 4;
            float x0   = cx - mw * 0.5f;
            float step = mw / zig;
            for (int i = 0; i < zig; ++i) {
                float xa = x0 + i * step;
                float xb = xa + step;
                float xm = (xa + xb) * 0.5f;
                sf::ConvexShape t;
                t.setPointCount(3);
                if (i % 2 == 0) { t.setPoint(0,{xa,my}); t.setPoint(1,{xm,my-mh}); t.setPoint(2,{xb,my}); }
                else            { t.setPoint(0,{xa,my-mh}); t.setPoint(1,{xm,my}); t.setPoint(2,{xb,my-mh}); }
                t.setFillColor(sf::Color::White);
                window.draw(t);
            }
            return;
        }

        // ----- Ojos -----
        float eyeR    = radius * 0.32f;
        float eyeOffX = radius * 0.40f;
        float eyeY    = cy - radius * 0.10f;
        float pupR    = eyeR * 0.5f;

        float lx = float(lookDx), ly = float(lookDy);
        float len = std::sqrt(lx * lx + ly * ly);
        if (len > 0.001f) { lx /= len; ly /= len; } else { lx = 0.f; ly = 0.f; }
        float pupShift = eyeR * 0.42f;

        for (int s = -1; s <= 1; s += 2) {
            sf::Vector2f ec(cx + s * eyeOffX, eyeY);

            sf::CircleShape sclera(eyeR);
            sclera.setOrigin({eyeR, eyeR});
            sclera.setPosition(ec);
            sclera.setFillColor(sf::Color::White);
            window.draw(sclera);

            sf::CircleShape pupil(pupR);
            pupil.setOrigin({pupR, pupR});
            pupil.setPosition({ ec.x + lx * pupShift, ec.y + ly * pupShift });
            pupil.setFillColor(scared ? sf::Color(210, 210, 255) : sf::Color(40, 40, 200));
            window.draw(pupil);
        }
    }
};