#include "utils.h"
#include "walls.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <ncurses.h>
#include <semaphore.h>
#include <stack>
#include <string.h>
#include <string>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
using namespace std;
using namespace sf;

int livesCount = 3;
int no_of_ghosts = 0;
bool isoutofHouse[4] = {false, false, false, false};
sem_t keys[2];
sem_t permits[2];
sem_t semaphoreTimerHelp;
sem_t Scenario3Helper;
bool blueGhost = 0;
int TotalScore = 0;

struct Ghost
{
    Texture ghost_tex;
    Sprite ghost;
    float ghost_speed;
    char ghost_direction;

    Ghost()
    {
        ghost_tex.loadFromFile("./img/Enemies/E" + std::to_string(no_of_ghosts % 4 + 1) + ".png");
        ghost.setTexture(ghost_tex);
        ghost.setScale(0.09, 0.09);
        ghost.setPosition(470 + no_of_ghosts * 45, 420);
        ghost.setOrigin(ghost.getLocalBounds().width / 2, ghost.getLocalBounds().height / 2);

        ghost_speed = 30.0;
        ghost_direction = 'l';
        no_of_ghosts++;
    }
};

Ghost ghosts[4];

int selectRandom(int dividend)
{
    return rand() % dividend + 1;
}

void selectGhostDirection(char &direction, int index)
{
     int ghostsXPostion = ghosts[index].ghost.getPosition().x;
    int ghostsYPostion = ghosts[index].ghost.getPosition().y;
    if (ghostsXPostion > 511 && ghostsXPostion < 574 && ghostsYPostion >= 357 && ghostsYPostion <= 534 )
    {
        direction = 'l';
        return;
    }
    int choice;
    if (isoutofHouse[index]) /*if ghost is out of ghost house then ghosts are allowed to move in 4 direction.
                               but if in house only 3 directions.*/
        choice = selectRandom(4);
    else
        choice = selectRandom(3);

   

    /*is allowing ghost to go inside the house*/

    switch (choice)
    {
    case 1:
        direction = 'd';
        break;

    case 2:
        direction = 'l';
        break;

    case 3:
        direction = 'r';
        break;

    case 4:
        direction = 'u';
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

void ghostCollisionWall(Ghost *g, int index)
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
            char prevdir = g->ghost_direction;
            while (g->ghost_direction == prevdir)
                selectGhostDirection(g->ghost_direction, index);
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
                g->ghost_direction = '\0';
        }
    }
}

void ghostCollisionPacman(Ghost *g, Sprite &Pacman)
{
        if (g->ghost.getGlobalBounds().intersects(Pacman.getGlobalBounds()))
        {
            if(!blueGhost)
            {Pacman.setPosition(195, 195);
            livesCount--;}
            else
            {
                g->ghost.setPosition(500,320);
                TotalScore += 100;
            }
        }
  
}

void MovingGhostoutofHouse(Ghost *q)
{
}