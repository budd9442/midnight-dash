// importing relevent libraries
#include "raylib.h" // for game design
#include <iostream> // basic input output for debugging
#include <math.h>   // generating random numbers
#include <fstream>  // file handling
#include <vector>   // to store highscore pairs
#include <string>   // to use strings
#include <thread>   // to run highscore uploads and downloads in a different thread
using namespace std;

typedef enum GameState // Enumerate to keep track of game state
{
    MENU,
    GAMEPLAY,
    INSTRUCTIONS,
    CREDITS,
    CHARACTER,
    GAMEOVER
} GameState;

vector<pair<string, string>> highScores; // vector to store name and score of top 5 players

string avatars[] = { // names of avatars
    "  Random girl #1",
    "  Random girl #2",
    "  Random girl #3",
    "   Intern Witch",
    "Unidentified blue dog"};

string descriptions[] = { // avatar descriptions
    "    Very classy",
    "    Very demure",
    "    Very mindful",
    "Terrible at driving",
    "    doesnt talk"};

int getPersonalBest() // get the highest score from locally saved scores file
{
    ifstream scorefile("scores");
    int n;
    int max;
    scorefile >> max;
    while (scorefile >> n)
    {
        if (n > max)
            max = n;
    }
    scorefile.close(); // close file after using
    return max;
}

void writeScore(int n) // write the score to loclaly saved scores file
{
    fstream scorefile("scores", ios::app); // ios:app used to append contents to the existing fiile..
    scorefile << endl                      // ..to avoid losing exisiting score records
              << n;
    scorefile.close(); // close file after using
}

void uploadScores(string name, int score) // uses a precompiled binary to upload the scores to a firebase real time database
{
    system(("uploadscore.exe " + string(name) + " " + to_string(score)).c_str()); // ./uploadscore.exe <name> <score>
}

void downloadScores() // uses a precompiled binary to get the top 5 scores from firebase and store them in highscores file
{
    system("getScores.exe"); // ./getscores.exe
}

void updateHighScores() // read the updated highscores file and store the data in highscores vector
{
    highScores.clear();
    string name, score;
    ifstream scorefile("highScores");
    while (scorefile >> name)
    {
        scorefile >> score;
        highScores.push_back(make_pair(name, score));
    }
    scorefile.close(); // close the file after using
}

