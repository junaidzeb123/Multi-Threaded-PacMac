#include "enemy.h"
#include "utils.h"
#include "walls.h"
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stack>
#include <time.h>
/*
g++ -c game.cpp&& g++ game.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system&& ./sfml-app
./sfml
*/
///////////////////////////////////////////////////Synchronization

sem_t pelletMutex;
sem_t pelletEmpty;
sem_t pelletFull;
int pelletCounter = 0;

////////////////////////////////////////////////////

#define Window_width 1000
#define Window_height 700

sf::RenderWindow window(sf::VideoMode(Window_width, Window_height), "Game");

/*  SCENARIO # 1*/
sem_t Scenario1a;     // common to both pacman and ghost
sem_t Scenario1b;     // for updating no of ghost reading maze
int GhostPresent = 0; // to keep check how many ghost are reading maza
/*  SCENARIO # 1*/

int totalCoins = 0;
bool Pacturn = true;
Ghost ghosts[4];
int coin_animation_count = 0;
int TotalScore = 0;
struct tempos
{
    int x;
    int y;
};

struct forPellet
{
    sf::CircleShape *Pellet;
    sf::Sprite *Pacman;
};

void placePellet(sf::CircleShape *Pellet)
{
    int a = rand() % 16 + 1;
    int b = rand() % 21 + 3;

    for (int i = 0; i < 8; i++)
    {
        if (Pellet[i].getPosition().x == -10 && Pellet[i].getPosition().y == -10)
        {
            while (temp[a][b] != 0)
            {
                a = rand() % 16 + 1;
                b = rand() % 21 + 4;
            }
            grid[a][b] = 2;
            temp[a][b] = 2;
            Pellet[i].setPosition(b * wallSize + 160, a * wallSize + 160);
        }
    }
}

void collionWithPellet(Sprite *Pacman, sf::CircleShape *Pellet)
{
    for (int i = 0; i < 8; i++)
    {
        if (Pacman->getGlobalBounds().intersects(Pellet[i].getGlobalBounds()))
        {
            Pellet[i].setPosition(-10, -10);
            TotalScore += 20;
        }
    }
}

void *PelletProducer(void *Pellet)
{

    sem_wait(&pelletEmpty);
    sem_wait(&pelletMutex);

    sf::CircleShape *CastPellet = (sf::CircleShape *)Pellet;
    placePellet(CastPellet);

    sem_post(&pelletFull);
    sem_post(&pelletMutex);
}

void *PelletConsumer(void *Pellet_structure)
{
    sem_wait(&pelletFull);
    sem_wait(&pelletMutex);

    forPellet *CastPellet = (forPellet *)Pellet_structure;
    collionWithPellet(CastPellet->Pacman, CastPellet->Pellet);

    sem_post(&pelletEmpty);
    sem_post(&pelletMutex);
}

struct ghostMovementData
{
    float *ghost_movement_timer;
    sf::Time deltaTime1;
    float threshold;
    Sprite *Pacman;
    int index;
    ghostMovementData(float *time, Ghost *ghosts, Time deltaTime, float threshold, Sprite *Pacman, int index)
    {
        ghost_movement_timer = time;
        this->deltaTime1 = deltaTime;
        this->threshold = threshold;
        this->Pacman = Pacman;
        this->index = index;
    }

    ghostMovementData()
    {
    }
};

void LoadPacmanSupport(Sprite &temp, Sprite &Pacman)
{
    temp.setPosition(Pacman.getPosition().x, Pacman.getPosition().y);
    Pacman = temp;
}

void loadPacman(Sprite *pSheet, Sprite &Pacman, float &timer, char direction)
{
    if (timer > 0.04)
    {
        if (direction == 'r')
        {
            if (Pacturn == true)
            {
                Pacturn = false;
                LoadPacmanSupport(pSheet[0], Pacman);
            }
            else
            {
                Pacturn = true;
                LoadPacmanSupport(pSheet[1], Pacman);
            }
        }
        else if (direction == 'l')
        {
            if (Pacturn == true)
            {
                Pacturn = false;
                LoadPacmanSupport(pSheet[2], Pacman);
            }
            else
            {
                Pacturn = true;
                LoadPacmanSupport(pSheet[3], Pacman);
            }
        }
        else if (direction == 'u')
        {
            if (Pacturn == true)
            {
                Pacturn = false;
                LoadPacmanSupport(pSheet[4], Pacman);
            }
            else
            {
                Pacturn = true;
                LoadPacmanSupport(pSheet[5], Pacman);
            }
        }
        else if (direction == 'd')
        {
            if (Pacturn == true)
            {
                Pacturn = false;
                LoadPacmanSupport(pSheet[6], Pacman);
            }
            else
            {
                Pacturn = true;
                LoadPacmanSupport(pSheet[7], Pacman);
            }
        }
        timer = 0;
    }
}

