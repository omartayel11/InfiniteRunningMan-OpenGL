// Includes
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <glut.h>
#include <stdio.h>
#include <windows.h>

// Define M_PI if it's not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Method Signatures
void Display(void);
void Anim(void);
void Init(void);
void DrawPlayer(void);
void DrawBoundaries(void);
void DrawBackground(void);
void DrawCircle(float x, float y, float radius, int segments);
void DrawTree(float x, float y);
void DrawCloud(float x, float y);
void DrawObstacle(void);
void HandleCollisions(void);
void HandleJump(void);
void DrawText(const char* text, float x, float y);
void DrawStartScreen(void);
void DrawGameOverScreen(void);
void DrawGameWonScreen(void);
void updateTimer(void);
void drawTimer(void);
void handleSpeedupWithTime(void);
void StartGame();
void RestartGame();
void DrawPowerUp(float x, float y, float size, int type);
void DrawCollectible(float x, float y, float radius);
void DrawText2(float x, float y, const char* text);
void DrawPowerUpTimer();
void DrawCircleOutline(float cx, float cy, float r, int num_segments);
void DrawCircle22(float cx, float cy, float r, int num_segments);
void PlaySoundEffect(const char* filename);
void PlaySoundEffect2(const char* filename);
void PlaySoundEffect3(const char* filename);
void PlaySoundEffect4(const char* filename);
void SetVolume(WORD volume);

// Global Variables
int windowWidth = 1200;
int windowHeight = 400;

// Player position, size, and movement
float playerX = 100;
float playerY = 50;
float playerWidth = 20;
float playerHeight = 50;
float jumpSpeed = 0;
bool isJumping = false;
bool isDucking = false;
float gravity = 0.00015;

float powerUpAngle = 0.0f; // Global variable for rotation angle
float collectibleAngle = 0.0f;

// Obstacle position and size
float obstacleX = windowWidth;
float obstacleY = 50;
float obstacleWidth = 30;
float obstacleHeight = 10 + rand()%40;
float obstacleSpeed = 0.1f;

// Second Obstacle position and size
float obstacle2X = windowWidth + 300;  // Starts off-screen to the right
float obstacle2Y = 90 + rand()%5;           // Set this to be above the first obstacle
float obstacle2Width = 30;
float obstacle2Height = 90 + rand()%20;
float obstacle2Speed = 0.1f;      // Same speed for consistency

float collectibleX = 300;      // X position of the collectible
float collectibleY = 200;      // Y position of the collectible
float collectibleWidth = 20;   // Width of the collectible
float collectibleHeight = 20;  // Height of the collectible
float collectibleSpeed = 0.1f; // Speed at which the collectible moves

float powerUpX = 1000;          // X position of the power-up
float powerUpY = 200;          // Y position of the power-up
float powerUpWidth = 30;       // Width of the power-up
float powerUpHeight = 30;      // Height of the power-up
float powerUpSpeed = 0.1f;     // Speed at which the power-up moves

int powerUpType = 0;          // 1 for invincibility, 2 for score multiplier
int powerUpDuration = 0;      // Timer for the power-up
bool invincible = false;
bool scoreMultiplier = false;

int playerScore = 0;           // Player's score

float defaultTime = 60.0f;

float remainingTime = defaultTime;  // Start with 60 seconds
int lastTime = 0;  // To track the time between frames

float timeElapsed = 0.0f;

// Game state variables
int playerLives = 5;

bool isFlashing = false;
int flashCounter = 0;
bool showElbes = false;
int elbesCounter = 0;

// Boundaries
float upperBoundaryY = windowHeight - 100;
float lowerBoundaryY = 40;

// Game state
bool gameStarted = false;
bool gameOver = false;
bool gameWon = false;

bool playerCollided = false;
float collisionTimer = 0.0f;



float lightningTimer = 0.0f; // Timer for lightning effect
const float lightningInterval = 3.0f; // Time in seconds between lightning strikes
bool lightningActive = false; // Whether lightning is currently active



// Main Function
int main(int argc, char** argv)
{

    srand(static_cast<unsigned int>(time(0)));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Infinite Runner Game");
    Init();

    glutDisplayFunc(Display);
    glutIdleFunc(Anim);
    SetVolume(0x3000);
    
    glutKeyboardFunc([](unsigned char key, int x, int y) {
        if (key == ' ' && !isJumping && gameStarted && !gameOver) {
            isJumping = true;
            jumpSpeed = 32; // Initial jump speed
        }
        if (key == 'd' && !gameOver) {
            // Check if the player can duck without going below the lower boundary
            if (playerY - playerHeight / 2 >= lowerBoundaryY - 30) {
                isDucking = true;
                playerY -= playerHeight / 2; // Move player down to duck
            }
        }
        });

    glutKeyboardUpFunc([](unsigned char key, int x, int y) {
        if (key == 'd') {
            isDucking = false;
            playerY += playerHeight / 2; // Move player up to stand

        }
        });

    glutMouseFunc([](int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            // Check if clicking the start button
            if (!gameStarted && !gameOver) {
                StartGame();
            }
            // Check if clicking the restart button
            if (gameOver) {
                // Restart the game by resetting all variables
                RestartGame();
            }
            if (gameWon) {
                RestartGame();
            }
        }
        });

    glutMainLoop();
    return 0;
}

// Initialization Function
void Init(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Background color
    gluOrtho2D(0.0, windowWidth, 0.0, windowHeight); // Set 2D orthographic projection
}

// Display Function
void Display(void)
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the screen

    DrawBackground();
    DrawBoundaries();

    if (gameStarted && !gameOver && !gameWon) {
        DrawPlayer();
        DrawObstacle();

        // Draw trees
        DrawTree(200, 50);
        DrawTree(400, 50);
        DrawTree(600, 50);
        DrawTree(800, 50);

        // Draw clouds
        DrawCloud(300, upperBoundaryY - 30);
        DrawCloud(800, upperBoundaryY - 50);

        DrawCollectible(collectibleX, collectibleY, collectibleWidth); // Call to the collectible drawing function
        DrawPowerUp(powerUpX, powerUpY, powerUpWidth / 2, powerUpType);
        DrawPowerUpTimer();


        // Handle collisions and game logic
        HandleCollisions();

        if (showElbes) {
            DrawText("elbes", playerX - 20, playerY + playerHeight + 40); // Display "elbes"
        }
    }
    else if (!gameStarted) {
        DrawStartScreen();
        PlaySoundEffect("../../Wii Music - Gaming Background Music (HD).wav");
    }
    else if (gameOver) {
        DrawGameOverScreen();
        PlaySoundEffect("../../Game Over sound effect.wav");
    }
    else if (gameWon) {
        DrawGameWonScreen();
        PlaySoundEffect("../../WIN sound effect no copyright.wav");
    }

    updateTimer();
    drawTimer();

    glutSwapBuffers();

    glFlush(); // Flush the drawing to the screen
}