int main() // main function
{
    thread downloadThread(downloadScores);
    downloadThread.detach();
    //   Initialization
    bool scoreUpdated = false; // variable to keep track whether current score is updated locally and online

    const int screenWidth = 1024; // width of the game screen
    const int screenHeight = 768; // height of the game screen
    int character = 1;            // index of currently selected character
    int fps = 120;                // starting 'frames per second' of the game
    char playerName[20] = "\0";   // Buffer for storing the player's name
    int letterCount = 0;          // Count of entered letters

    InitWindow(screenWidth, screenHeight, "Midnight dash"); // default function to initialize the game window
    SetTargetFPS(fps);                                      // sets starting fps

    // loading relevant texture resources
    Texture2D playerTextures[5][6]; // a 2D array to store frames of 5 characters ( 6 frames each )
    Texture2D coinTextures[6];      // coin textures

    for (int k = 0; k < 5; k++)
    {
        for (int i = 0; i < 6; i++)
        {
            playerTextures[k][i] = LoadTexture(("assets/players/" + to_string(k + 1) + "/" + to_string(i + 1) + ".png").c_str());
        }
    }

    for (int i = 0; i < 6; i++)
    {
        coinTextures[i] = LoadTexture(("assets/other/coin/" + to_string(i + 1) + ".png").c_str());
    }

    Texture2D backgroundTexture = LoadTexture("assets/background.png");                   // Background image
    Texture2D menuTexture = LoadTexture("assets/menus/menu.png");                         // menu texture
    Texture2D obstacleTexture = LoadTexture("assets/obstacles/1.png");                    // Obstacle texture
    Texture2D heartTexture = LoadTexture("assets/other/heart.png");                       // Heart texture
    Texture2D profileBackgroundTexture = LoadTexture("assets/menus/profileSelector.png"); // profile selector background
    Texture2D controlsTexture = LoadTexture("assets/menus/controls.png");                 // controls background
    Texture2D creditsTexture = LoadTexture("assets/menus/credits.png");                   // credits background
    Texture2D gameOverTexture = LoadTexture("assets/menus/gameover.png");                 // game over background

    int personalBest = getPersonalBest(); // retrieves personal best score and stores it in a varianle

    GameState currentState = MENU; // set the current game state to menu

    // bounding box of preview area in character selectiion menu
    Rectangle previeW = {200, 350, 400, 200};

    // Player setup
    Rectangle player = {100, screenHeight - playerTextures[character - 1][0].height - 50, (float)playerTextures[character - 1][0].width, (float)playerTextures[character - 1][0].height}; // Player's position and size
    float playerJumpVelocity = -8;                                                                                                                                                        // negative because player jumps vertically upwards ( -y )                                                                                                                                                        // How high the player jumps
    float playerVelocity = 0;                                                                                                                                                             // variable to track player velocity                                                                                                                                                             // Player's vertical velocity
    bool isJumping = false;                                                                                                                                                               // varuable to track whether a player is jumping                                                                                                                   // Is the player jumping?

    // Gravity
    const float gravity = 0.1;

    // background scrolling
    float backgroundX1 = 0.0f;
    float backgroundX2 = (float)backgroundTexture.width;
    float backgroundScrollSpeed = 5.0f;

    // Obstacle setup
    Rectangle obstacle = {screenWidth, screenHeight - obstacleTexture.height - 50, (float)obstacleTexture.width, (float)obstacleTexture.height}; // Obstacle position and size
    float obstacleSpeed = 5;                                                                                                                     // Speed of an obstacle                                                                                                                  // Speed of the obstacle

    // coin setup
    Rectangle coin = {screenWidth, screenHeight - coinTextures[0].height - 100, (float)coinTextures[0].width, (float)coinTextures[0].height}; // Coin position and size
    coin.x = 2000;
    int coinSpeed = 5; // Speed of the coin

    int score = 0; // Player's score

    int lives = 3; // Number of lives

    bool gameOver = false; // Game Over flag

    // frame tracking for animating the runner
    int runnerFrame = 0;
    float runnerFrameCounter = 0.0f;

    // frame tracking for animating coins
    int coinFrame = 0;
    float coinFrameCounter = 0.0f;

    // frame tracking for animating the character previews
    int previewFrame = 0;
    float previewFrameCounter = 0.0f;

    float frameSpeed = 8.0f; // Speed of animation

    // Hurt effect
    bool isHurt = false;              // Is the player hurt?
    float hurtTimer = 0.0f;           // Timer to track hurt state duration
    const float hurtDuration = 0.5f;  // Duration of the hurt state
    float flashTimer = 0.0f;          // Timer to control the flashing effect
    const float flashInterval = 0.1f; // Interval between flashes (0.1 second)

    // obstacle spawning
    float obstacleSpawnTimer = 0.0f;
    float obstacleSpawnInterval = 2.8f;

    // coin spawning
    float coinSpawnTimer = 0.0f;
    float coinSpawnInterval = 3.7f;

    updateHighScores();

    // Main game loop
    while (!WindowShouldClose())
    {
        if (currentState == MENU)
        {
            DrawTexture(menuTexture, 0, 0, WHITE); // main menu background

            DrawText(("Personal Best : " + to_string(personalBest)).c_str(), 510, 630, 32, RAYWHITE);
            DrawText("> Play", 720, 300, 36, PURPLE);
            DrawText("> Controls", 720, 350, 36, PURPLE);
            DrawText("> Credits", 720, 400, 36, PURPLE);
            DrawText("> Quit", 720, 450, 36, PURPLE);

            scoreUpdated = false; 
            int count = 0;

            for (pair<string, string> p : highScores) // foreach loop to display top 5 players
            {
                DrawText((p.first).c_str(), 45, 170 + (count * 30), 24, WHITE);
                DrawText((p.second).c_str(), 250, 170 + (count * 30), 24, WHITE);
                count++;
            }

            // Check for button clicks in the main menu
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mousePosition = GetMousePosition();

                // Play Button
                if (CheckCollisionPointRec(mousePosition, (Rectangle){720, 300, 100, 40}))
                {
                    currentState = CHARACTER; // go to character selection
                    lives = 3;                // resetting variables
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
                    currentState = INSTRUCTIONS;
                }
                // Credits Button
                else if (CheckCollisionPointRec(mousePosition, (Rectangle){720, 400, 200, 30}))
                {
                    currentState = CREDITS;
                }
                // exit buttpn
                else if (CheckCollisionPointRec(mousePosition, (Rectangle){720, 450, 200, 30}))
                {
                    CloseWindow();
                    exit(0);
                }
            }
        }
        else if (currentState == GAMEPLAY)
        {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            // Draw background
            DrawTexture(backgroundTexture, (int)backgroundX1, 0, WHITE);
            DrawTexture(backgroundTexture, (int)backgroundX2, 0, WHITE);
            player = {player.x, player.y, (float)playerTextures[character - 1][0].width, (float)playerTextures[character - 1][0].height}; // Player's position and size
            if (!gameOver)
            {
                // Handle player jumping
                if (character == 4) // fly mode
                {
                    if (IsKeyDown(KEY_W))
                    {
                        if (player.y>10)   // to prevent character going out of bounds
                        player.y -= 1;
                    }
                    if (IsKeyDown(KEY_S))
                    {
                        player.y += 1;
                    }
                }
                else // walking mode
                {
                    if (IsKeyPressed(KEY_SPACE) && !isJumping)
                    {
                        playerVelocity = playerJumpVelocity; // jump when space is pressed
                        isJumping = true;
                    }
                }

                if (IsKeyDown(KEY_A))
                {
                    if (player.x > 50)  // to prevent character going out of bounds
                        player.x -= 2;
                }
                if (IsKeyDown(KEY_D))
                {
                    if (player.x < screenWidth - 200)   // to prevent character going out of bounds
                        player.x += 1;
                }
                else
                {
                    if (player.x > 50)  // make the character sowly fall behind when not actively running
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


                // timer to keep track of obstacles spawning
                obstacleSpawnTimer += GetFrameTime();

                //if enough time has passed since last obstacle and there are no active obstacles
                if (obstacleSpawnTimer >= obstacleSpawnInterval && (obstacle.x > screenWidth || obstacle.x < 0))
                {
                    obstacle.x = screenWidth; // Reset obstacle to the right of the screen
                    obstacle.y = screenHeight - obstacleTexture.height - 50;
                    obstacleSpawnTimer = 0.0f;
                    obstacleSpawnInterval = rand() % 10; // randomly set the time until next obstacle spawns
                    fps += 5; // increase game speed
                    SetTargetFPS(fps);
                }

                
                coinSpawnTimer += GetFrameTime();
                 //if enough time has passed since last coin and there are no active coin
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
                    obstacle.x = 2000;  // move the obstacle out of screen
                    isHurt = true;      // hurt effect 
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

                //frame counters are used for tracking frames of specific objects
                coinFrameCounter += GetFrameTime() * 5; // Update counter based on time and animation speed
                if (coinFrameCounter >= 1.0f)
                {
                    coinFrameCounter = 0.0f;
                    coinFrame++;
                    if (coinFrame > 5)
                        coinFrame = 0; // Loop through frames ( 6 frames total)
                }
                if (!isJumping) //dont animate the player when jumping
                {
                    runnerFrameCounter += GetFrameTime() * frameSpeed; // Update counter based on time and animation speed
                    if (runnerFrameCounter >= 1.0f)
                    {
                        runnerFrameCounter = 0.0f;
                        runnerFrame++;
                        if (runnerFrame > 5)
                            runnerFrame = 0; // Loop through frames (assuming 6 frames total)
                    }
                }
                else
                {
                    runnerFrame = 0; // Reset to the first frame when jumping
                }

                backgroundX1 -= backgroundScrollSpeed; // Background unlimited scrolling effect
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

            if (gameOver)
            {
                currentState = GAMEOVER;
            }
            else
            {
                if (!isHurt || (flashTimer < flashInterval / 2)) // when hurt, blink the character
                {
                    DrawTexture(playerTextures[character - 1][runnerFrame], (int)player.x, (int)player.y, WHITE);
                }
                
                DrawTexture(obstacleTexture, (int)obstacle.x, (int)obstacle.y, WHITE);
                DrawTexture(coinTextures[coinFrame], (int)coin.x, (int)coin.y, YELLOW);

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
            DrawTexture(controlsTexture, 0, 0, WHITE);
            if (GetKeyPressed())// Check for user input to return to the menu
            {
                currentState = MENU; // Go back to main menu
            }
        }
        else if (currentState == CREDITS)
        {
            DrawTexture(creditsTexture, 0, 0, WHITE);
            // Check for user input to return to the menu
            if (GetKeyPressed())
            {
                currentState = MENU; // Go back to main menu
            }
        }
        else if (currentState == CHARACTER)
        {
            // Check for user input to return to the menu
            if (IsKeyPressed(KEY_TAB))
            {
                currentState = MENU; // Go back to main menu
            }
            // draw relevent textures and texts
            DrawTexture(profileBackgroundTexture, 0, 0, WHITE);
            DrawText(playerName, 365, 675, 32, WHITE);
            DrawText(avatars[character - 1].c_str(), 70, 210, 32, WHITE);
            DrawText(descriptions[character - 1].c_str(), 70, 550, 32, WHITE);
            previewFrameCounter += GetFrameTime() * 5; // Update counter based on time and animation speed

            //handling keyboard input for player name
            int key = GetCharPressed();
            cout<<(char)key;
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
            if (IsKeyPressed(KEY_ENTER) && letterCount > 0) //if length of name > 0, start the game
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
            //frame counter for character rpeviews in avatar selection menu
            if (previewFrameCounter >= 1.0f)
            {
                previewFrameCounter = 0.0f;
                previewFrame++;
                if (previewFrame > 5)
                    previewFrame = 0; // Loop through frames (assuming 6 frames total)
            }
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) // when clicking on specific avatars
            {
                Vector2 mousePosition = GetMousePosition();
                if (CheckCollisionPointRec(mousePosition, (Rectangle){450, 250, 120, 120}))
                {
                    character = 1;
                    previeW = {200, 350, 400, 200}; // changing the location of the preview as some characters are different in size
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
                if (CheckCollisionPointRec(mousePosition, (Rectangle){655, 655, 215, 60}) && letterCount > 0)
                {
                    // when start button is pressed with a valid name, start the game
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
            DrawTexture(playerTextures[character - 1][previewFrame], (int)previeW.x, (int)previeW.y, WHITE);
        }
        else if (currentState == GAMEOVER)
        {
            // Draw Game Over screen
            DrawTexture(gameOverTexture,0,0,WHITE);
            if (score > getPersonalBest()) // when the score is greater than all locally existing records
                DrawText(("New personalBest : " + to_string(score)).c_str(), 400, 490, 32, WHITE);
            else
                DrawText(("Score : " + to_string(score)).c_str(), 430, 490, 32, WHITE);
            if (!scoreUpdated) // to only update the score once. otherwise it'll keep repeating and add a million entries to the file
            {
                writeScore(score);
                scoreUpdated = true;

                thread uploadThread(uploadScores, playerName, score); // call in a separate thread to avoid the game freezing during the upload
                uploadThread.detach(); // thread will automatically terminate after upload is complete
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
            if (IsKeyPressed(KEY_M)) // go back to menu
            {
                downloadScores();
                gameOver = false;
                fps = 120;
                SetTargetFPS(fps);
                personalBest = getPersonalBest();
                updateHighScores();
                currentState = MENU;
                scoreUpdated = false;
            }
        }
        // Draw
        ClearBackground(RAYWHITE);
        EndDrawing(); // end of loop for one single frame
    }
    CloseWindow(); // Close window and OpenGL context
    return 0;
}

