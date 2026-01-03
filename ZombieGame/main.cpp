
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <windows.h>

using namespace std;

//declaring the constants for the game

const int mapRows = 20; // this is for the grid for the map
const int mapCols = 20;
const int maxTurns = 50; // maximum number of turns
const int maxZombies = 5; // maximum number of zombies
const int minResources = 1; // minimum resources
const char tileEmpty = '.';        // empty tile representation
const char tilePlayer = '@';       // player representation
const char tileZombie = 'Z';       // zombie representation
const char tileBarrier = '#';      // barrier representation
const char tileFood = '*';         // food representation
const char tileHealth = '+';       // health representation
const char tileAmmunition = '!';   // ammo representation
const char tileSafeZone = 'S';     // safe zone representation

// Structure to represent a position on the map

struct Player {
    int row;
    int col;
    int health;
    int food;
    int ammunition;
    int score;
};

struct Zombie {
    int row;
    int col;
    bool alive;
};

//stuff for the global state of the game

vector<vector<char>> grid(mapRows, vector<char>(mapCols, tileEmpty));
vector<Zombie> zombies;
Player player;

int remainingTurns = maxTurns;
bool gameOver = false;
bool playerWon = false;
bool playerReachedSafeZone = false;
int safeZoneRow = -1;
int safeZoneCol = -1; // Coordinates of the safe zone, to be set during city generation

// Function prototypes

void seedRandom();
void initialiseGame();
void createEmptyMap();
void generateRandomCity();
void placePlayer();
void placeZombies();
void gameLoop();
void drawMap();
bool isZombieAt(int row, int col);
void showStatus();
char getPlayerMove();
void movePlayer(char direction);
void moveZombies();
void checkZombieEncounters();
void checkSafeZone();
void showGameOverMessage();
void clearScreen();

// Main function

int main() {
    SetConsoleOutputCP(65001); // Enable UTF-8 output in Windows CMD
    seedRandom();
    initialiseGame();
    gameLoop();
    showGameOverMessage();
    return 0;
}

// Function definitions

void seedRandom() {
    srand(static_cast<unsigned int>(time(0)));
}

void initialiseGame() {
    createEmptyMap();
    generateRandomCity();
    placePlayer();
    placeZombies();

    player.health = 100;
    player.food = 1;
    player.ammunition = 1;
    player.score = 0;

    remainingTurns = maxTurns;
    gameOver = false;
    playerWon = false;
    playerReachedSafeZone = false;
}

void createEmptyMap() {
    for (int r = 0; r < mapRows; ++r) {
        for (int c = 0; c < mapCols; ++c) {
            if (r == 0 || r == mapRows - 1 || c == 0 || c == mapCols - 1) {
                grid[r][c] = tileBarrier; // Border walls
            }
            else {
                grid[r][c] = tileEmpty;
            }
        }
    }
}

void generateRandomCity() {
    for (int r = 1; r < mapRows - 1; ++r) {
        for (int c = 1; c < mapCols - 1; ++c) {
            int randNum = rand() % 100;
            if (randNum < 20) {
                grid[r][c] = tileBarrier;
            }
            else if (randNum < 25) {
                grid[r][c] = tileFood;
            }
            else if (randNum < 30) {
                grid[r][c] = tileHealth;
            }
            else if (randNum < 35) {
                grid[r][c] = tileAmmunition;
            }
        }
    }

    // Place safe zone
    do {
        safeZoneRow = 1 + rand() % (mapRows - 2);
        safeZoneCol = 1 + rand() % (mapCols - 2);
    } while (grid[safeZoneRow][safeZoneCol] != tileEmpty);
    grid[safeZoneRow][safeZoneCol] = tileSafeZone;

    // Ensure minimum resources
    int resourcesPlaced = 0;
    while (resourcesPlaced < minResources) {
        int row = 1 + rand() % (mapRows - 2);
        int col = 1 + rand() % (mapCols - 2);
        if (grid[row][col] == tileEmpty) {
            grid[row][col] = tileFood;
            ++resourcesPlaced;
        }
    }
}

void placePlayer() {
    int row, col;
    do {
        row = 1 + rand() % (mapRows - 2);
        col = 1 + rand() % (mapCols - 2);
    } while (grid[row][col] != tileEmpty);
    player.row = row;
    player.col = col;
    grid[row][col] = tilePlayer;
}

void placeZombies() {
    zombies.clear();
    int placedZombies = 0;
    while (placedZombies < maxZombies) {
        int row = 1 + rand() % (mapRows - 2);
        int col = 1 + rand() % (mapCols - 2);
        if (grid[row][col] == tileEmpty) {
            Zombie zombie = { row, col, true };
            zombies.push_back(zombie);
            grid[row][col] = tileZombie;
            ++placedZombies;
        }
    }
}

