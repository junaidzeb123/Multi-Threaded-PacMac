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
int main() {
    Texture maptect;

    int size = 30;
    sf::RectangleShape rec({(float)size, (float)size});
    rec.setFillColor({Color::Blue});
    rec.setOutlineColor({124, 252, 0});



    Sprite map;
    map.setTexture(maptect);
    map.setPosition(50, 50);
    sf::RenderWindow window(sf::VideoMode(width, height), "Game");
    window.setFramerateLimit(60);


   std:: cout<<"hello";
    sf::Clock clock;
    sf::Time elapsedTime;
    sf::Time timerDuration = sf::seconds(2);

    rec.setFillColor({Color::Blue});
    rec.setSize({size, size});

    int x = 0;
    int y = 0;


    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 13; j++) {
            if (grid[i][j]) {
                rec.setPosition(x, y);
                window.draw(rec);
                window.display();
            }
            x += size;

            while (elapsedTime <= timerDuration) {
                elapsedTime = clock.getElapsedTime();
            }
            clock.restart();
        }
        x = 0;
        y += size;
    }

    while (window.isOpen()) {
        Event event;
        window.display();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }
}