void dfsLoadCoin(sf::CircleShape *coin, int grid[][26], int x, int y, tempos *coin_ani)
{
    std::stack<std::pair<int, int>> stack;
    stack.push({x, y});

    while (!stack.empty())
    {
        auto current = stack.top();
        stack.pop();

        int i = current.first;
        int j = current.second;

        if (i < 1 || i > 15 || j < 1 || j > 24 || grid[i][j])
        {
            continue; // Out of bounds or already visited
        }

        grid[i][j] = 1; // Mark as visited

        if (!((i == 5 || i == 6 || i == 11 || i == 12) && (j == 24 || j == 25 || j == 1 || j == 2)))
        {
            coin[totalCoins].setPosition(j * wallSize + 160, i * wallSize + 160);
            coin_ani[coin_animation_count].x = j * wallSize + 160;
            coin_ani[coin_animation_count].y = i * wallSize + 160;
            coin_animation_count++;
            totalCoins++;
        }

        // Explore neighbors in DFS order (down, right, up, left)
        stack.push({i + 1, j}); // Down
        stack.push({i, j + 1}); // Right
        stack.push({i - 1, j}); // Up
        stack.push({i, j - 1}); // Left
    }
}

void loadCoinDFS(sf::CircleShape *coin, sf::RectangleShape *rec)
{
    Sprite aniPac[3];
    Texture tx[3];
    tx[0].loadFromFile("p.png");
    tx[1].loadFromFile("./img/Enemies/e11.png");
    tx[2].loadFromFile("./img/Enemies/e22.png");
    for (int i = 0; i < 3; i++)
    {
        aniPac[i].setTexture(tx[i]);
        aniPac[i].setScale(0.035, 0.035);
    }
    aniPac[2].setScale(0.05, 0.05);

    tempos *coin_animation = new tempos[236];

    for (int i = 1; i <= 15; ++i)
    {
        for (int j = 1; j <= 24; ++j)
        {
            if (!grid[i][j])
            {
                dfsLoadCoin(coin, grid, i, j, coin_animation);
            }
        }
    }

    window.clear();
    for (int i = 0; i < totalWalls; i++)
        window.draw(rec[i]);
    for (int i = 0, j = totalCoins / 3, k = 2 * (totalCoins / 3) + 1; i < 79; i++, j++, k++)
    {
        aniPac[0].setPosition(coin_animation[i].x, coin_animation[i].y);
        aniPac[1].setPosition(coin_animation[j].x, coin_animation[j].y);
        aniPac[2].setPosition(coin_animation[k].x, coin_animation[k].y);
        window.draw(aniPac[0]);
        window.draw(aniPac[1]);
        window.draw(aniPac[2]);
        window.display();
        sleep(0.2);
    }
}

bool Collion_With_Walls(sf::RectangleShape *rec, sf::Sprite &Pacman, char &direction)
{
    sf::FloatRect pacmanBounds = Pacman.getGlobalBounds();
    for (int i = 0; i < totalWalls; i++)
    {
        sf::FloatRect wallBounds = rec[i].getGlobalBounds();
        if (wallBounds.intersects(pacmanBounds))
        {
            if (direction == 'r')
            {
                Pacman.move(-5, 0);
            }
            else if (direction == 'l')
            {
                Pacman.move(5, 0);
            }
            else if (direction == 'd')
            {
                Pacman.move(0, -5);
            }
            else if (direction == 'u')
            {
                Pacman.move(0, 5);
            }
            else
                return false;
            direction = '\0';
            return true; // Collision detected
        }
    }
    return false; // No collision
}

bool Collion_With_Coins(sf::CircleShape *coin, Sprite &Pacman) {
    // as pacman acts a writer so it lock the cs as soon as he enter the cs
    sem_wait(&Scenario1a);
    for (int i = 0; i < 236; i++) {
        if (coin[i].getGlobalBounds().intersects(Pacman.getGlobalBounds())) {
            coin[i].setPosition(-100, -100);
            totalCoins--;
            sem_post(&Scenario1a);
            return true;
        }
    }
    sem_post(&Scenario1a);
    return false;
}