void PlaySoundEffect(const char* filename) {
    PlaySoundA(filename, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NOSTOP);
}
void PlaySoundEffect2(const char* filename) {
    //PlaySoundA(filename, NULL, SND_FILENAME | SND_ASYNC);
    sndPlaySoundA(filename, SND_FILENAME | SND_ASYNC);

}
void PlaySoundEffect3(const char* filename) {
    PlaySoundA(filename, NULL, SND_FILENAME | SND_ASYNC);
}
void PlaySoundEffect4(const char* filename) {
    PlaySoundA(filename, NULL, SND_FILENAME | SND_ASYNC);
}
void SetVolume(WORD volume) {
    // Set the volume for both left and right channels
    waveOutSetVolume(0, volume | (volume << 16));
}


// Draw Player Function
void DrawPlayer(void)
{
    if (isFlashing && flashCounter % 20 < 10) {
        return; // Skip drawing player for flashing effect
    }

    // Adjust player height based on jumping or ducking state
    float adjustedHeight = isJumping ? playerHeight : (isDucking ? playerHeight / 2 : playerHeight);

    // Draw body
    glColor3f(0.0f, 0.0f, 1.0f); // Blue color for the body
    glBegin(GL_QUADS);
    glVertex2f(playerX, playerY);
    glVertex2f(playerX + playerWidth, playerY);
    glVertex2f(playerX + playerWidth, playerY + adjustedHeight);
    glVertex2f(playerX, playerY + adjustedHeight);
    glEnd();

    // Draw polygon (pentagon) on the body
    glColor3f(1.0f, 0.0f, 0.0f); // Green color for the polygon
    glBegin(GL_POLYGON);
    float radius = 10.0f; // Radius of the pentagon
    for (int i = 0; i < 5; i++) {
        float angle = 2.0f * M_PI * float(i) / 5.0f; // Calculate angle
        float x = playerX + playerWidth / 2 + radius * cos(angle); // X position
        float y = playerY + adjustedHeight / 2 + radius * sin(angle); // Y position
        glVertex2f(x, y);
    }
    glEnd();

    // Add lines to the body for a detailed look
    glColor3f(1.0f, 1.0f, 1.0f); // White color for lines
    glBegin(GL_LINES);
    // Vertical center line
    glVertex2f(playerX + playerWidth / 2, playerY);
    glVertex2f(playerX + playerWidth / 2, playerY + adjustedHeight);

    glVertex2f(playerX + playerWidth / 4, playerY);
    glVertex2f(playerX + playerWidth / 4, playerY + adjustedHeight);

    glVertex2f(playerX + 3 * playerWidth / 4, playerY);
    glVertex2f(playerX + 3 * playerWidth / 4, playerY + adjustedHeight);
    // Horizontal line at mid-height
    glVertex2f(playerX, playerY + adjustedHeight / 2);
    glVertex2f(playerX + playerWidth, playerY + adjustedHeight / 2);
    glEnd();

    // Draw head (circle)
    glColor3f(1.0f, 0.8f, 0.6f);
    DrawCircle(playerX + playerWidth / 2, playerY + adjustedHeight + 10, 10, 20);

    // Draw eyes and mouth
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for eyes
    DrawCircle(playerX + 4, playerY + adjustedHeight + 15, 2, 10); // Left eye
    DrawCircle(playerX + 16, playerY + adjustedHeight + 15, 2, 10); // Right eye

    // Draw mouth
    glBegin(GL_QUADS);
    glVertex2f(playerX + 5, playerY + adjustedHeight + 10);
    glVertex2f(playerX + 15, playerY + adjustedHeight + 10);
    glVertex2f(playerX + 15, playerY + adjustedHeight + 12);
    glVertex2f(playerX + 5, playerY + adjustedHeight + 12);
    glEnd();

    glColor3f(0.7f, 0.0f, 0.0f); // Red color for the hat
    // Hat base (rectangle)
    glBegin(GL_QUADS);
    glVertex2f(playerX + 2, playerY + adjustedHeight + 18); // Slightly above the head
    glVertex2f(playerX + playerWidth - 2, playerY + adjustedHeight + 18);
    glVertex2f(playerX + playerWidth - 2, playerY + adjustedHeight + 25);
    glVertex2f(playerX + 2, playerY + adjustedHeight + 25);
    glEnd();
    // Hat tip (triangle)
    glBegin(GL_TRIANGLES);
    glVertex2f(playerX + playerWidth / 2, playerY + adjustedHeight + 35); // Tip of the hat
    glVertex2f(playerX + 3, playerY + adjustedHeight + 25);
    glVertex2f(playerX + playerWidth - 3, playerY + adjustedHeight + 25);
    glEnd();

    // Add a dot on the tip of the hat
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color for the dot
    DrawCircle(playerX + playerWidth / 2, playerY + adjustedHeight + 35, 2, 10); // Small dot on hat tip

    // Draw arms
    glColor3f(0.8f, 0.5f, 0.3f); // Light brown color for arms
    float armWidth = playerWidth / 5; // Width of each arm
    float armHeight = playerHeight / 5; // Height of each arm

    // Calculate arm positions based on time for swinging motion
    float armOffset = sin(glutGet(GLUT_ELAPSED_TIME) * 0.005) * 5; // Oscillate arms

    // Left arm
    glBegin(GL_QUADS);
    glVertex2f(playerX - armWidth, playerY + (adjustedHeight - 10) + armOffset); // Positioning based on offset
    glVertex2f(playerX, playerY + (adjustedHeight - 10) + armOffset);
    glVertex2f(playerX, playerY + (adjustedHeight - 10) + armHeight + armOffset);
    glVertex2f(playerX - armWidth, playerY + (adjustedHeight - 10) + armHeight + armOffset);
    glEnd();

    // Right arm
    glBegin(GL_QUADS);
    glVertex2f(playerX + playerWidth, playerY + (adjustedHeight - 10) - armOffset); // Positioning based on offset
    glVertex2f(playerX + playerWidth + armWidth, playerY + (adjustedHeight - 10) - armOffset);
    glVertex2f(playerX + playerWidth + armWidth, playerY + (adjustedHeight - 10) + armHeight - armOffset);
    glVertex2f(playerX + playerWidth, playerY + (adjustedHeight - 10) + armHeight - armOffset);
    glEnd();

    // Draw legs
    glColor3f(0.8f, 0.5f, 0.3f); // Light brown color for legs
    float legWidth = playerWidth / 4; // Width of each leg
    float legHeight = playerHeight / 2.3; // Height of each leg

    // Calculate leg positions based on time for running motion
    float legOffset = sin(glutGet(GLUT_ELAPSED_TIME) * 0.01) * 10; // Oscillate legs

    // Draw left leg
    glBegin(GL_QUADS);
    glVertex2f(playerX + playerWidth / 4, playerY); // Left leg normal
    glVertex2f(playerX + playerWidth / 4 + legWidth, playerY);
    glVertex2f(playerX + playerWidth / 4 + legWidth, playerY - legHeight + legOffset); // Add offset
    glVertex2f(playerX + playerWidth / 4, playerY - legHeight + legOffset);
    glEnd();

    // Draw right leg
    glBegin(GL_QUADS);
    glVertex2f(playerX + 3 * playerWidth / 4 - legWidth, playerY - legHeight + legOffset * 0.8); // Right leg raised
    glVertex2f(playerX + 3 * playerWidth / 4, playerY - legHeight + legOffset * 0.8);
    glVertex2f(playerX + 3 * playerWidth / 4, playerY);
    glVertex2f(playerX + 3 * playerWidth / 4 - legWidth, playerY);
    glEnd();
}

