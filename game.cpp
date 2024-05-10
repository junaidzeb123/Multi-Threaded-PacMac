#include "enemy.h"
#include "utils.h"
#include "walls.h"
#include <stack>
#include <time.h>
/*
g++ -c game.cpp&& g++ game.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system&& ./sfml-app
./sfml
*/
///////////////////////////////////////////////////Synchronization

////////////////////////////////////////////////////

#define Window_width 1080
#define Window_height 700

int totalCoins = 0;
bool Pacturn = true;
sf::RenderWindow window(sf::VideoMode(Window_width, Window_height), "Game");

void LoadPacmanSupport(Sprite &temp, Sprite &Pacman) {
    temp.setPosition(Pacman.getPosition().x, Pacman.getPosition().y);
    Pacman = temp;
}
void loadPacman(Sprite *pSheet, Sprite &Pacman, float &timer, char direction) {
    if (timer > 0.02) {
        if (direction == 'r') {
            if (Pacturn == true) {
                Pacturn = false;
                LoadPacmanSupport(pSheet[0], Pacman);
            } else {
                Pacturn = true;
                LoadPacmanSupport(pSheet[1], Pacman);
            }
        } else if (direction == 'l') {
            if (Pacturn == true) {
                Pacturn = false;
                LoadPacmanSupport(pSheet[2], Pacman);
            } else {
                Pacturn = true;
                LoadPacmanSupport(pSheet[3], Pacman);
            }
        } else if (direction == 'u') {
            if (Pacturn == true) {
                Pacturn = false;
                LoadPacmanSupport(pSheet[4], Pacman);
            } else {
                Pacturn = true;
                LoadPacmanSupport(pSheet[5], Pacman);
            }
        } else if (direction == 'd') {
            if (Pacturn == true) {
                Pacturn = false;
                LoadPacmanSupport(pSheet[6], Pacman);
            } else {
                Pacturn = true;
                LoadPacmanSupport(pSheet[7], Pacman);
            }
        }
        timer = 0;
    }
}
struct tempos {
    int x;
    int y;
};
int coin_animation_count = 0;
void dfsLoadCoin(sf::CircleShape *coin, int grid[][26], int x, int y, tempos *coin_ani) {
    std::stack<std::pair<int, int>> stack;
    stack.push({x, y});

    while (!stack.empty()) {
        auto current = stack.top();
        stack.pop();

        int i = current.first;
        int j = current.second;

        if (i < 1 || i > 15 || j < 1 || j > 24 || grid[i][j]) {
            continue; // Out of bounds or already visited
        }

        grid[i][j] = 1; // Mark as visited

        if (!((i == 5 || i == 6 || i == 11 || i == 12) && (j == 24 || j == 25 || j == 1 || j == 2))) {
            coin[totalCoins].setPosition(j * wallSize + 160, i * wallSize + 160);
            coin_ani[coin_animation_count].x = j * wallSize + 160;
            coin_ani[coin_animation_count].y = i * wallSize + 160;
            coin_animation_count++;
            // window.clear();
            // for(int i=0;i<totalWalls;i++)
            // window.draw(rec[i]);
            // window.draw(producer);
            // for(int i=0;i<totalCoins;i++)
            // window.draw(coin[i]);
            // window.display();
            totalCoins++;
        }

        // Explore neighbors in DFS order (down, right, up, left)
        stack.push({i + 1, j}); // Down
        stack.push({i, j + 1}); // Right
        stack.push({i - 1, j}); // Up
        stack.push({i, j - 1}); // Left
    }
}

void loadCoinDFS(sf::CircleShape *coin, sf::RectangleShape *rec) {
    Sprite producer[3];
    Texture tx[3];
    tx[0].loadFromFile("p.png");
    tx[1].loadFromFile("e11.png");
    tx[2].loadFromFile("e22.png");
    for (int i = 0; i < 3; i++) {
        producer[i].setTexture(tx[i]);
        producer[i].setScale(0.035, 0.035);
    }
    producer[2].setScale(0.05, 0.05);

    tempos *coin_animation = new tempos[236];

    for (int i = 1; i <= 15; ++i) {
        for (int j = 1; j <= 24; ++j) {
            if (!grid[i][j]) {
                dfsLoadCoin(coin, grid, i, j, coin_animation);
            }
        }
    }

    window.clear();
    for (int i = 0; i < totalWalls; i++)
        window.draw(rec[i]);
    for (int i = 0, j = totalCoins / 3, k = 2 * (totalCoins / 3) + 1; i < 79; i++, j++, k++) {
        producer[0].setPosition(coin_animation[i].x, coin_animation[i].y);
        producer[1].setPosition(coin_animation[j].x, coin_animation[j].y);
        producer[2].setPosition(coin_animation[k].x, coin_animation[k].y);
        window.draw(producer[0]);
        window.draw(producer[1]);
        window.draw(producer[2]);
        window.display();
        sleep(0.2);
    }
}

bool Collion_With_Walls(sf::RectangleShape *rec, sf::Sprite &Pacman, char &direction) {
    sf::FloatRect pacmanBounds = Pacman.getGlobalBounds();
    for (int i = 0; i < totalWalls; i++) {
        sf::FloatRect wallBounds = rec[i].getGlobalBounds();
        if (wallBounds.intersects(pacmanBounds)) {
            if (direction == 'r') {
                Pacman.move(-5, 0);
            } else if (direction == 'l') {
                Pacman.move(5, 0);
            } else if (direction == 'd') {
                Pacman.move(0, -5);
            } else if (direction == 'u') {
                Pacman.move(0, 5);
            } else
                return false;
            direction = '\0';
            return true; // Collision detected
        }
    }
    return false; // No collision
}

