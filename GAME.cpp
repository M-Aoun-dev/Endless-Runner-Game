#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <ctime>
using namespace std;

// declearing constants
const int mapWidth = 30;
const int mapHeight = 20;
const char playerChar = 'P';
const char coinChar = '$';
const char hurdleChar = 'X';
const char wallChar = '#';
const int maxLives = 3;

// creating player structure
struct Player {
    char name[30];
    int score;
    int distance;
};

// declaring varables
char gameMap[mapHeight][mapWidth];
int playerX = mapWidth / 2;
int playerY = mapHeight - 2;
int score = 0;
int distanceCovered = 0;
int lives = maxLives;
int speed = 220;  // speed of blinking

// creating hurdles, coins and other map objects
const int maxObjects = 100;
int coinX[maxObjects], coinY[maxObjects], totalCoins = 0;
int hurdleX[maxObjects], hurdleY[maxObjects], totalHurdles = 0;

// creating prototypes of all functions
void mainMenu();
void instructions();
void help();
void playGame();
void drawMap();
void updateMap();
void movePlayer(char key);
void generateCoins();
void generateHurdles();
void moveObjectsDown();
bool checkCollision();
void saveHighScore(Player p);
void viewHighScores();
void delay(int ms);
void clearScreen();
bool isCoin(int x, int y);
bool isHurdle(int x, int y);

// creating main function
int main() {
    srand(time(0));
    mainMenu();
    system("pause");
    return 0;
}

// creating menu function
void mainMenu() {
    while (true) {
        clearScreen();
        cout << "=====================================\n";
        cout << "    WELCOME TO ENDLESS RUNNER GAME   \n";
        cout << "=====================================\n";
        cout << "1. Play Game\n";
        cout << "2. Instructions\n";
        cout << "3. Help\n";
        cout << "4. Highest Scores\n";
        cout << "5. Exit\n";
        cout << "Select an option (1-5): ";
        char choice = _getch();

        // creating switch for choice matching
        switch (choice) {
        case '1':
            playGame();
            break;
        case '2':
            instructions();
            break;
        case '3':
            help();
            break;
        case '4':
            viewHighScores();
            break;
        case '5':
            return;
        default:
            cout << endl;
            cout << "Invalid Choice!" << endl;
            cout << "Next Time Please Enter The Correct Choice: " << endl;
            Sleep(1500);
        }
    }
}

// craeting function for instructions
void instructions() {
    clearScreen();
    cout << "========== INSTRUCTIONS ==========" << endl;
    cout << "Player: " << playerChar << endl;
    cout << "Coins: " << coinChar << endl;
    cout << "Hurdle: " << hurdleChar << endl;
    cout << "Controls: W/A/S/D" << endl;
    cout << "Collect Coins and Be Safe from Hurdles." << endl;
    cout << "Survive as long as you can." << endl << endl;
    cout << "Press any key to return Main Menu. ";
    _getch();
}

// creating function for Help of player
void help() {
    clearScreen();
    cout << "========== HELP ==========" << endl;
    cout << "Collect coins: " << coinChar << endl;
    cout << "Avoid Hurdles: " << hurdleChar << endl;
    cout << "Maximum Lives: " << lives << endl;
    cout << "'W' for Upp" << endl;
    cout << "'A' for Left" << endl;
    cout << "'S' for Down" << endl;
    cout << "'D' for Right" << endl;
    cout << "Speed increases with distance. " << endl;
    cout << "Press any key to return Main Menu. ";
    _getch();
}

//  creating function for storing high score
void saveHighScore(Player p) {
    ofstream file("highscores.txt", ios::app);  // opening the file
    file << p.name << " " << p.score << " " << p.distance << endl;
    file.close();
}

// creating function to watch high score
void viewHighScores() {
    clearScreen();
    cout << "========== HIGHEST SCORES ==========\n";
    ifstream file("highscores.txt");
    Player temp;
    while (file >> temp.name >> temp.score >> temp.distance) {
        cout << "Player: " << temp.name << "\t  Score: " << temp.score << "\t Distance: " << temp.distance << "\n";
    }
    file.close();
    cout << endl;
    cout << "Press any key to return...";
    _getch();
}