void drawTimer() {

    if (!gameStarted || gameOver) {
        return;
    }

    char timeStr[20];  // Buffer for storing formatted time
    int minutes = (int)remainingTime / 60;  // Get minutes
    int seconds = (int)remainingTime % 60;  // Get seconds

    // Manually build the time string as "Time: MM:SS"
    timeStr[0] = 'T';
    timeStr[1] = 'i';
    timeStr[2] = 'm';
    timeStr[3] = 'e';
    timeStr[4] = ' ';
    timeStr[5] = ':';
    timeStr[6] = ' ';
    timeStr[7] = '0' + (minutes / 10);  // Tens place of minutes
    timeStr[8] = '0' + (minutes % 10);  // Ones place of minutes
    timeStr[9] = ':';
    timeStr[10] = '0' + (seconds / 10);  // Tens place of seconds
    timeStr[11] = '0' + (seconds % 10);  // Ones place of seconds
    timeStr[12] = '\0';  // Null-terminator for the string

    // Set the text color to an attractive color (e.g., light blue)
    glColor3f(0.1f, 0.6f, 0.9f);

    // Set the position for the timer at the top-right corner (adjust coordinates accordingly)
    glRasterPos2f(windowWidth - 150, windowHeight - 30);

    // Optionally, scale the text size if needed (adjust scale values)
    glPushMatrix();  // Save the current matrix
    glScalef(0.2f, 0.2f, 1.0f);  // Scale the text down (experiment with values to get the right size)

    // Draw the timer text
    for (const char* c = timeStr; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);  // Use a clean font for readability
    }

    glPopMatrix();  // Restore the original matrix

    //------------------------------
    // Display the player's score
    char scoreText[50]; // Buffer to hold the score text
    // Convert the score to a string manually
    int score = playerScore; // Assuming playerScore is an integer
    int i = 0;
    if (score == 0) {
        scoreText[i++] = '0'; // Handle score 0
    }
    else {
        while (score > 0) {
            scoreText[i++] = (score % 10) + '0'; // Get the last digit
            score /= 10; // Remove the last digit
        }
    }
    scoreText[i] = '\0'; // Null-terminate the string

    // Reverse the score text to display it correctly
    for (int j = 0; j < i / 2; j++) {
        char temp = scoreText[j];
        scoreText[j] = scoreText[i - j - 1];
        scoreText[i - j - 1] = temp;
    }

    DrawText("Your Score: ", windowWidth - 350, windowHeight - 30);
    DrawText(scoreText, windowWidth - 225, windowHeight - 30); // Adjust position as needed

    //----------------------------------------------
    // Draw the player's health bar
    float maxHealth = 5.0f;  // The total health value
    float currentHealth = (float)playerLives;  // Use playerLives to track the current health
    float healthBarWidth = 200.0f;  // The total width of the health bar
    float healthBarHeight = 20.0f;  // The height of the health bar
    float filledHealthWidth = (currentHealth / maxHealth) * healthBarWidth;  // Width proportional to remaining health

    // Set the health bar color (green for healthy)
    glColor3f(0.0f, 0.8f, 0.0f);  // Green color for the filled health

    // Draw the filled portion of the health bar
    glBegin(GL_QUADS);
    glVertex2f(windowWidth - 400, windowHeight - 60);  // Bottom-left corner
    glVertex2f(windowWidth - 400 + filledHealthWidth, windowHeight - 60);  // Bottom-right corner
    glVertex2f(windowWidth - 400 + filledHealthWidth, windowHeight - 60 + healthBarHeight);  // Top-right corner
    glVertex2f(windowWidth - 400, windowHeight - 60 + healthBarHeight);  // Top-left corner
    glEnd();

    // Set the outline color for the health bar (black)
    glColor3f(0.0f, 0.0f, 0.0f);

    // Draw the outline of the health bar
    glBegin(GL_LINE_LOOP);
    glVertex2f(windowWidth - 400, windowHeight - 60);  // Bottom-left corner
    glVertex2f(windowWidth - 400 + healthBarWidth, windowHeight - 60);  // Bottom-right corner
    glVertex2f(windowWidth - 400 + healthBarWidth, windowHeight - 60 + healthBarHeight);  // Top-right corner
    glVertex2f(windowWidth - 400, windowHeight - 60 + healthBarHeight);  // Top-left corner
    glEnd();

    //----------------------------------------------
}





void updateTimer() {
    if (!gameStarted || gameOver) {
        return;
    }
    int currentTime = glutGet(GLUT_ELAPSED_TIME);  // Get elapsed time since the program started in milliseconds
    int deltaTime = currentTime - lastTime;  // Calculate time passed since last update
    timeElapsed += deltaTime;
    if (deltaTime > 1000) {  // Update every second
        remainingTime -= deltaTime / 1000.0f;  // Reduce remaining time
        lastTime = currentTime;  // Update lastTime
    }
    if (remainingTime <= 0) {
        remainingTime = 0;  // Ensure the timer doesn't go below 0
        gameWon = true;
    }

}