void gameLoop() {
    while (!gameOver) {
        clearScreen();
        drawMap();
        showStatus();
        char move = getPlayerMove();
        movePlayer(move);
        moveZombies();
        checkZombieEncounters();
        checkSafeZone();
        --remainingTurns;
        if (remainingTurns <= 0) {
            gameOver = true;
        }
    }
}

void drawMap() {
    for (int r = 0; r < mapRows; ++r) {
        for (int c = 0; c < mapCols; ++c) {
            cout << grid[r][c] << ' ';
        }
        cout << endl;
    }
}

bool isZombieAt(int row, int col) {
    for (const auto& zombie : zombies) {
        if (zombie.alive && zombie.row == row && zombie.col == col) {
            return true;
        }
    }
    return false;
}

void showStatus() {
    cout << "Get the player (@) to the Safe Zone (S)! | Avoid Zombies (Z)!" << endl;
    cout << "-------------------------------------------------------------" << endl;
    cout << "(+) Health: " << player.health << " | (*) Food: " << player.food
        << " | (!) Ammo: " << player.ammunition
		<< " | Score: " << player.score
        << " | Turns Left: " << remainingTurns << endl;
    cout << "-------------------------------------------------------------" << endl;
}

char getPlayerMove() {
    char move;
    cout << "Enter your move (W/A/S/D): ";
    cin >> move;
    return toupper(move);
}

void moveZombies() {
    for (auto& zombie : zombies) {
        if (!zombie.alive) continue;
        int direction = rand() % 4;
        int newRow = zombie.row;
        int newCol = zombie.col;
        switch (direction) {
        case 0: newRow--; break; // up
        case 1: newRow++; break; // down
        case 2: newCol--; break; // left
        case 3: newCol++; break; // right
        }
        if (newRow >= 0 && newRow < mapRows && newCol >= 0 && newCol < mapCols &&
            grid[newRow][newCol] == tileEmpty) {
            grid[zombie.row][zombie.col] = tileEmpty;
            zombie.row = newRow;
            zombie.col = newCol;
            grid[zombie.row][zombie.col] = tileZombie;
        }
    }
}


void movePlayer(char direction) {
    int newRow = player.row;
    int newCol = player.col;

    switch (direction) {
    case 'W':
    case 'w':
        newRow--;
        break;

    case 'S':
    case 's':
        newRow++;
        break;

    case 'A':
    case 'a':
        newCol--;
        break;

    case 'D':
    case 'd':
        newCol++;
        break;

    default:
        return; // invalid move
    }

    // Check bounds + barriers
    if (newRow >= 0 && newRow < mapRows &&
        newCol >= 0 && newCol < mapCols &&
        grid[newRow][newCol] != tileBarrier) {
        char destinationTile = grid[newRow][newCol];

        switch (destinationTile) {
        case tileFood:
            player.food++;
            player.score += 10;
            break;

        case tileHealth:
            player.health += 20;
            if (player.health > 100) player.health = 100;
            player.score += 15;
            break;

        case tileAmmunition:
            player.ammunition += 1;
            player.score += 8;
            break;

        case tileSafeZone:
            // optional small reward here (main bonus happens in checkSafeZone)
            player.score += 50;
            break;

        default:
            break;
        }

        // Move player on the grid
        grid[player.row][player.col] = tileEmpty;
        player.row = newRow;
        player.col = newCol;
        grid[player.row][player.col] = tilePlayer;
    }
}
void checkZombieEncounters() {
    for (auto& zombie : zombies) {
        if (player.ammunition > 0) {
            player.ammunition--;
            zombie.alive = false;
            player.score += 25;                 // points for killing a zombie
            grid[zombie.row][zombie.col] = tilePlayer;
        }
            else {
            player.health -= 60;
            player.score -= 10;                 // small penalty for being hit (optional)
                if (player.score < 0) player.score = 0;

                if (player.health <= 0) {
                gameOver = true;
            }
                else {
                grid[player.row][player.col] = tilePlayer;
                }
            }
        }
    }

void checkSafeZone() {
    if (player.row == safeZoneRow && player.col == safeZoneCol) {
        playerReachedSafeZone = true;
        playerWon = true;
        gameOver = true;

        // Bonus for escaping quickly
        player.score += remainingTurns * 5;
    }
}

void showGameOverMessage() {
    clearScreen();
    drawMap();
    if (playerWon) {
        cout << "Congratulations! You reached the safe zone and survived!" << endl;
    }
    else if (player.health <= 0) {
        cout << "Game Over! You were killed by zombies." << endl;
    }
    else {
        cout << "Game Over! You ran out of time." << endl;
    }

	cout << "Final Score: " << player.score << endl;  //added final score display based on new system
    }


void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
