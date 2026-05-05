#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(ms) usleep((ms) * 1000)
#endif

using namespace std;

//declaring the constants for the game

const int mapRows = 20; // this is for the grid for the map
const int mapCols = 30;
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
#ifdef _WIN32
    SetConsoleOutputCP(65001); // Enable UTF-8 output in Windows CMD
#endif
    seedRandom();
    initialiseGame();
    gameLoop();
    showGameOverMessage();
    return 0;
}

// Function definitions
void seedRandom() { // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));
}
void initialiseGame() { // Initialise the game state
    createEmptyMap();
    generateRandomCity();
    placePlayer();
    placeZombies();
	// Initialise player stats
	player.health = 100;       
    player.food = 1;
    player.ammunition = 1;
    player.score = 0;
	// Initialise game state variables
    remainingTurns = maxTurns;
    gameOver = false;
    playerWon = false;
    playerReachedSafeZone = false;
}
void createEmptyMap() { // Create an empty map with borders
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
void generateRandomCity()   // Generate random city layout with barriers and resources
{
    for (int r = 1; r < mapRows - 1; ++r) {
        for (int c = 1; c < mapCols - 1; ++c) {
            int randNum = rand() % 100;
            if (randNum < 15) {
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
void placePlayer() {    // Place the player at a random empty location
    int row, col;
    do {
        row = 1 + rand() % (mapRows - 2);
        col = 1 + rand() % (mapCols - 2);
    } while (grid[row][col] != tileEmpty);
    player.row = row;
    player.col = col;
    grid[row][col] = tilePlayer;
}
void placeZombies() {       // Place zombies at random empty locations
    zombies.clear();
    int placedZombies = 0;
    while (placedZombies < maxZombies) {
        int row = 1 + rand() % (mapRows - 2);
        int col = 1 + rand() % (mapCols - 2);
		if (grid[row][col] == tileEmpty) {      // only place on empty tiles
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
        if (gameOver) break;

        moveZombies();
        checkZombieEncounters();
        if (gameOver) break;

        checkSafeZone();
        if (gameOver) break;

        // Zombie avoidance / survival score (only awarded if still alive and not already won)
        player.score += 1;

        --remainingTurns;
        if (remainingTurns <= 0) {
            gameOver = true;
        }
    }
}
void drawMap() {    // Draw the current state of the map
    for (int r = 0; r < mapRows; ++r) {
        for (int c = 0; c < mapCols; ++c) {
            cout << grid[r][c] << ' ';
        }
        cout << endl;
    }
}
void showStatus() {     // Display player status
    cout << "Get the player (@) to the Safe Zone (S)! | Avoid Zombies (Z)!" << endl;
    cout << "-------------------------------------------------------------" << endl;
    cout << "(+) Health: " << player.health << " | (*) Food: " << player.food
        << " | (!) Ammo: " << player.ammunition
		<< " | Score: " << player.score
        << " | Turns Left: " << remainingTurns << endl;
    cout << "-------------------------------------------------------------" << endl;
}
char getPlayerMove() {      // Get player's move input
    char move;
    cout << "Enter your move (W/A/S/D): ";
    cin >> move;
    return move;
}
void moveZombies() {        // Move each zombie randomly
    for (auto& zombie : zombies) {
		if (!zombie.alive) continue;    // skip dead zombies
        int direction = rand() % 4;
        int newRow = zombie.row;
        int newCol = zombie.col;
        switch (direction) {
        case 0: newRow--; break; // up
        case 1: newRow++; break; // down
        case 2: newCol--; break; // left
        case 3: newCol++; break; // right
        }
		if (newRow >= 0 && newRow < mapRows && newCol >= 0 && newCol < mapCols &&       // Check bounds
			(grid[newRow][newCol] == tileEmpty || grid[newRow][newCol] == tilePlayer)) {    // Can move onto empty or player tile

            // clear old position
            grid[zombie.row][zombie.col] = tileEmpty;

            zombie.row = newRow;
            zombie.col = newCol;

            // only draw Z if it didn't move onto player (player stays visible)
            if (grid[newRow][newCol] != tilePlayer) {
                grid[newRow][newCol] = tileZombie;
            }
        }
    }
}
void movePlayer(char direction) {
    int newRow = player.row;
    int newCol = player.col;

    switch (direction) {
    case 'W': case 'w': newRow--; break;
    case 'S': case 's': newRow++; break;
    case 'A': case 'a': newCol--; break;
    case 'D': case 'd': newCol++; break;
    default:
        return; // invalid input
    }

    // Bounds check
    if (newRow < 0 || newRow >= mapRows || newCol < 0 || newCol >= mapCols) {
        return;
    }

    char destinationTile = grid[newRow][newCol];

    // Block barriers
    if (destinationTile == tileBarrier) {
        return;
    }

    // Zombie encounter (handled immediately)
    if (destinationTile == tileZombie) {
        if (player.ammunition > 0) {
            player.ammunition--;
            player.score += 25;

            cout << "You used 1 ammo and defeated a zombie! (+25 score)" << endl;
            Sleep(700);

            // Mark zombie as dead
            for (auto& z : zombies) {
                if (z.alive && z.row == newRow && z.col == newCol) {
                    z.alive = false;
                    break;
                }
            }

            // Move player into the tile
            grid[player.row][player.col] = tileEmpty;
            player.row = newRow;
            player.col = newCol;
            grid[player.row][player.col] = tilePlayer;
        }
        else {
            player.health -= 60;
            if (player.score >= 10) player.score -= 10;

            cout << "A zombie bites you! (-60 health)" << endl;
            Sleep(700);

            if (player.health <= 0) {
                gameOver = true;
            }
        }
        return; 
    }

    // Handle resource pickups / safe zone
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
        player.ammunition++;
        player.score += 8;
        break;

    case tileSafeZone:
        player.score += 50;
        break;

    default:
        break;
    }
    // Move player normally
    grid[player.row][player.col] = tileEmpty;
    player.row = newRow;
    player.col = newCol;
    grid[player.row][player.col] = tilePlayer;
}
void checkZombieEncounters() {      // Check for encounters with zombies
    for (auto& zombie : zombies) {
        if (!zombie.alive) continue;
                // Only trigger an encounter if the zombie is on the player
        if (zombie.row == player.row && zombie.col == player.col) {
            if (player.ammunition > 0) {
                player.ammunition--;
                zombie.alive = false;
                player.score += 25;
                // Remove zombie from the map; player stays visible
                grid[player.row][player.col] = tilePlayer;
            }
            else {
                player.health -= 60;
                cout << "A zombie attacks you! (-60 health)" << endl;
				Sleep(700); // Pause to show the message
                if (player.score >= 10) player.score -= 10;

                if (player.health <= 0) {
                    gameOver = true;
                }
                grid[player.row][player.col] = tilePlayer;
            }
            return;
        }
    }
}
void checkSafeZone() {  // Check if player has reached the safe zone
    if (player.row == safeZoneRow && player.col == safeZoneCol) {
        playerReachedSafeZone = true;
        playerWon = true;
        gameOver = true;

        // Bonus for escaping quickly
        player.score += remainingTurns * 5;
    }
}
void showGameOverMessage() {        // Display game over message
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
void clearScreen() {        // Clear the console screen
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