bool Collion_With_Coins(sf::CircleShape *coin, Sprite &Pacman) {
    for (int i = 0; i < 236; i++) {
        if (coin[i].getGlobalBounds().intersects(Pacman.getGlobalBounds())) {
            coin[i].setPosition(-100, -100);
            totalCoins--;
            return true;
        }
    }
    return false;
}
void movePacman(sf::Sprite &Pacman, sf::RectangleShape *rec, sf::Time deltaTime, float speed, char &direction) {
    float dt = deltaTime.asSeconds();
    sf::Vector2f movement(0.0f, 0.0f);
    if (direction == 'l') {
        movement.x -= speed * dt;
    }
    if (direction == 'r') {
        movement.x += speed * dt;
    }
    if (direction == 'u') {
        movement.y -= speed * dt;
    }
    if (direction == 'd') {
        movement.y += speed * dt;
    }

    Pacman.move(movement);
}
void teleport(Sprite &Pacman) {
    if (Pacman.getPosition().y < 150)
        Pacman.setPosition(Pacman.getPosition().x, 650);
    else if (Pacman.getPosition().y > 650)
        Pacman.setPosition(Pacman.getPosition().x, 150);
    else if (Pacman.getPosition().x < 150)
        Pacman.setPosition(920, Pacman.getPosition().y);
    else if (Pacman.getPosition().x > 920)
        Pacman.setPosition(150, Pacman.getPosition().y);
}
void ghost_random_movement_support(float &ghost_movement_timer, Ghost *ghosts, sf::Time deltaTime1, float threshold, Sprite &Pacman) {
    int choice = 0;
    if (ghost_movement_timer > threshold) {
        choice = selectRandom(4);
        cout << choice << endl;
        selectGhostDirection(ghosts[choice].ghost_direction);
        ghost_movement_timer = 0;
    }
    for (int i = 0; i < 4; i++) {
        ghostCollisionWall(&ghosts[i]);
        // ghostCollisionOtherGhosts(&ghosts[i],ghosts);
        ghostCollisionPacman(&ghosts[i], Pacman);
        ghost_movement(&ghosts[i], deltaTime1);
    }
}
/////////////////////////////////////////Threads////////////////////////////////////////////

void Game_Engine() {

    Sprite Psheet[8];
    Texture Pac[8];
    Sprite Pacman;
    Ghost ghosts[4];
    for (int i = 0; i < 8; i++) {
        Pac[i].loadFromFile("pacman1/" + std::to_string(i + 1) + ".png");
        Psheet[i].setTexture(Pac[i]);
        Psheet[i].setScale(1.3, 1.3);
        Psheet[i].setOrigin(Psheet[i].getLocalBounds().width / 2, Psheet[i].getLocalBounds().height / 2);
    }
    Psheet[0].setPosition(195, 195);
    Pacman = Psheet[0];

    window.setFramerateLimit(60);

    sf::CircleShape coin[236];

    for (int i = 0; i < 236; i++) {
        coin[i].setFillColor(sf::Color::Yellow);
        coin[i].setOutlineColor(sf::Color(255, 215, 0)); // RGB values for golden color
        coin[i].setRadius(3);
    }
    LoadMaze(rec, window);
    loadCoinDFS(coin, rec);

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
    while (window.isOpen()) {
        time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;
        ghost_movement_timer += time;
        total_timer += time;
        if (timer > 0.2 && AnimatedDisplay == false) {
            AnimatedDisplay = true;
        }
        if (AnimatedDisplay) // some wait to display the animation
        {
            Event event;
            window.display();
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                    window.close();
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                    currentDirection = 'u';
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                    currentDirection = 'd';
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                    currentDirection = 'l';
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                    currentDirection = 'r';
            }

            teleport(Pacman);
            for (int i = 0; i < 4; i++)
                teleport(ghosts[i].ghost);

            deltaTime1 = clock1.restart();
            if (total_timer > 2) {
                ghost_random_movement_support(ghost_movement_timer, ghosts, deltaTime1, 0.2, Pacman);
            } else {
                ghost_random_movement_support(ghost_movement_timer, ghosts, deltaTime1, 0.08, Pacman);
            }

            deltaTime = clock.restart();
            if (!Collion_With_Walls(rec, Pacman, currentDirection)) {
                movePacman(Pacman, rec, deltaTime, Pacman_speed, currentDirection);
            }
            loadPacman(Psheet, Pacman, timer, currentDirection);
            Collion_With_Coins(coin, Pacman);

            // Draw the maze and Pacman sprite
            window.clear();
            for (int j = 0; j < 236; j++) {
                if (j < 188)
                    window.draw(rec[j]);
                window.draw(coin[j]);
            }
            for (int i = 0; i < 4; i++)
                window.draw(ghosts[i].ghost);
            window.draw(Pacman);
        }
    }
    cout << totalCoins << endl;
}

int main() {
    srand(time(0));
    Game_Engine();

    return 0;
}
