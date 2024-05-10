#include "utils.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <ncurses.h>
#include <string.h>
#include <string>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
using namespace sf;
/*
g++ -c game.cpp&& g++ game.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system&& ./sfml-app
./sfml
*/
#define width 800
#define height 700
int size = 30;

void drawBorder(RenderWindow &window) {
    sf::RectangleShape rec({(float)800, (float)5});
    rec.setFillColor({Color::Blue});
    rec.setOutlineColor({124, 252, 0});

    window.draw(rec);
    rec.setPosition({0,size*19+10}); //(x,y)
    window.draw(rec);
  

    rec.setSize({5,700});
    rec.setPosition({0,0}); //(x,y)
    window.draw(rec);

    rec.setPosition({size*15+10,0}); //(x,y)
    window.draw(rec);

    window.display();
}

int main() {
    Texture maptect;

    sf::RectangleShape rec({size, size});
    rec.setFillColor({Color::Blue});
    rec.setOutlineColor({124, 252, 0});
    rec.setOutlineColor({124, 252, 0});

    sf::RenderWindow window(sf::VideoMode(width, height), "Game");
    window.setFramerateLimit(60);

    sf::Clock clock;
    sf::Time elapsedTime;
    sf::Time timerDuration = sf::seconds(1);

    rec.setFillColor({Color::Blue});
    rec.setSize({size, size});

    drawBorder(window);


    int x = 20;
    int y = 20;

    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 17; j++) {
            if (grid[i][j]) {
                rec.setPosition(x, y);
               window.draw(rec);
                window.display();
            }
            x += size;
        //     while (elapsedTime <= timerDuration) {
        //         elapsedTime = clock.getElapsedTime();
        //     }
        //    clock.restart();
        }
        x = 20;
        y += size;
    }

    window.display();
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }
}