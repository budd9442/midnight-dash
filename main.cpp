#include "raylib.h"
#include <iostream>
#include <math.h>
#include <fstream>
using namespace std;

typedef enum GameState
{
    MENU,
    GAMEPLAY,
    INSTRUCTIONS,
    CREDITS,
    CHARACTER
} GameState;

string avatars[] = {
    "  Random girl #1",
    "  Random girl #2",
    "  Random girl #3",
    "   Intern Witch",
    "Unidentified blue dog"};

string descriptions[] = {
    "    Very classy",
    "    Very demure",
    "    Very mindful",
    "Terrible at driving",
    "    doesnt talk"};

int getHighScore(){
    ifstream scorefile("scores");
    int n;
    int max;
    scorefile>>max;
    while(scorefile>>n){
        if(n>max) max = n;
    }
    scorefile.close();
    return max;
}

void writeScore(int n){
    fstream scorefile("scores",ios::app);
    scorefile<<endl<<n;
    scorefile.close();
}



int main()
{
    //system("handlescores.exe ");
    // Initialization
    bool scoreUpdated = false;
    const int screenWidth = 1024;
    const int screenHeight = 768;
    int character = 2;
    int fps = 120;
    char playerName[20] = "\0"; // Buffer for storing the player's name
    int letterCount = 0;        // Count of entered letters

    InitWindow(screenWidth, screenHeight, "Midnight dash");
    SetTargetFPS(fps);
    int highScore = getHighScore();
    // Load textures (Replace with your own image file paths)
    Texture2D playerTextures[5][6];
    Texture2D coinTextures[6];

    GameState currentState = MENU;

    for (int k = 0; k < 5; k++)
    {
        for (int i = 0; i < 6; i++)
        {
            playerTextures[k][i] = LoadTexture(("sprites/players/" + to_string(k + 1) + "/" + to_string(i + 1) + ".png").c_str());
        }
    }

    for (int i = 0; i < 6; i++)
    {
        coinTextures[i] = LoadTexture(("sprites/other/coin/" + to_string(i + 1) + ".png").c_str());
    }

    Texture2D backgroundTexture = LoadTexture("background.png"); // Background image
    Texture2D menuTexture = LoadTexture("menu.png");
    Texture2D obstacleTexture = LoadTexture("sprites/obstacles/1.png"); // Obstacle sprite
    Texture2D heartTexture = LoadTexture("sprites/other/heart.png");    // Heart sprite
    Texture2D frameTexture = LoadTexture("sprites/other/frame.png");    // Heart sprite
    Texture2D profileBackgroundTexture = LoadTexture("profileSelector.png");
    Texture2D controlsTexture = LoadTexture("sprites/other/controls.png");
    Texture2D creditsTexture = LoadTexture("sprites/other/credits.png");
    // Player setup
    Rectangle player = {100, screenHeight - playerTextures[character - 1][0].height - 50, (float)playerTextures[character - 1][0].width, (float)playerTextures[character - 1][0].height}; // Player's position and size
    float playerJumpVelocity = -8;                                                                                                                                                        // How high the player jumps
    float playerVelocity = 0;                                                                                                                                                             // Player's vertical velocity
    bool isJumping = false;                                                                                                                                                               // Is the player jumping?

    Rectangle previeW = {100, 250, 400, 200};

    // Gravity
    const float gravity = 0.1;

    float backgroundX1 = 0.0f;
    float backgroundX2 = (float)backgroundTexture.width;
    float backgroundScrollSpeed = 5.0f;

    // Obstacle setup
    Rectangle obstacle = {screenWidth, screenHeight - obstacleTexture.height - 50, (float)obstacleTexture.width, (float)obstacleTexture.height}; // Obstacle position and size
    float obstacleSpeed = 5;                                                                                                                     // Speed of the obstacle

    Rectangle coin = {screenWidth, screenHeight - coinTextures[0].height - 100, (float)coinTextures[0].width, (float)coinTextures[0].height}; // Coin position and size
    coin.x = 2000;
    int coinSpeed = 5; // Speed of the coin
    int score = 0;     // Player's score

    // Life system
    int lives = 3;        // Number of lives
    bool gameOver = false; // Game Over flag

    int currentFrame = 0;      // Current frame of animation
    float frameCounter = 0.0f; // Counter for animation timing
    int currentFrame2 = 0;     // Current frame of animation
    float frameCounter2 = 0.0f;
    int currentFrame3 = 0; // Current frame of animation
    float frameCounter3 = 0.0f;
    float frameSpeed = 8.0f; // Speed of animation (frames per second)

    // Hurt effect variables
    bool isHurt = false;              // Is the player hurt?
    float hurtTimer = 0.0f;           // Timer to track hurt state duration
    const float hurtDuration = 0.5f;  // Duration of the hurt state (1 second)
    float flashTimer = 0.0f;          // Timer to control the flashing effect
    const float flashInterval = 0.1f; // Interval between flashes (0.1 second)
    float obstacleSpawnTimer = 0.0f;
    float obstacleSpawnInterval = 2.8f;
    float coinSpawnTimer = 0.0f;
    float coinSpawnInterval = 3.7f;
    
    // Main game loop
    while (!WindowShouldClose())
    {
        if (currentState == MENU)
        {
            DrawTexture(menuTexture, 0, 0, WHITE);
            DrawText(("High Score : "+to_string(highScore)).c_str(),610,630,32,RAYWHITE);
            // Check for button clicks in the main menu
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mousePosition = GetMousePosition();

                // Play Button
                if (CheckCollisionPointRec(mousePosition, (Rectangle){720, 300, 100, 40}))
                {
                    currentState = CHARACTER; // Start the game
                    lives = 3;
                    gameOver = false;
                    obstacle.x = screenWidth;
                    fps = 120;
                    SetTargetFPS(fps);
                    player.x = 100;
                    player.y = 700;
                    score = 0;
                }
                // Instructions Button
                else if (CheckCollisionPointRec(mousePosition, (Rectangle){720, 350, 200, 30}))
                {
                    currentState = INSTRUCTIONS; // Go to instructions screen
                }
                // Credits Button
                else if (CheckCollisionPointRec(mousePosition, (Rectangle){720, 400, 200, 30}))
                {
                    currentState = CREDITS; // Go to credits screen
                }
                else if (CheckCollisionPointRec(mousePosition, (Rectangle){720, 450, 200, 30}))
                {
                    CloseWindow();
                    exit(0);
                }
            }
        }
        else if (currentState == GAMEPLAY)
        {
            // All the gameplay code goes here (like the previous logic)
            // Handle player jumping, collision detection, obstacle/coin movement, etc.

            // Add logic to return to the main menu if the game is over
            // if (gameOver && IsKeyPressed(KEY_R))
            // {
            //     currentState = MENU; // Go back to main menu
            // }d
            player = {player.x, player.y, (float)playerTextures[character - 1][0].width, (float)playerTextures[character - 1][0].height}; // Player's position and size
        }
        else if (currentState == INSTRUCTIONS)
        {
            // Check for user input to return to the menu
            if (GetKeyPressed())
            {
                currentState = MENU; // Go back to main menu
            }
        }
        else if (currentState == CREDITS)
        {
            // Check for user input to return to the menu
            if (GetKeyPressed())
            {
                currentState = MENU; // Go back to main menu
            }
        }
        else if (currentState == CHARACTER)
        {
            // Check for user input to return to the menu
            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentState = MENU; // Go back to main menu
            }
        }
        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentState == MENU)
        {
            // Draw the Main Menu
            // DrawText("Runner Game", screenWidth / 2 - MeasureText("Runner Game", 40) / 2, screenHeight / 2 - 150, 40, DARKGRAY);
            // DrawRectangle(screenWidth / 2 - 50, screenHeight / 2 - 60, 100, 40, LIGHTGRAY);
            DrawText("> Play", 720, 300, 36, PURPLE);
            DrawText("> Controls", 720, 350, 36, PURPLE);
            DrawText("> Credits", 720, 400, 36, PURPLE);
            DrawText("> Quit", 720, 450, 36, PURPLE);
            scoreUpdated = false;
            // DrawRectangle(screenWidth / 2 - 50, screenHeight / 2, 100, 40, LIGHTGRAY);

            // DrawRectangle(screenWidth / 2 - 50, screenHeight / 2 + 60, 100, 40, LIGHTGRAY);
        }
        else if (currentState == GAMEPLAY)
        {
            if (!gameOver)
            {
                // Handle player jumping
                if (character == 4)
                {
                    if (IsKeyDown(KEY_W))
                    {
                        player.y -= 1;
                    }
                    if (IsKeyDown(KEY_S))
                    {
                        player.y += 1;
                    }
                }
                else
                {
                    if (IsKeyPressed(KEY_SPACE) && !isJumping)
                    {
                        playerVelocity = playerJumpVelocity;
                        isJumping = true;
                    }
                }

                if (IsKeyDown(KEY_A))
                {
                    if (player.x > 50)
                        player.x -= 2;
                }
                if (IsKeyDown(KEY_D))
                {
                    if (player.x < screenWidth - 200)
                        player.x += 1;
                }
                else
                {
                    if (player.x > 50)
                        player.x -= 0.5;
                }

                // Apply gravity
                if (character != 4)
                {
                    playerVelocity += gravity;
                    player.y += playerVelocity;
                }
                else
                {
                    playerVelocity += gravity / 1000;
                    player.y += playerVelocity;
                }

                // Prevent player from falling through the ground
                if (player.y >= screenHeight - player.height - 50)
                {
                    player.y = screenHeight - player.height - 50;
                    playerVelocity = 0;
                    isJumping = false;
                }

                // Move obstacle
                obstacle.x -= obstacleSpeed;
                // Move coin
                coin.x -= coinSpeed;

                obstacleSpawnTimer += GetFrameTime();

                if (obstacleSpawnTimer >= obstacleSpawnInterval && (obstacle.x > screenWidth || obstacle.x < 0))
                {
                    obstacle.x = screenWidth;                                // Reset obstacle to the right of the screen
                    obstacle.y = screenHeight - obstacleTexture.height - 50; // Randomize Y position
                    obstacleSpawnTimer = 0.0f;
                    obstacleSpawnInterval = rand() % 10;
                    fps += 5;
                    SetTargetFPS(fps);
                }

                coinSpawnTimer += GetFrameTime();
                if (coinSpawnTimer >= coinSpawnInterval && (coin.x > screenWidth || coin.x < 0))
                {

                    coin.x = screenWidth;                                                  // Reset coin to the right of the screen
                    coin.y = screenHeight - coinTextures[0].height - 100 - (rand() % 300); // Randomize Y position within a range
                    coinSpawnTimer = 0.0f;
                    coinSpawnInterval = rand() % 5;
                }
                // Check for collision between player and obstacle
                if (CheckCollisionRecs(player, obstacle))
                {
                    // Reduce life and reset obstacle position
                    lives--;
                    obstacle.x = 2000;
                    isHurt = true;
                    hurtTimer = 0.0f;
                    flashTimer = 0.0f;

                    // Check for Game Over
                    if (lives <= 0)
                    {
                        gameOver = true;
                    }
                }
                if (CheckCollisionRecs(player, coin))
                {
                    // Increase score and reset coin position
                    score += 10;
                    coin.x = 2000;
                }
                frameCounter2 += GetFrameTime() * 5; // Update counter based on time and animation speed
                if (frameCounter2 >= 1.0f)
                {
                    frameCounter2 = 0.0f;
                    currentFrame2++;
                    if (currentFrame2 > 5)
                        currentFrame2 = 0; // Loop through frames (assuming 6 frames total)
                }
                if (!isJumping)
                {
                    frameCounter += GetFrameTime() * frameSpeed; // Update counter based on time and animation speed
                    if (frameCounter >= 1.0f)
                    {
                        frameCounter = 0.0f;
                        currentFrame++;
                        if (currentFrame > 5)
                            currentFrame = 0; // Loop through frames (assuming 6 frames total)
                    }
                }
                else
                {
                    currentFrame = 0; // Reset to the first frame when jumping
                }
                backgroundX1 -= backgroundScrollSpeed;
                backgroundX2 -= backgroundScrollSpeed;

                // Reset background position when it moves out of the screen
                if (backgroundX1 <= -backgroundTexture.width)
                {
                    backgroundX1 = backgroundX2 + backgroundTexture.width;
                }
                if (backgroundX2 <= -backgroundTexture.width)
                {
                    backgroundX2 = backgroundX1 + backgroundTexture.width;
                }
                // Handle hurt state and flashing effect
                if (isHurt)
                {
                    hurtTimer += GetFrameTime();  // Increment hurt timer
                    flashTimer += GetFrameTime(); // Increment flash timer

                    if (hurtTimer >= hurtDuration)
                    {
                        isHurt = false; // End hurt state after the duration
                    }

                    if (flashTimer >= flashInterval)
                    {
                        flashTimer = 0.0f; // Reset flash timer for blinking effect
                    }
                }
            }
            BeginDrawing();
            ClearBackground(RAYWHITE);
            // Draw background
            DrawTexture(backgroundTexture, (int)backgroundX1, 0, WHITE);
            DrawTexture(backgroundTexture, (int)backgroundX2, 0, WHITE);
            
            if (gameOver)
            {
                // Draw Game Over screen
                DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 100) / 2, screenHeight / 2 - 20, 100, RED);
                DrawText("Press R to Restart, M to go back to menu", 300,680, 20, GRAY);
                if(score>getHighScore())
                    DrawText(("New Highscore : " + to_string(score) ).c_str(), 400, 490, 32, WHITE);
                else
                DrawText(("Score : " + to_string(score)).c_str(), 430, 490, 32, WHITE);
                if(!scoreUpdated)
                {
                    writeScore(score);
                    scoreUpdated =  true;
                }
                // Restart game when R is pressed
                if (IsKeyPressed(KEY_R))
                {
                    lives = 3;
                    gameOver = false;
                    obstacle.x = screenWidth;
                    fps = 120;
                    SetTargetFPS(fps);
                    currentState = GAMEPLAY;
                    player.x = 100;
                    player.y = 700;
                    isHurt = false;
                    score = 0;
                    scoreUpdated = false;
                }
                if (IsKeyPressed(KEY_M))
                {
                    gameOver = false;
                    fps = 120;
                    SetTargetFPS(fps);
                    highScore = getHighScore();
                    currentState = MENU;
                    scoreUpdated =  false;
                }
            }
            else
            {
                if (!isHurt || (flashTimer < flashInterval / 2))
                {
                    DrawTexture(playerTextures[character - 1][currentFrame], (int)player.x, (int)player.y, WHITE);
                }
                // Draw player and obstacle using sprites
                // DrawTexture(playerTextures[currentFrame], (int)player.x, (int)player.y, WHITE);
                DrawTexture(obstacleTexture, (int)obstacle.x, (int)obstacle.y, WHITE);
                DrawTexture(coinTextures[currentFrame2], (int)coin.x, (int)coin.y, YELLOW);

                // Draw lives as hearts
                for (int i = 0; i < lives; i++)
                {
                    // DrawRectangle( 20, 20, RED);
                    DrawTexture(heartTexture, 10 + 36 * i, 10, WHITE);
                }

                if (score > 0)
                    DrawText(("Score : " + to_string(score)).c_str(), (screenWidth - 110 - (floor(log10(score + 1) + 1)) * 10), 10, 24, WHITE);
                else
                    DrawText("Score : 00", screenWidth - 130, 10, 24, WHITE);
            }
        }
        else if (currentState == INSTRUCTIONS)
        {
            // Draw the instructions screen
            DrawTexture(controlsTexture, 0, 0, WHITE);
        }
        else if (currentState == CREDITS)
        {
            // Draw the credits screen
            DrawTexture(creditsTexture, 0, 0, WHITE);
        }
        else if (currentState == CHARACTER)
        {
            DrawTexture(profileBackgroundTexture, 0, 0, WHITE);
            DrawText(playerName, 365, 675, 32, WHITE);
            DrawText(avatars[character - 1].c_str(), 70, 210, 32, WHITE);
            DrawText(descriptions[character - 1].c_str(), 70, 550, 32, WHITE);
            frameCounter3 += GetFrameTime() * 5; // Update counter based on time and animation speed

            int key = GetKeyPressed();
            if (key >= 32 && key <= 125 && letterCount < 12)
            {
                playerName[letterCount] = (char)key;
                playerName[letterCount + 1] = '\0';
                letterCount++;
            }

            if (IsKeyPressed(KEY_BACKSPACE) && letterCount > 0)
            {
                playerName[letterCount - 1] = '\0';
                letterCount--;
            }

            if (IsKeyPressed(KEY_ENTER) && letterCount > 0)
            {
                lives = 3;
                gameOver = false;
                obstacle.x = screenWidth;
                fps = 120;
                SetTargetFPS(fps);
                currentState = GAMEPLAY;
                player.x = 100;
                player.y = 700;
                isHurt = false;
                score = 0;
            }

            if (frameCounter3 >= 1.0f)
            {
                frameCounter3 = 0.0f;
                currentFrame3++;
                if (currentFrame3 > 5)
                    currentFrame3 = 0; // Loop through frames (assuming 6 frames total)
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mousePosition = GetMousePosition();
                if (CheckCollisionPointRec(mousePosition, (Rectangle){450, 250, 120, 120}))
                {
                    character = 1;
                    previeW = {200, 350, 400, 200};
                }
                if (CheckCollisionPointRec(mousePosition, (Rectangle){600, 250, 120, 120}))
                {
                    character = 2;
                    previeW = {200, 350, 400, 200};
                }
                if (CheckCollisionPointRec(mousePosition, (Rectangle){750, 250, 120, 120}))
                {
                    character = 3;
                    previeW = {200, 350, 400, 200};
                }
                if (CheckCollisionPointRec(mousePosition, (Rectangle){450, 400, 120, 120}))
                {
                    character = 4;
                    previeW = {100, 290, 400, 200};
                }
                if (CheckCollisionPointRec(mousePosition, (Rectangle){600, 400, 120, 120}))
                {
                    character = 5;
                    previeW = {200, 350, 400, 200};
                }
                if (CheckCollisionPointRec(mousePosition, (Rectangle){655, 655, 215, 60}))
                {
                    lives = 3;
                    gameOver = false;
                    obstacle.x = screenWidth;
                    fps = 120;
                    SetTargetFPS(fps);
                    currentState = GAMEPLAY;
                    player.x = 100;
                    player.y = 700;
                    isHurt = false;
                    score = 0;
                }
            }
            DrawTexture(playerTextures[character - 1][currentFrame3], (int)previeW.x, (int)previeW.y, WHITE);
        }

        EndDrawing();
    }
    CloseWindow(); // Close window and OpenGL context
    return 0;
}