void movePacman(sf::Sprite &Pacman, sf::RectangleShape *rec, sf::Time deltaTime, float speed, char &direction)
{
    float dt = deltaTime.asSeconds();
    sf::Vector2f movement(0.0f, 0.0f);
    if (direction == 'l')
    {
        movement.x -= speed * dt;
    }
    if (direction == 'r')
    {
        movement.x += speed * dt;
    }
    if (direction == 'u')
    {
        movement.y -= speed * dt;
    }
    if (direction == 'd')
    {
        movement.y += speed * dt;
    }

    Pacman.move(movement);
}

void teleport(Sprite &Pacman)
{
    if (Pacman.getPosition().y < 150)
        Pacman.setPosition(Pacman.getPosition().x, 650);
    else if (Pacman.getPosition().y > 650)
        Pacman.setPosition(Pacman.getPosition().x, 150);
    else if (Pacman.getPosition().x < 150)
        Pacman.setPosition(920, Pacman.getPosition().y);
    else if (Pacman.getPosition().x > 920)
        Pacman.setPosition(150, Pacman.getPosition().y);
}

void ghostCollisionCheck(Ghost *ghosts, Sprite &Pacman)
{
    for (int i = 0; i < 4; i++)
    {
        teleport(ghosts[i].ghost);
        ghostCollisionWall(&ghosts[i]);
        // ghostCollisionOtherGhosts(&ghosts[i],ghosts);
        ghostCollisionPacman(&ghosts[i], Pacman);
    }
}

/////////////////////////////////////////Threads////////////////////////////////////////////
void *GhostThread(void *att)
{
    int choice = 0;
    ghostMovementData *data = (ghostMovementData *)att;
    if (*data->ghost_movement_timer > data->threshold) {
        selectGhostDirection(ghosts[data->index].ghost_direction);
        *data->ghost_movement_timer = 0;
    }

    /*locking hte common semaphore shared b/w pacman and ghost.
    because as soon as the ghost enter for reading the pacman must
    not be allowed to change the maze.*/
    sem_wait(&Scenario1a);

    /*incementing the value of no of ghost reading the maze.*/
    sem_wait(&Scenario1b);
    GhostPresent++;
    sem_post(&Scenario1b);

    ghostCollisionWall(&ghosts[data->index]);
    // ghostCollisionOtherGhosts(&ghosts[i],ghosts);
    ghostCollisionPacman(&ghosts[data->index], *data->Pacman);
    ghost_movement(&ghosts[data->index], data->deltaTime1);

    /*acquiring the lock of semaphore which is shared between ghosts.*/
    sem_wait(&Scenario1b);
    GhostPresent--;

    /*checking that if no ghost is in cs then allow pacman to acquire maze*/
    if (GhostPresent <= 0)
        sem_post(&Scenario1a);

    /*singling for ghosts if they want to access the GhostPresen*/
    sem_post(&Scenario1b);
}

void *UiConrolThread(void *attr)
{
    char *currentDirection = (char *)attr;
    Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            *currentDirection = 'u';
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            *currentDirection = 'd';
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            *currentDirection = 'l';
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            *currentDirection = 'r';
    }
}