//  creating function for playing game
void playGame() {
    clearScreen();
    score = 0;
    lives = maxLives; 
    distanceCovered = 0;
    playerX = mapWidth / 2;
    playerY = mapHeight - 2;
    totalCoins = totalHurdles = 0;

    // creating file to store player name
    Player currentPlayer;
    cout << "Enter your Name: ";
    cin >> currentPlayer.name;

    // calling functions to play game
    while (lives > 0) {
        generateCoins();
        generateHurdles();
        moveObjectsDown();
        updateMap();
        drawMap();
        
        // showing data of player during playing game
        cout << "Name: " << currentPlayer.name << endl;
        cout << "Score: " << score << endl;
        cout << "Distance: " << distanceCovered << endl;
        cout << "Lives: " << lives << endl;

        // calling function of moving player
        if (_kbhit()) {
            char key = _getch();
            movePlayer(key);
        }
        // checking collisions to remove lives
        if (checkCollision()) {
            lives--;
        }

        distanceCovered += 10;   // increasing score


        if (distanceCovered % 1000 == 0) {
            speed = max(speed - 20, 50);
        }
        delay(speed);
    }
    //  showing data at end of game
    clearScreen();
    cout << endl;
    cout << "Game Over!" << endl;
    cout << "Final Score: " << score << ", Distance: " << distanceCovered << "\n";

    //  storing data in file
    currentPlayer.score = score;
    currentPlayer.distance = distanceCovered;
    saveHighScore(currentPlayer);
    cout << "Press any key to return to menu...";
    _getch();
}

// creating function to draw map
void drawMap() {
    clearScreen();
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (y == 0 || y == mapHeight - 1) {
                if (x == 0 || x == mapWidth - 1) {
                    cout << "|";        // Corners
                }
                else {
                    cout << "=";        // Top and bottom border line
                }
            }
            // Side boundaries
            else if (x == 0 || x == mapWidth - 1) {
                cout << "|";            // Left and right sides
            }
            // Empty inner space
            else {
                cout << gameMap[y][x];  // Print map elements (player, coin, hurdle, etc.)
            }
        }
        cout << endl;
    }
}

// creating function to update map
void updateMap() {
    // Clear the map
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            gameMap[y][x] = ' ';
        }
    }
    for (int y = 0; y < mapHeight; y++) {
        gameMap[y][0] = wallChar;
        gameMap[y][mapWidth - 1] = wallChar;
    }

    // Placing the coins and hurdles
    for (int i = 0; i < totalCoins; i++) {
        if (coinY[i] >= 0 && coinY[i] < mapHeight) {
            gameMap[coinY[i]][coinX[i]] = coinChar;
        }
    }
    for (int i = 0; i < totalHurdles; i++) {
        if (hurdleY[i] >= 0 && hurdleY[i] < mapHeight) {
            gameMap[hurdleY[i]][hurdleX[i]] = hurdleChar;
        }
    }

    // Placing the player
    gameMap[playerY][playerX] = playerChar;
}

//  creating function to move player
void movePlayer(char key) {
    int newX = playerX;
    int newY = playerY;
    if (key == 'a' || key == 'A') {
        newX--;
    }
    else if (key == 'd' || key == 'D') {
        newX++;
    }
    else if (key == 'w' || key == 'W') {
        newY--;
    }
    else if (key == 's' || key == 'S') {
        newY++;
    }
    if (newX > 0 && newX < mapWidth - 1 && newY > 0 && newY < mapHeight - 1) {
        playerX = newX;
        playerY = newY;
    }
}

//  creating function to generate coins
void generateCoins() {
    if (totalCoins < maxObjects && rand() % 4 == 0) {
        coinX[totalCoins] = rand() % (mapWidth - 2) + 1;
        coinY[totalCoins] = 1;
        totalCoins++;
    }
}

//  creating function to generate hurdles
void generateHurdles() {
    if (totalHurdles < maxObjects && rand() % 5 == 0) {
        hurdleX[totalHurdles] = rand() % (mapWidth - 2) + 1;
        hurdleY[totalHurdles] = 1;
        totalHurdles++;
    }
}

//  creating function to move things down
void moveObjectsDown() {
    for (int i = 0; i < totalCoins; i++) {
        coinY[i]++;
    }
    for (int i = 0; i < totalHurdles; i++) {
        hurdleY[i]++;
    }
}

// creating functoin to check collision
bool checkCollision() {
    for (int i = 0; i < totalCoins; i++) {
        if (coinX[i] == playerX && coinY[i] == playerY) {
            score += 10;  //increasing score
            coinY[i] = -1; // removing coin
            return false;
        }
    }
    for (int i = 0; i < totalHurdles; i++) {
        if (hurdleX[i] == playerX && hurdleY[i] == playerY) {
            hurdleY[i] = -1; // removing hurdle
            return true;
        }
    }
    return false;
}

// creating function to check coins on map
bool isCoin(int x, int y) {
    for (int i = 0; i < totalCoins; i++) {
        if (coinX[i] == x && coinY[i] == y) {
            return true;
        }
    }
    return false;
}

//  creating function to check hurdles on map
bool isHurdle(int x, int y) {
    for (int i = 0; i < totalHurdles; i++) {
        if (hurdleX[i] == x && hurdleY[i] == y) {
            return true;
        }
    }
    return false;
}

// creating function for clearing screen
void clearScreen() {
    system("cls");
}

// creating function of delay
void delay(int ms) {
    Sleep(ms);
}