// Draw Obstacle Function
void DrawObstacle(void)
{
    // Draw first obstacle (Curvy Cactus with Details)
    glColor3f(0.0f, 0.7f, 0.0f); // Bright green color for the cactus base

    // Draw cactus body
    glBegin(GL_QUADS);
    glVertex2f(obstacleX + obstacleWidth / 4, obstacleY);
    glVertex2f(obstacleX + 3 * obstacleWidth / 4, obstacleY);
    glVertex2f(obstacleX + 3 * obstacleWidth / 4, obstacleY + obstacleHeight);
    glVertex2f(obstacleX + obstacleWidth / 4, obstacleY + obstacleHeight);
    glEnd();

    // Draw details on the cactus body (lines for texture)
    glColor3f(0.0f, 0.4f, 0.0f); // Darker green for the texture
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(obstacleX + obstacleWidth / 4 + (obstacleWidth / 4) * (i + 1) / 4, obstacleY + obstacleHeight);
        glVertex2f(obstacleX + obstacleWidth / 4 + (obstacleWidth / 4) * (i + 1) / 4, obstacleY);
        glEnd();
    }

    // Draw cactus arms (using quads)
    glColor3f(0.0f, 1.7f, 0.0f); // Bright green for arms
    // Left arm
    glBegin(GL_QUADS);
    glVertex2f(obstacleX + obstacleWidth / 4 - 15, obstacleY + obstacleHeight / 2);
    glVertex2f(obstacleX + obstacleWidth / 4, obstacleY + obstacleHeight / 2);
    glVertex2f(obstacleX + obstacleWidth / 4, obstacleY + obstacleHeight / 2 + 40);
    glVertex2f(obstacleX + obstacleWidth / 4 - 15, obstacleY + obstacleHeight / 2 + 40);
    glEnd();

    // Right arm
    glBegin(GL_QUADS);
    glVertex2f(obstacleX + 3 * obstacleWidth / 4 + 15, obstacleY + obstacleHeight / 2);
    glVertex2f(obstacleX + 3 * obstacleWidth / 4, obstacleY + obstacleHeight / 2);
    glVertex2f(obstacleX + 3 * obstacleWidth / 4, obstacleY + obstacleHeight / 2 + 40);
    glVertex2f(obstacleX + 3 * obstacleWidth / 4 + 15, obstacleY + obstacleHeight / 2 + 40);
    glEnd();

    // Add texture lines for arms
    glColor3f(0.0f, 0.4f, 0.0f);
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(obstacleX + obstacleWidth / 4 - 15, obstacleY + obstacleHeight / 2 + (10 * i));
        glVertex2f(obstacleX + obstacleWidth / 4, obstacleY + obstacleHeight / 2 + (10 * i));
        glEnd();
    }

    // Draw second obstacle (Enhanced Dirt Block with Rocks)
    glColor3f(0.8f, 0.52f, 0.25f); // Brown color for the dirt block
    glBegin(GL_QUADS);
    glVertex2f(obstacle2X, obstacle2Y);
    glVertex2f(obstacle2X + obstacle2Width, obstacle2Y);
    glVertex2f(obstacle2X + obstacle2Width, obstacle2Y + obstacle2Height);
    glVertex2f(obstacle2X, obstacle2Y + obstacle2Height);
    glEnd();

    // Add texture for the dirt block (horizontal lines)
    glColor3f(0.5f, 0.35f, 0.2f); // Darker brown for texture
    for (int i = 0; i < 5; i++) {
        glBegin(GL_LINES);
        glVertex2f(obstacle2X, obstacle2Y + (i * obstacle2Height / 5));
        glVertex2f(obstacle2X + obstacle2Width, obstacle2Y + (i * obstacle2Height / 5));
        glEnd();
    }

    // Draw rocks on top of the dirt block
    glColor3f(0.5f, 0.5f, 0.5f); // Gray color for rocks

    // Larger rock
    glBegin(GL_POLYGON);
    for (int i = 0; i < 8; i++) {
        float angle = 2.0f * M_PI * float(i) / 8.0f; // Octagon shape
        glVertex2f(obstacle2X + obstacle2Width / 2 + 15 * cos(angle), obstacle2Y + obstacle2Height + 15 * sin(angle));
    }
    glEnd();

    // Smaller rock
    glBegin(GL_POLYGON);
    for (int i = 0; i < 6; i++) {
        float angle = 2.0f * M_PI * float(i) / 6.0f; // Hexagon shape
        glVertex2f(obstacle2X + 3 * obstacle2Width / 4 + 8 * cos(angle), obstacle2Y + obstacle2Height + 8 * sin(angle));
    }
    glEnd();

    // Additional small rocks for detail
    glColor3f(0.7f, 0.7f, 0.7f); // Lighter gray for small rocks
    glBegin(GL_POLYGON);
    for (int i = 0; i < 5; i++) {
        float angle = 2.0f * M_PI * float(i) / 5.0f; // Pentagon shape
        glVertex2f(obstacle2X + 10 + 5 * cos(angle), obstacle2Y + obstacle2Height + 5 * sin(angle));
    }
    glEnd();
}



