#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
using namespace std;

// Defining variables
bool gameOver;
const int width  = 50;
const int height = 25;
int x, y, fruitX, fruitY, bigFruitX, bigFruitY, score;
int tailX[100], tailY[100];
int nTail;
bool bigFruitExists    = false; // Track if the big fruit exists
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;
HANDLE hConsole;
bool inMenu = true;

void Setup() {
    gameOver = false;
    dir      = STOP;
    x        = width / 2;
    y        = height / 2;
    fruitX   = rand() % width;
    fruitY   = rand() % height;
    bigFruitExists = false;
    score          = 0;
    nTail          = 0;
}



void Draw() {
    // Move cursor to top-left corner without clearing the screen
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    SetConsoleCursorPosition(hConsole, coord);

    SetConsoleTextAttribute(hConsole, 10); // Green color for border and snake

    for (int i = 0; i < height + 2; i++) 
    {
        for (int j = 0; j < width + 2; j++) 
        {
            if (i == 0 || i == height + 1 || j == 0 || j == width + 1) 
            {
                cout << "#"; // Draw border
            } 
            else if (i - 1 == y && j - 1 == x) 
            {
                cout << "O"; // Snake head
            } 
            else if (i - 1 == fruitY && j - 1 == fruitX) 
            {
                SetConsoleTextAttribute(hConsole, 14); // Yellow color for fruit
                cout << "$";
                SetConsoleTextAttribute(hConsole, 10); // Reset to green for the border
            } 
            else if (bigFruitExists && i - 1 == bigFruitY && j - 1 == bigFruitX) 
            {
                SetConsoleTextAttribute(hConsole, 12); // Red color for big fruit
                cout << "@";
                SetConsoleTextAttribute(hConsole, 10); // Reset to green for the border
            } 
            else 
            {
                bool print = false;
                for (int k = 0; k < nTail; k++) 
                {
                    if (tailX[k] == j  && tailY[k] == i - 1) 
                    {
                        cout << "o"; // Snake body
                        print = true;
                    }
                }
                if (!print) cout << " ";
            }
        }
        cout << endl;
    }

    SetConsoleTextAttribute(hConsole, 15); // White color for score
    cout << "Score: " << score << endl;
}

void Input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a':
            if (dir != RIGHT) dir = LEFT;
            break;
        case 'd':
            if (dir != LEFT) dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN) dir = UP;
            break;
        case 's':
            if (dir != UP) dir = DOWN;
            break;
        case 'x':
            gameOver = true;
            break;
        }
    }
}

void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    
    
    switch (dir) {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }

    if (x >= width || x < 0 || y >= height || y < 0) // Wall hit detection
        gameOver = true;

    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y) // Body hit detection
            gameOver = true;

    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
    }

    if (bigFruitExists && x == bigFruitX && y == bigFruitY) {
        score += 50; // Increase score by 50 for big fruit
        nTail += 5;  // Increase snake length by 5 for big fruit
        bigFruitExists = false; // Remove the big fruit after it's eaten
    }

    // Generate big fruit at specific scores
    if (!bigFruitExists && (score == 50 || score == 150 || score == 250 || score == 350 || score == 450)) {
        bigFruitX = rand() % width;
        bigFruitY = rand() % height;
        bigFruitExists = true;
    }

    if (nTail == 100) { // Win condition
        gameOver = true;
    }
}

void GameOverScreen() {
    system("cls");
    if (nTail == 100) {
        SetConsoleTextAttribute(hConsole, 10); // Green color for win screen
        cout << "============================\n";
        cout << "        CONGRATULATIONS!     \n";
        cout << "        YOU WIN!            \n";
        cout << "============================\n";
    } else {
        SetConsoleTextAttribute(hConsole, 12); // Red color for game over screen
        cout << "============================\n";
        cout << "         GAME OVER          \n";
        cout << "============================\n";
    }
    cout << "Your final score: " << score << endl;
    cout << "Would you like to play again? (y/n): ";
    char choice;
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        Setup(); // Restart the game
    } else {
        inMenu = false; // Exit the menu loop
    }
}

void DisplayMenu() {
   system("cls");
    SetConsoleTextAttribute(hConsole, 11); // Cyan color for menu
    cout << "============================\n";
    cout << "        WELCOME TO          \n";
    cout << "          SNAKE.0           \n";
    cout << "============================\n";
    SetConsoleTextAttribute(hConsole, 14); // Yellow color for options
    cout << "1. Play Game\n";
    cout << "2. Instructions\n";
    cout << "3. Exit\n";
    SetConsoleTextAttribute(hConsole, 15); // White color for prompt
    cout << "Choose an option: ";
}

void ShowInstructions() {
    system("cls");
    SetConsoleTextAttribute(hConsole, 14); // Yellow color for instructions
    cout << "Instructions:\n";
    cout << "1. Use 'W', 'A', 'S', 'D' to move the snake.\n";
    cout << "2. Eat the fruit '$' to grow and increase your score.\n";
    cout << "3. If you hit the wall or your own body, the game is over.\n";
    cout << "4. Press 'X' during gameplay to exit the game.\n";
    cout << "5. Special '@' fruits appear when your score is 50, 100, 150, etc. They give extra points and increase the snake's length faster.\n";
    cout << "Press any key to return to the menu...";
    _getch(); // Wait for user input before returning to menu
}

int main() {
    // Set the console title
    SetConsoleTitle("SNAKE.0");

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    while (inMenu) {
        DisplayMenu();
        int choice;
        cin >> choice;
        switch (choice) {
        case 1:
            Setup();
            while (!gameOver) {
                Draw();
                Input();
                Logic() ;
                Sleep(60); // Slow down the game
            }
            GameOverScreen();
            break;
        case 2:
            ShowInstructions();
            break;
        case 3:
            inMenu = false;
            break;
        default:
            cout << "Invalid choice. Try again.\n";
            break;
        }
    }
    return 0;
}
