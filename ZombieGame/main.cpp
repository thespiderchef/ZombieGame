#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

//declaring the constants for the game

const int mapRows = 10; // this is for the grid for the map
const int mapCols = 10;
const int maxTurns = 50; // maximum number of turns
const int maxZombies = 5; // maximum number of zombies
const int minResources = 1; // minimum resources
const char tileEmpty = '.'; // empty tile representation
const char tilePlayer = 'P'; // player representation
const char tileZombie = 'Z'; // zombie representation
const char tileBarrier = '#'; // barrier representation
const char tileFood = 'F'; // food representation
const char tileHealth = 'H'; // medicine representation
const char tileAmmunition = 'A'; // ammunition representation

// Structure to represent a position on the map

struct Player {
	int row;
	int col;
	int health;
	int food;
	int ammunition;
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
int safeZoneCol = -1;		// Coordinates of the safe zone, to be set during city generation


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
void resourceCollection();
void checkZombieEncounters();
void checkSafeZone();
void showGameOverMessage();
void clearScreen();

// Main function

int main() {
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
	
	remainingTurns = maxTurns;
	gameOver = false;
	playerWon = false;
	playerReachedSafeZone = false;
}

void createEmptyMap() {
	for (int r = 0; r < mapRows; ++r) {
		for (int c = 0; c < mapCols; ++c) {
			grid[r][c] = tileEmpty;
		}
	}
}

void generateRandomCity() {
// Randomly place barriers, food, health kits, and ammunition on the map
for (int r = 0; r < mapRows; ++r) {
	for (int c = 0; c < mapCols; ++c) {
		int randNum = rand() % 100;
		if (randNum < 20) {
			grid[r][c] = tileBarrier; // 20% chance for barrier
		}
		else if (randNum < 15) {
			grid[r][c] = tileFood; // 5% chance for food
		}
		else if (randNum < 20) {
			grid[r][c] = tileHealth; // 5% chance for health kit
		}
		else if (randNum < 25) {
			grid[r][c] = tileAmmunition; // 5% chance for ammunition
		}
	}
}
// place a single safe zone on the map
do {
	safeZoneRow = rand() % mapRows;
	safeZoneCol = rand() % mapCols;
} while (grid[safeZoneRow][safeZoneCol] != tileEmpty);
grid[safeZoneRow][safeZoneCol] = 'S'; // 'S' represents the safe zone

// place some resources on the map
int resourcesPlaced = 0;
while (resourcesPlaced < minResources) {
	int r = rand() % mapRows;
	int c = rand() % mapCols;
	if (grid[r][c] == tileEmpty) {
		grid[r][c] = tileFood; // place food as a resource
		++resourcesPlaced;
	}
}
}
void placePlayer() {
	int row, col;
	do {
		row = rand() % mapRows;
		col = rand() % mapCols;
	} while (grid[row][col] != tileEmpty);
	player.row = row;
	player.col = col;
	grid[row][col] = tilePlayer;
}

void placeZombies() {
	zombies.clear();
	int placedZombies = 0;
	while (placedZombies < maxZombies) {
		int row = rand() % mapRows;
		int col = rand() % mapCols;
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
		resourceCollection();
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
	cout << "Health: " << player.health << " | Food: " << player.food
		<< " | Ammunition: " << player.ammunition
		<< " | Turns Remaining: " << remainingTurns << endl;
}
char getPlayerMove() {
	char move;
	cout << "Enter your move (W/A/S/D): ";
	cin >> move;
	return toupper(move);
}
void movePlayer(char direction) {
	int newRow = player.row;
	int newCol = player.col;
	switch (direction) {
	case 'W': newRow--; break;
	case 'S': newRow++; break;
	case 'A': newCol--; break;
	case 'D': newCol++; break;
	default: return; // invalid move
	}
	// Check boundaries and barriers
	if (newRow >= 0 && newRow < mapRows && newCol >= 0 && newCol < mapCols &&
		grid[newRow][newCol] != tileBarrier) {
		// Update grid
		grid[player.row][player.col] = tileEmpty;
		player.row = newRow;
		player.col = newCol;
		grid[player.row][player.col] = tilePlayer;
	}
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
		// Check boundaries and barriers
		if (newRow >= 0 && newRow < mapRows && newCol >= 0 && newCol < mapCols &&
			grid[newRow][newCol] == tileEmpty) //updated to prevent zombies from overlapping
			{
			grid[zombie.row][zombie.col] = tileEmpty; //clear old position
			zombie.row = newRow;
			zombie.col = newCol;
			grid[zombie.row][zombie.col] = tileZombie; //tile becomes zombie (nom nom)
		}
	}
}
void resourceCollection() {
	char tile = grid[player.row][player.col];
	switch (tile) {
	case tileFood:
		player.food++;
		cout << "You collected food!" << endl;
		break;
	case tileHealth:
		player.health += 20;
		if (player.health > 100) player.health = 100;
		cout << "You collected a health kit!" << endl;
		break;
	case tileAmmunition:
		player.ammunition += 5;
		cout << "You collected ammunition!" << endl;
		break;
	default:
		return; // no resource
	}
	// Clear the resource from the map
	grid[player.row][player.col] = tilePlayer;
}
void checkZombieEncounters() {
	for (auto& zombie : zombies) {
		if (zombie.alive && zombie.row == player.row && zombie.col == player.col) {
			if (player.ammunition > 0) {
				player.ammunition--;
				zombie.alive = false;
				grid[zombie.row][zombie.col] = tilePlayer;
				cout << "You encountered a zombie and used ammunition to kill it!" << endl;
			}
			else {
				player.health -= 20;
				cout << "You encountered a zombie and lost health!" << endl;
				if (player.health <= 0) {
					gameOver = true;
				}
			}
		}
	}
}
void checkSafeZone() {
	if (player.row == safeZoneRow && player.col == safeZoneCol) { //updated to match safe zone coords in global state
		playerReachedSafeZone = true;
		gameOver = true;
		playerWon = true;
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
}
void clearScreen() {
	#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}