void DrawCollectible(float x, float y, float radius)
{
    int numSegments = 40; // Number of segments for smoother circle
    float angleStep = 2.0f * M_PI / numSegments;

    float scaleFactor = 1.0f + 0.2f * sin(glutGet(GLUT_ELAPSED_TIME) / 200.0f);
    glPushMatrix(); // Save the current state of the modelview matrix
    glTranslatef(x, y, 0); // Move to the collectible's position
    glScalef(scaleFactor, scaleFactor, 1.0f);// Rotate around Y-axis (spinning effect)

    // Outer shiny ring using a gradient effect
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 0.9f, 0.0f); // Bright gold at the center
    glVertex2f(0.0f, 0.0f); // Center of the coin

    for (int i = 0; i <= numSegments; i++)
    {
        float angle = i * angleStep;
        float dx = cos(angle) * radius;
        float dy = sin(angle) * radius;

        // Gradient effect based on the segment position
        glColor3f(0.9f + 0.1f * (i % 2), 0.7f + 0.1f * (i % 2), 0.0f); // Alternating light gold
        glVertex2f(dx, dy);
    }
    glEnd();

    // Inner circle with polygonal detail
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.84f, 0.0f); // Slightly darker gold
    float innerRadius = radius * 0.8f;

    for (int i = 0; i < 6; i++) // Hexagon shape
    {
        float angle = i * M_PI / 3.0f; // 60-degree segments
        float dx = cos(angle) * innerRadius;
        float dy = sin(angle) * innerRadius;
        glVertex2f(dx, dy);
    }
    glEnd();

    // Star-like shine using lines
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 0.8f); // Shiny white for sparkle
    for (int i = 0; i < 8; i++) // Star with 8 rays
    {
        float angle = i * M_PI / 4.0f; // 45-degree increments
        glVertex2f(0.0f, 0.0f); // Start from the center
        glVertex2f(cos(angle) * radius * 0.7f, sin(angle) * radius * 0.7f); // Extend outward
    }
    glEnd();

    // Outer sparkle points
    glPointSize(5.0f); // Increase point size for sparkle effect
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f); // White sparkle
    for (int i = 0; i < 5; i++)
    {
        float angle = i * M_PI / 2.5f; // 72-degree increments for points
        float dx = cos(angle) * radius * 1.1f;
        float dy = sin(angle) * radius * 1.1f;
        glVertex2f(dx, dy);
    }
    glEnd();

    glPopMatrix(); // Restore the modelview matrix
}



void DrawPowerUp(float x, float y, float size, int type)
{
    glPushMatrix(); // Save the current state of the modelview matrix
    glTranslatef(x, y, 0); // Move to the power-up's position
    glRotatef(powerUpAngle, 0.0f, 0.0f, 1.0f); // Rotate around the Z-axis for spinning effect

    if (type == 1) {
        // Draw invincibility power-up as a star using triangles and lines
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f, 1.0f, 0.0f); // Bright yellow for the star
        glVertex2f(0.0f, 0.0f); // Center of the star

        for (int i = 0; i < 5; i++) {
            float outerAngle = i * 2.0f * M_PI / 5.0f; // Outer points of the star
            float innerAngle = outerAngle + M_PI / 5.0f; // Inner points of the star

            // Outer point
            glVertex2f(cos(outerAngle) * size, sin(outerAngle) * size);
            // Inner point
            glVertex2f(cos(innerAngle) * (size * 0.5f), sin(innerAngle) * (size * 0.5f));
        }
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(1.0f, 0.5f, 0.0f); // Orange for the quad
        glVertex2f(-size * 0.5f, -size * 0.5f);
        glVertex2f(size * 0.5f, -size * 0.5f);
        glVertex2f(size * 0.5f, size * 0.5f);
        glVertex2f(-size * 0.5f, size * 0.5f);
        glEnd();

        // Add glowing outline using lines
        glBegin(GL_LINE_LOOP);
        glColor3f(1.0f, 1.0f, 0.2f); // Light yellow for glow effect
        for (int i = 0; i < 5; i++) {
            float outerAngle = i * 2.0f * M_PI / 5.0f;
            glVertex2f(cos(outerAngle) * size, sin(outerAngle) * size);
        }
        glEnd();

        // Add the edges using line strips for a cleaner look
        glBegin(GL_LINE_STRIP);
        glColor3f(1.0f, 0.8f, 0.0f); // A slightly darker shade for depth
        for (int i = 0; i <= 5; i++) {
            float angle = i * 2.0f * M_PI / 5.0f; // Create a closed shape
            glVertex2f(cos(angle) * (size + 5.0f), sin(angle) * (size + 5.0f)); // Slightly larger for effect
        }
        glEnd();
        // Add sparkle points for both power-ups to make them stand out
        glPointSize(4.0f); // Increase point size for sparkles
        glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 1.0f); // White sparkle
        for (int i = 0; i < 6; i++) {
            float angle = i * M_PI / 3.0f; // Points around the power-up
            float dx = cos(angle) * (size * 1.2f);
            float dy = sin(angle) * (size * 1.2f);
            glVertex2f(dx, dy); // Place the sparkle around the power-up
        }
        glEnd();
    }
    else if (type == 2) {
        // Draw score multiplier power-up as a diamond using quads and lines
        glBegin(GL_QUADS);
        glColor3f(0.0f, 1.0f, 0.0f); // Green for score multiplier
        glVertex2f(0.0f, size);  // Top point
        glVertex2f(-size, 0.0f); // Left point
        glVertex2f(0.0f, -size); // Bottom point
        glVertex2f(size, 0.0f);  // Right point
        glEnd();

        glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.0f, 1.0f); // Blue for the triangle
        glVertex2f(0.0f, size * 1.2f); // Top vertex
        glVertex2f(-size * 0.6f, -size * 0.6f); // Bottom left vertex
        glVertex2f(size * 0.6f, -size * 0.6f); // Bottom right vertex
        glEnd();

        // Add glowing outline using lines
        glBegin(GL_LINE_LOOP);
        glColor3f(0.3f, 1.0f, 0.3f); // Light green for glow effect
        glVertex2f(0.0f, size);  // Top point
        glVertex2f(-size, 0.0f); // Left point
        glVertex2f(0.0f, -size); // Bottom point
        glVertex2f(size, 0.0f);  // Right point
        glEnd();

        // Adding additional edge using line strip for a more polished look
        glBegin(GL_LINE_STRIP);
        glColor3f(0.0f, 0.5f, 0.0f); // Darker green for depth
        glVertex2f(0.0f, size);  // Top point
        glVertex2f(-size, 0.0f); // Left point
        glVertex2f(0.0f, -size); // Bottom point
        glVertex2f(size, 0.0f);  // Right point
        glVertex2f(0.0f, size);  // Closing the diamond shape
        glEnd();
        // Add sparkle points for both power-ups to make them stand out
        glPointSize(4.0f); // Increase point size for sparkles
        glBegin(GL_POINTS);
        glColor3f(1.0f, 1.0f, 1.0f); // White sparkle
        for (int i = 0; i < 6; i++) {
            float angle = i * M_PI / 3.0f; // Points around the power-up
            float dx = cos(angle) * (size * 1.2f);
            float dy = sin(angle) * (size * 1.2f);
            glVertex2f(dx, dy); // Place the sparkle around the power-up
        }
        glEnd();
    }

    

    glPopMatrix(); // Restore the previous state of the modelview matrix
}