void *Game_Engine(void *)
{
    Sprite Psheet[8];
    Texture Pac[8];
    Sprite Pacman;
    Font f1;
    f1.loadFromFile("arial.ttf");
    sf::Text Score;
    Score.setFont(f1);
    Score.setCharacterSize(20);
    Score.setFillColor(sf::Color::White);
    Score.setPosition(160, 30);

    for (int i = 0; i < 8; i++)
    {
        Pac[i].loadFromFile("pacman1/" + std::to_string(i + 1) + ".png");
        Psheet[i].setTexture(Pac[i]);
        Psheet[i].setScale(1.3, 1.3);
        Psheet[i].setOrigin(Psheet[i].getLocalBounds().width / 2, Psheet[i].getLocalBounds().height / 2);
    }
    Psheet[0].setPosition(195, 195);
    Pacman = Psheet[0];

    window.setFramerateLimit(60);

        Sprite lives[3];
        Texture lv;
        lv.loadFromFile("p.png");
        for (int i = 0; i < 3; i++)
        {
            lives[i].setTexture(lv);
            lives[i].setScale(0.05,0.05);
            lives[i].setPosition(800+i*40, 30);
        }
    sf::CircleShape coin[236];
    sf::CircleShape Pellet[8];


    for (int i = 0; i < 236; i++)
    {
        coin[i].setFillColor(sf::Color::Yellow);
        coin[i].setOutlineColor(sf::Color(255, 215, 0)); // RGB values for golden color
        coin[i].setRadius(3);
    }
    for (int i = 0; i < 8; i++)
    {
        Pellet[i].setFillColor(sf::Color::Blue);
        Pellet[i].setOutlineColor(sf::Color(255, 215, 0)); // RGB values for golden color
        Pellet[i].setRadius(5);
        Pellet[i].setPosition(-10, -10);
    }
    LoadMaze(rec, window);
    loadCoinDFS(coin, rec);

    forPellet obj1;
    obj1.Pellet = Pellet;
    obj1.Pacman = &Pacman;

    sf::Clock clock;
    sf::Clock clock1;

    sf::Time elapsedTime;
    float time = 0;
    float timer = 0;
    float total_timer = 0;
    float ghost_movement_timer = 0;
    int choice = 0;
    sf::Time deltaTime;
    sf::Time deltaTime1;

    float Pacman_speed = 150.0f;
    bool AnimatedDisplay = false;
    char currentDirection = '\0';

    ///////////////////////////////
    sem_init(&pelletMutex, 0, 1);
    sem_init(&pelletEmpty, 0, 8);
    sem_init(&pelletFull, 0, 0);
    pthread_t Producer;
    pthread_t Consumer;
    //////////////////////////////

    while (window.isOpen())
    {
        pthread_t UithreadId;
        pthread_t ghostsId[4];
        time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;
        ghost_movement_timer += time;
        total_timer += time;

       

        if (timer > 0.2 && AnimatedDisplay == false)
        {
            AnimatedDisplay = true;
        }
        // some wait to display the animation
        if (AnimatedDisplay)
        {

            window.display();
            pthread_create(&UithreadId, NULL, UiConrolThread, &currentDirection);

            ghostMovementData data[4];
            for (int i = 0; i < 4; i++)
            {
                data[i].deltaTime1 = deltaTime1;
                data[i].ghost_movement_timer = &ghost_movement_timer;
                data[i].Pacman = &Pacman;
                data[i].threshold = 1;
                data[i].index = i;
            }
            for (int i = 0; i < 4; i++) {
                pthread_create(&ghostsId[i], NULL, GhostThread, &data[i]);
            }
            deltaTime1 = clock1.restart();
            teleport(Pacman);

            ghostCollisionCheck(ghosts, Pacman);
            deltaTime = clock.restart();
            if (!Collion_With_Walls(rec, Pacman, currentDirection))
            {
                movePacman(Pacman, rec, deltaTime, Pacman_speed, currentDirection);
            }
            loadPacman(Psheet, Pacman, timer, currentDirection);
            Collion_With_Coins(coin, Pacman);

            pthread_create(&Producer, NULL, PelletProducer, Pellet);
            pthread_create(&Consumer, NULL, PelletConsumer, &obj1);
            Score.setString("SCORE : " + to_string(TotalScore));

                if(livesCount <= 0)
                exit(0);

            window.clear();
            for (int j = 0; j < 236; j++)
            {
                if (j < 188)
                    window.draw(rec[j]);
                window.draw(coin[j]);
            }
            for (int i = 0; i < 8; i++)
                window.draw(Pellet[i]);

            for (int i = 0; i < 4; i++)
                window.draw(ghosts[i].ghost);
            window.draw(Pacman);
            window.draw(Score);
            for(int i=0;i<livesCount;i++)
            window.draw(lives[i]);
            pthread_join(UithreadId, NULL);
            for (int i = 0; i < 4; i++)
                pthread_join(ghostsId[i], NULL);

            pthread_join(Producer, NULL);
            pthread_join(Consumer, NULL);
        }
    }
}

int main()
{
    srand(time(0));
    pthread_t id;

    window.setActive(false);
    sem_init(&Scenario1a, 0, 1);
    sem_init(&Scenario1b, 0, 1);
    pthread_create(&id, NULL, Game_Engine, NULL);

    pthread_join(id, NULL);
    return 0;
}
