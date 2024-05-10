#pragma once
#include<iostream>
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
#include <stack>
using namespace std;
using namespace sf;

#define wallSize 30
#define totalWalls 188
    sf::RectangleShape rec[totalWalls];

void LoadMaze(sf::RectangleShape *rec,sf::RenderWindow &window)
{
    int count = 0;

    for (int i = 0; i < 17; i++)
    {
        for (int j = 0; j < 26; j++)
        {
            if (grid[i][j])
            {
                rec[count].setFillColor(sf::Color::Green);
                rec[count].setOutlineColor(sf::Color(255, 215, 0)); // RGB values for golden color
                rec[count].setSize({wallSize, wallSize});
                rec[count].setPosition(j * wallSize + 150, i * wallSize + 150);
//                rec[count].setOrigin(rec[count].getLocalBounds().width / 2, rec[count].getLocalBounds().height / 2);
                window.draw(rec[count]);
                window.display();
                count++;
            }
        }
    }
}