void DrawPowerUpTimer() {
    if (invincible) {
        char invincibilityTime[50];
        sprintf(invincibilityTime, "Invincible: %d", powerUpDuration);
        DrawText2(10, 10, invincibilityTime);
    }
    else if (scoreMultiplier) {
        char multiplierTime[50];
        sprintf(multiplierTime, "Multiplier: %d", powerUpDuration);
        DrawText2(10, 10, multiplierTime);
    }
}

void DrawText2(float x, float y, const char* text)
{
    glRasterPos2f(x, y);
    while (*text)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }
}

void handleSpeedupWithTime() {
    // Calculate the elapsed time since the game started
    int elapsedTime = defaultTime - remainingTime;

    // Define speed increments
    float speedIncrement = 0.005f; // Base speed increment
    float maxSpeed = 1.7f; // Maximum speed cap

    // Update the obstacle speed based on elapsed time
    obstacleSpeed = 0.1f + (elapsedTime * speedIncrement);
    obstacle2Speed = obstacleSpeed; // Keep both obstacle speeds synchronized
    powerUpSpeed = obstacleSpeed;
    collectibleSpeed = obstacleSpeed;

    // Cap the speed to maximum allowed
    if (obstacleSpeed > maxSpeed) {
        obstacleSpeed = maxSpeed;
        obstacle2Speed = maxSpeed;
    }
}



// Collision Detection and Game Logic
void HandleCollisions(void)
{
    handleSpeedupWithTime();
    if (playerCollided)
        collisionTimer += 0.1;

    if (collisionTimer >= 50) {
        playerCollided = false;
        collisionTimer = 0;
    }

   
    // Move obstacles and power-ups
    if (!playerCollided) {
        obstacleX -= obstacleSpeed;
        obstacle2X -= obstacle2Speed;
        collectibleX -= collectibleSpeed;
        powerUpX -= powerUpSpeed;
    }

    if (playerCollided) {
        obstacleX += obstacleSpeed;
        obstacle2X += obstacle2Speed;
        collectibleX += collectibleSpeed;
        powerUpX += powerUpSpeed;
    }

    // Reset obstacle positions when off-screen
    if (obstacleX + obstacleWidth < 0) {
        obstacleX = windowWidth;
    }
    if (obstacle2X + obstacleWidth < 0) {

        obstacle2X = windowWidth + (windowWidth / 2);
    }

    // Reset collectible and power-up positions when off-screen
    if (collectibleX + collectibleWidth < 0) {
        collectibleX = windowWidth + rand() % 100 + 50;
        collectibleY = 200;
    }
    if (powerUpX + powerUpWidth < 0) {
        powerUpX = windowWidth + rand() % 150 + 600;
        powerUpY = 200;
        powerUpType = rand() % 2 + 1; // Randomize power-up type: 1 for invincibility, 2 for score multiplier
    }

    // Collision detection for obstacles (only if not invincible)
    if (!invincible) {
        if (playerX + playerWidth > obstacleX && playerX < obstacleX + obstacleWidth &&
            playerY < obstacleY + obstacleHeight && playerY + playerHeight > obstacleY) {

            playerCollided = true;
            //PlaySoundEffect("../../game-start-6104.wav");
            if (!isFlashing) {
                playerLives--;
                printf("Player lives after collision: %d\n", playerLives);
                isFlashing = true;
                flashCounter = 0;
                showElbes = true;
                elbesCounter = 0;

                if (playerLives == 0) {
                    gameOver = true;
                }
            }
            PlaySoundEffect4("../../Bruh (Sound Effect) 1 second video!.wav");
        }

        if (playerX + playerWidth > obstacle2X && playerX < obstacle2X + obstacle2Width &&
            playerY < obstacle2Y + obstacle2Height && playerY + playerHeight > obstacle2Y) {

            playerCollided = true;
            //PlaySoundEffect("../../game-start-6104.wav");
            if (!isFlashing) {
                playerLives--;
                printf("Player lives after collision: %d\n", playerLives);
                isFlashing = true;
                flashCounter = 0;
                showElbes = true;
                elbesCounter = 0;

                if (playerLives == 0) {
                    gameOver = true;
                }
            }
            PlaySoundEffect4("../../Bruh (Sound Effect) 1 second video!.wav");
        }
    }

    // Collision detection for collectibles
    if (playerX + playerWidth > collectibleX && playerX < collectibleX + collectibleWidth &&
        playerY < collectibleY + collectibleHeight && playerY + playerHeight > collectibleY) {
        
        playerScore += scoreMultiplier ? 200 : 100; // Double score if scoreMultiplier is active
        collectibleX = -50; // Move off-screen until reset
        //SetVolume(0x5000);
        PlaySoundEffect2("../../Retro Game Coin Sound Effect.wav");
        
        
    }

    // Collision detection for power-ups
    if (playerX + playerWidth > powerUpX && playerX < powerUpX + powerUpWidth &&
        playerY < powerUpY + powerUpHeight && playerY + playerHeight > powerUpY) {

        if (powerUpType == 1) {
            invincible = true;
        }
        else if (powerUpType == 2) {
            scoreMultiplier = true;
        }

        powerUpDuration = 5000; // Lasts for 5 seconds
        powerUpX = -50; // Move off-screen until reset
        PlaySoundEffect3("../../MARIO POWER UP - SOUND EFFECT.wav");
    }

    // Handle flashing effect after collision
    if (isFlashing) {
        flashCounter++;
        if (flashCounter > 40) {
            isFlashing = false;
        }
    }

    // Handle "elbes" display after collision
    if (showElbes) {
        elbesCounter++;
        if (elbesCounter > 1000) {
            showElbes = false;
        }
    }

    // Manage power-up durations
    if (invincible || scoreMultiplier) {
        powerUpDuration--;
        if (powerUpDuration <= 0) {
            invincible = false;
            scoreMultiplier = false;
        }
    }
}


// Jump and gravity logic
void HandleJump()
{
    if (isJumping) {
        // Update player's vertical position based on jump speed
        playerY += jumpSpeed;

        // Check if the player is ascending
        if (jumpSpeed > 0) {
            jumpSpeed -= gravity; // Apply gravity to slow the jump

            // Check if the player exceeds the upper boundary
            if (playerY + playerHeight >= upperBoundaryY - 20) { // Ensure height is below the upper boundary
                playerY = upperBoundaryY - playerHeight - 60; // Adjust to just below the upper boundary
                jumpSpeed = 0; // Stop jump speed
            }
        }
        else {
            // Descending phase
            jumpSpeed -= gravity; // Continue applying gravity
        }

        // Check if player has hit the ground
        if (playerY <= 50) { // Reset if below ground level
            playerY = 50; // Reset to ground level
            isJumping = false; // End jump
            jumpSpeed = 0; // Reset jump speed
        }
    }
}

