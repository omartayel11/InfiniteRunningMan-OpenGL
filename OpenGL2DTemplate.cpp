// Includes
#include <stdlib.h>
#include <cmath>
#include <glut.h>

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
void StartGame();
void RestartGame();

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
float gravity = 0.00009;


// Obstacle position and size
float obstacleX = windowWidth;
float obstacleY = 50;
float obstacleWidth = 30;
float obstacleHeight = 40;
float obstacleSpeed = 0.1f;

// Second Obstacle position and size
float obstacle2X = windowWidth + 50;  // Starts off-screen to the right
float obstacle2Y = 70;           // Set this to be above the first obstacle
float obstacle2Width = 30;
float obstacle2Height = 40;
float obstacle2Speed = 0.1f;      // Same speed for consistency




// Game state variables
int playerLives = 70;
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

// Main Function
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Infinite Runner Game");
    Init();

    glutDisplayFunc(Display);
    glutIdleFunc(Anim);
    glutKeyboardFunc([](unsigned char key, int x, int y) {
    if (key == ' ' && !isJumping && gameStarted && !gameOver) {
        isJumping = true;
        jumpSpeed = 32; // Initial jump speed
    }
    if (key == 'd' && !gameOver) {
        // Check if the player can duck without going below the lower boundary
        if (playerY - playerHeight / 2 >= lowerBoundaryY-30) {
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

    if (gameStarted && !gameOver) {
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

        // Handle collisions and game logic
        HandleCollisions();

        if (showElbes) {
            DrawText("elbes", playerX - 20, playerY + playerHeight + 40); // Display "elbes"
        }
    }
    else if (!gameStarted) {
        DrawStartScreen();
    }
    else if (gameOver) {
        DrawGameOverScreen();
    }

    glFlush(); // Flush the drawing to the screen
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





// Draw Obstacle Function
void DrawObstacle(void)
{
    // Draw first obstacle
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the first obstacle
    glBegin(GL_QUADS);
    glVertex2f(obstacleX, obstacleY);
    glVertex2f(obstacleX + obstacleWidth, obstacleY);
    glVertex2f(obstacleX + obstacleWidth, obstacleY + obstacleHeight);
    glVertex2f(obstacleX, obstacleY + obstacleHeight);
    glEnd();

    // Draw second obstacle
    glColor3f(0.0f, 0.0f, 1.0f); // Blue color for the second obstacle
    glBegin(GL_QUADS);
    glVertex2f(obstacle2X, obstacle2Y);
    glVertex2f(obstacle2X + obstacle2Width, obstacle2Y);
    glVertex2f(obstacle2X + obstacle2Width, obstacle2Y + obstacle2Height);
    glVertex2f(obstacle2X, obstacle2Y + obstacle2Height);
    glEnd();
}


// Collision Detection and Game Logic
void HandleCollisions(void)
{
    obstacleX -= obstacleSpeed; // Move obstacle towards the player
    obstacle2X -= obstacleSpeed;
    // Reset obstacle position when it goes off-screen
    if (obstacleX + obstacleWidth < 0) {
        obstacleX = windowWidth;
    }

    if (obstacle2X + obstacleWidth < 0) {
        obstacle2X = windowWidth + 50;
    }

    // Collision detection
    if (playerX + playerWidth > obstacleX && playerX < obstacleX + obstacleWidth &&
        playerY < obstacleY + obstacleHeight && playerY + playerHeight > obstacleY) {

        if (!isFlashing) { // Only decrement lives if not already flashing
            playerLives--;
            isFlashing = true;
            flashCounter = 0;
            showElbes = true;
            elbesCounter = 0;

            if (playerLives == 0) {
                gameOver = true; // Set game over state
            }
        }
    }

    if (playerX + playerWidth > obstacle2X && playerX < obstacle2X + obstacleWidth &&
        playerY < obstacle2Y + obstacleHeight && playerY + playerHeight > obstacle2Y) {

        if (!isFlashing) { // Only decrement lives if not already flashing
            playerLives--;
            isFlashing = true;
            flashCounter = 0;
            showElbes = true;
            elbesCounter = 0;

            if (playerLives == 0) {
                gameOver = true; // Set game over state
            }
        }
    }

    if (isFlashing) {
        flashCounter++;
        if (flashCounter > 40) { // Stop flashing after a short time
            isFlashing = false;
        }
    }

    if (showElbes) {
        elbesCounter++;
        if (elbesCounter > 1000) { // Show "elbes" 
            showElbes = false;
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
    // Draw upper boundary (solid rectangle)
    glColor3f(0.5f, 0.45f, 0.05f); // Brown color
    glBegin(GL_QUADS);
    glVertex2f(0, upperBoundaryY);
    glVertex2f(windowWidth, upperBoundaryY);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    // Draw lower boundary (solid rectangle)
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.25f, 0.0f); // Dark brown color
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, lowerBoundaryY);
    glVertex2f(0, lowerBoundaryY);
    glEnd();
}

// Draw Background Function
void DrawBackground(void)
{
    // Draw a gradient background
    for (int i = 0; i < windowHeight; i++) {
        float colorValue = (float)i / windowHeight; // Calculate gradient color
        glColor3f(colorValue, colorValue, 1.0f); // Blue gradient
        glBegin(GL_LINES);
        glVertex2f(0, i);
        glVertex2f(windowWidth, i);
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
    // Draw trunk
    glColor3f(0.545f, 0.271f, 0.075f); // Brown color
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + 10, y);
    glVertex2f(x + 10, y + 30);
    glVertex2f(x, y + 30);
    glEnd();

    // Draw leaves
    glColor3f(0.0f, 0.5f, 0.0f); // Green color
    DrawCircle(x + 5, y + 40, 20, 20);
}

// Draw Cloud Function
void DrawCloud(float x, float y)
{
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    DrawCircle(x, y, 15, 10);
    DrawCircle(x + 20, y, 15, 10);
    DrawCircle(x + 10, y + 15, 15, 10);
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
    playerLives = 70; // Reset lives
    playerY = 50; // Reset player position
    obstacleX = windowWidth; // Reset obstacle position
    gameStarted = true; // Set game started state
    gameOver = false; // Set game over state
}

// Restart Game Function
void RestartGame() {
    StartGame(); // Call start game to reset everything
}

// Animation Function
void Anim(void)
{
    HandleJump(); // Handle jumping logic

    // Redisplay the scene
    glutPostRedisplay(); // Mark the current window as needing to be redrawn

}
