#include "utils.h"
#include "walls.h"
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
#include <time.h>
using namespace std;
using namespace sf;

int no_of_ghosts = 0;
struct Ghost
{
    Texture ghost_tex;
    Sprite ghost;
    float ghost_speed;
    char ghost_direction;

    Ghost()
    {
        ghost_tex.loadFromFile("E" + std::to_string(no_of_ghosts % 5 + 1) + ".png");
        ghost.setTexture(ghost_tex);
        ghost.setScale(0.09, 0.09);
        ghost.setPosition(470 + no_of_ghosts * 45, 420);
                ghost.setOrigin(ghost.getLocalBounds().width / 2, ghost.getLocalBounds().height / 2); 

        ghost_speed = 30.0;
        ghost_direction = 'u';
        no_of_ghosts++;
    }
};
int selectRandom(int dividend)
{
    return rand() % dividend + 1;
}
void selectGhostDirection(char &direction)
{
    int choice = selectRandom(4);
    switch (choice)
    {
    case 1:
        direction = 'u';
        break;

    case 2:
        direction = 'd';
        break;

    case 3:
        direction = 'l';
        break;

    case 4:
        direction = 'r';
        break;

    default:
        break;
    }
}
void ghost_movement(Ghost *g, sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds();
    sf::Vector2f movement(0.0f, 0.0f);
    if (g->ghost_direction == 'l')
    {
        movement.x -= g->ghost_speed * dt;
    }
    else if (g->ghost_direction == 'r')
    {
        movement.x += g->ghost_speed * dt;
    }
    else if (g->ghost_direction == 'u')
    {
        movement.y -= g->ghost_speed * dt;
    }
    else if (g->ghost_direction == 'd')
    {
        movement.y += g->ghost_speed * dt;
    }
    g->ghost.move(movement);
}
void ghostCollisionWall(Ghost *g)
{
    for (int i = 0; i < totalWalls; i++)
    {
        if (g->ghost.getGlobalBounds().intersects(rec[i].getGlobalBounds()))
        {
            if (g->ghost_direction == 'r')
            {
                g->ghost.move(-5, 0);
            }
            else if (g->ghost_direction == 'l')
            {
                g->ghost.move(5, 0);
            }
            else if (g->ghost_direction == 'd')
            {
                g->ghost.move(0, -5);
            }
            else if (g->ghost_direction == 'u')
            {
                g->ghost.move(0, 5);
            }
            char prevdir=g->ghost_direction;
            while(g->ghost_direction == prevdir)
            selectGhostDirection(g->ghost_direction);
        }
    }
}
void ghostCollisionOtherGhosts(Ghost *g, Ghost *others)
{
    for (int i = 0; i < 4; i++)
    {
        if (g->ghost.getTexture() == others[i].ghost.getTexture())
            continue;
        if (g->ghost.getGlobalBounds().intersects(others[i].ghost.getGlobalBounds()))
        {
            if (g->ghost_direction == 'r')
            {
                g->ghost_direction = 'l';
            }
            else if (g->ghost_direction == 'l')
            {
                g->ghost_direction = 'r';
            }
            else if (g->ghost_direction == 'd')
            {
                g->ghost_direction = 'u';
            }
            else if (g->ghost_direction == 'd')
            {
                g->ghost_direction = 'd';
            }
            else
               g->ghost_direction ='\0';
        }
    }
}
void ghostCollisionPacman(Ghost *g, Sprite &Pacman)
{
    if (g->ghost.getGlobalBounds().intersects(Pacman.getGlobalBounds()))
    {
        Pacman.setPosition(195, 195);
    }
}