// Draw Boundaries Function
void DrawBoundaries(void)
{
    // Upper boundary (gradient rectangle)
    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.7f, 0.3f); // Light brown top
    glVertex2f(0, upperBoundaryY);
    glVertex2f(windowWidth, upperBoundaryY);
    glColor3f(0.5f, 0.4f, 0.1f); // Darker brown bottom
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    // Lower boundary (gradient ground)
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.15f, 0.05f); // Lighter brown top
    glVertex2f(0, lowerBoundaryY);
    glVertex2f(windowWidth, lowerBoundaryY);
    glColor3f(0.2f, 0.1f, 0.05f); // Darker brown bottom
    glVertex2f(windowWidth, 0);
    glVertex2f(0, 0);
    glEnd();

    // Add a smooth transition line at the top of the lower boundary
    glLineWidth(4.0f);
    glColor3f(0.4f, 0.2f, 0.1f); // Mid-tone brown for outline
    glBegin(GL_LINES);
    glVertex2f(0, lowerBoundaryY);
    glVertex2f(windowWidth, lowerBoundaryY);
    glEnd();

    // Add small decorative triangles (grass-like) along the lower boundary
    for (float i = 0; i < windowWidth; i += 50.0f)
    {
        glColor3f(0.2f, 0.7f, 0.3f); // Green grass color
        glBegin(GL_TRIANGLES);
        glVertex2f(i, lowerBoundaryY);
        glVertex2f(i + 10, lowerBoundaryY + 20); // Point of the triangle
        glVertex2f(i + 20, lowerBoundaryY);
        glEnd();
    }

    // Add a series of arcs to the upper boundary (cloud-like shapes)
    for (float i = 50; i < windowWidth; i += 100.0f)
    {
        glColor3f(0.9f, 0.9f, 0.9f); // Light gray for the arcs
        DrawCircle22(i, upperBoundaryY + 20, 15, 50); // Cloud arc
    }

    // Thin line for extra detailing on the upper boundary
    glLineWidth(2.0f);
    glColor3f(0.6f, 0.5f, 0.2f); // Mid-tone brown
    glBegin(GL_LINES);
    glVertex2f(0, upperBoundaryY);
    glVertex2f(windowWidth, upperBoundaryY);
    glEnd();

    glPointSize(5.0f); // Set point size
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for points
    glBegin(GL_POINTS);
    glVertex2f(0, lowerBoundaryY); // Bottom-left corner
    glVertex2f(windowWidth, lowerBoundaryY); // Bottom-right corner
    glVertex2f(0, upperBoundaryY); // Top-left corner
    glVertex2f(windowWidth, upperBoundaryY); // Top-right corner
    glEnd();
}

// Helper function to draw a circle
void DrawCircle22(float cx, float cy, float r, int num_segments)
{
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(num_segments); // Angle
        float dx = r * cosf(theta); // X component
        float dy = r * sinf(theta); // Y component
        glVertex2f(cx + dx, cy + dy);
    }
    glEnd();
}


// Draw Background Function

void DrawBackground(void)
{
    // Define global variables for raindrops
    const int numRaindrops = 100; // Number of raindrops
    static float rainX[numRaindrops]; // X positions of raindrops
    static float rainY[numRaindrops]; // Y positions of raindrops
    float rainLength = 10.0f; // Length of each raindrop
    float rainSpeed = 0.1f; // Speed of falling rain

    // Initialize raindrop positions randomly (only on the first call)
    static bool initialized = false; // Flag to check if initialized
    if (!initialized) {
        for (int i = 0; i < numRaindrops; i++) {
            rainX[i] = rand() % windowWidth; // Random X position
            rainY[i] = rand() % windowHeight; // Random starting Y position
        }
        initialized = true; // Set the flag to true after initialization
    }

    // Draw a gradient background
    for (int i = 0; i < windowHeight; i++) {
        float colorValue = (float)i / windowHeight; // Calculate gradient color
        glColor3f(colorValue, colorValue, 1.0f); // Blue gradient
        glBegin(GL_LINES);
        glVertex2f(0, i);
        glVertex2f(windowWidth, i);
        glEnd();
    }

    // Draw rain effect
    glColor3f(1.0f, 1.0f, 1.0f); // White color for rain

    // Update positions of raindrops
    for (int i = 0; i < numRaindrops; i++) {
        rainY[i] -= rainSpeed; // Move raindrop down

        // Reset raindrop position when it goes off screen
        if (rainY[i] < 0) {
            rainY[i] = windowHeight; // Reset to the top
            rainX[i] = rand() % windowWidth; // Random new X position
        }

        // Draw each raindrop as a line
        glBegin(GL_LINES);
        glVertex2f(rainX[i], rainY[i]); // Start point of the raindrop
        glVertex2f(rainX[i], rainY[i] - rainLength); // End point of the raindrop
        glEnd();
    }
}




// Draw Circle Function
void DrawCircle(float x, float y, float radius, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center of the circle
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float dx = radius * cosf(angle);
        float dy = radius * sinf(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

// Draw Tree Function
void DrawTree(float x, float y)
{
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw trunk with a gradient for better appearance
    glBegin(GL_QUADS);
    glColor3f(0.4f, 0.2f, 0.05f); // Darker brown at the bottom
    glVertex2f(x, y);
    glVertex2f(x + 10, y);
    glColor3f(0.6f, 0.3f, 0.1f); // Lighter brown at the top
    glVertex2f(x + 10, y + 30);
    glVertex2f(x, y + 30);
    glEnd();

    // Outline for the trunk
    glColor3f(0.0f, 0.0f, 0.0f); // Black outline
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + 10, y);
    glVertex2f(x + 10, y + 30);
    glVertex2f(x, y + 30);
    glEnd();

    // Draw transparent leaves
    glColor4f(0.0f, 0.8f, 0.0f, 0.6f); // Green with transparency
    DrawCircle(x + 5, y + 40, 20, 20); // Main leafy part

    // Draw the outline for the leaves
    glColor3f(0.0f, 0.3f, 0.0f); // Darker green for the outline
    glLineWidth(2.0f);
    DrawCircleOutline(x + 5, y + 40, 20, 20);

    // Disable blending after we're done
    glDisable(GL_BLEND);
}

// Helper function to draw an outlined circle
void DrawCircleOutline(float cx, float cy, float r, int num_segments)
{
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(num_segments);
        float dx = r * cosf(theta); // Calculate the x component
        float dy = r * sinf(theta); // Calculate the y component
        glVertex2f(cx + dx, cy + dy);
    }
    glEnd();
}


// Draw Cloud Function
void DrawCloud(float x, float y)
{
    // Set the color for the cloud
    glColor3f(1.0f, 1.0f, 1.0f); // White color

    // Draw the base cloud shape
    DrawCircle(x, y, 15, 10);
    DrawCircle(x + 20, y, 15, 10);
    DrawCircle(x + 10, y + 15, 15, 10);

    // Draw lightning effect if active
    if (lightningActive) {
        glColor3f(1.0f, 1.0f, 0.8f); // Brighten the clouds to simulate lightning
        DrawCircle(x, y, 15, 10);
        DrawCircle(x + 20, y, 15, 10);
        DrawCircle(x + 10, y + 15, 15, 10);
    }
}

// Draw Start Screen
void DrawStartScreen(void)
{
    DrawText("Welcome to the Infinite Runner Game!", 250, windowHeight / 2 + 20);
    DrawText("Click to Start", 450, windowHeight / 2 - 20);
}

// Draw Game Over Screen
void DrawGameOverScreen(void)
{
    DrawText("Game Over!", 500, windowHeight / 2 + 20);
    DrawText("Click to Restart", 450, windowHeight / 2 - 20);
    // Display the player's score
    char scoreText[50]; // Buffer to hold the score text
    // Convert the score to a string manually
    int score = playerScore; // Assuming playerScore is an integer
    int i = 0;
    if (score == 0) {
        scoreText[i++] = '0'; // Handle score 0
    }
    else {
        while (score > 0) {
            scoreText[i++] = (score % 10) + '0'; // Get the last digit
            score /= 10; // Remove the last digit
        }
    }
    scoreText[i] = '\0'; // Null-terminate the string

    // Reverse the score text to display it correctly
    for (int j = 0; j < i / 2; j++) {
        char temp = scoreText[j];
        scoreText[j] = scoreText[i - j - 1];
        scoreText[i - j - 1] = temp;
    }

    DrawText("Your Score: ", 450, windowHeight / 2 - 60);
    DrawText(scoreText, 600, windowHeight / 2 - 60); // Adjust position as needed
    
}

void DrawGameWonScreen(void) {
    if (gameWon) {
        DrawText("Congratulations, you survived! Game Won", 500, windowHeight / 2 + 20);
        DrawText("Click to Restart", 450, windowHeight / 2 - 20);

        // Display the player's score
        char scoreText[50]; // Buffer to hold the score text
        // Convert the score to a string manually
        int score = playerScore; // Assuming playerScore is an integer
        int i = 0;
        if (score == 0) {
            scoreText[i++] = '0'; // Handle score 0
        }
        else {
            while (score > 0) {
                scoreText[i++] = (score % 10) + '0'; // Get the last digit
                score /= 10; // Remove the last digit
            }
        }
        scoreText[i] = '\0'; // Null-terminate the string

        // Reverse the score text to display it correctly
        for (int j = 0; j < i / 2; j++) {
            char temp = scoreText[j];
            scoreText[j] = scoreText[i - j - 1];
            scoreText[i - j - 1] = temp;
        }

        DrawText("Your Score: ", 450, windowHeight / 2 - 60);
        DrawText(scoreText, 600, windowHeight / 2 - 60); // Adjust position as needed
        
    }
}


void DrawLightning(float startX, float startY, float endX, float endY)
{
    glColor3f(1.0f, 1.0f, 0.8f); // Bright color for lightning

    glBegin(GL_LINE_STRIP);
    glVertex2f(startX, startY); // Start point of the lightning

    // Create a zigzag effect for the lightning bolt
    for (int i = 1; i <= 4; i++) {
        float offsetX = (rand() % 20 - 10) / 10.0f; // Random horizontal offset
        float offsetY = i % 2 == 0 ? -15.0f : 15.0f; // Alternating vertical offset

        // Calculate intermediate points
        float x = startX + (endX - startX) * (i / 4.0f) + offsetX;
        float y = startY + (endY - startY) * (i / 4.0f) + offsetY;

        glVertex2f(x, y); // Intermediate point of the lightning
    }

    glVertex2f(endX, endY); // End point of the lightning
    glEnd();
}


// Draw Text Function
void DrawText(const char* text, float x, float y)
{
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for text
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Start Game Function
void StartGame() {
    playerLives = 5; // Reset lives
    playerScore = 0;
    remainingTime = 60.0f; // Reset remaining time to 60 seconds
    lastTime = glutGet(GLUT_ELAPSED_TIME); // Reset lastTime to current time
    playerY = 50; // Reset player position
    obstacleX = windowWidth; // Reset obstacle position
    obstacle2X = windowWidth + 400;
    gameStarted = true; // Set game started state
    gameOver = false; // Set game over state
    gameWon = false;
    invincible = false;
    scoreMultiplier = false;
    playerCollided = false;
}

// Restart Game Function
void RestartGame() {
    StartGame(); // Call start game to reset everything
}

// Animation Function
void Anim(void)
{
    HandleJump(); // Handle jumping logic

    powerUpAngle += 0.2f; // Increase angle for spinning (adjust speed as needed)
    if (powerUpAngle >= 360.0f) {
        powerUpAngle -= 360.0f; // Keep angle within 0 to 360 degrees
    }

    collectibleAngle += 0.1f; // Increase angle for spinning (adjust speed as needed)
    if (collectibleAngle >= 360.0f) {
        collectibleAngle -= 360.0f; // Keep angle within 0 to 360 degrees
    }

    // Update lightning effect
    lightningTimer += 0.016f; // Increment timer (assuming ~60 FPS)
    if (lightningTimer >= lightningInterval) {
        lightningActive = rand() % 2; // Randomly activate or deactivate lightning
        lightningTimer = 0.0f; // Reset timer
    }

    // Redisplay the scene
    glutPostRedisplay(); // Mark the current window as needing to be redrawn
}

