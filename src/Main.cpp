/*******************************************************************************************
*
*   raylib - classic game: tetris
*
*   Sample game developed by Marc Palau and Ramon Santamaria
*
*   This game has been created using raylib v1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "flecs.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define SQUARE_SIZE             20

#define GRID_HORIZONTAL_SIZE    12
#define GRID_VERTICAL_SIZE      20

#define LATERAL_SPEED           10
#define TURNING_SPEED           12
#define FAST_FALL_AWAIT_COUNTER 30

#define FADING_TIME             33

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GridSquare { EMPTY, MOVING, FULL, BLOCK, FADING } GridSquare;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

static bool gameOver = false;
static bool pause = false;

// Matrices
static GridSquare grid[GRID_HORIZONTAL_SIZE][GRID_VERTICAL_SIZE];
static GridSquare piece[4][4];
static GridSquare incomingPiece[4][4];

// Theese variables keep track of the active piece position
static int piecePositionX = 0;
static int piecePositionY = 0;

// Game parameters
static Color fadingColor;
//static int fallingSpeed;           // In frames

static bool beginPlay = true; // This var is only true at the begining of the game, used for the first matrix creations
static bool pieceActive = false;
static bool detection = false;
static bool lineToDelete = false;

// Statistics
static int level = 1;
static int lines = 0;

// Counters
static int gravityMovementCounter = 0;
static int lateralMovementCounter = 0;
static int turnMovementCounter = 0;
static int fastFallMovementCounter = 0;

static int fadeLineCounter = 0;

// Based on level
static int gravitySpeed = 30;

// Custom color definitions
Color grayColor = {128, 128, 128, 255}; // Gray
Color darkGrayColor = {80, 80, 80, 255}; // Dark Gray
Color lightGrayColor = {200, 200, 200, 255}; // Light Gray
Color maroonColor = {128, 0, 0, 255}; // Maroon
Color whiteColor = {255, 255, 255, 255}; // White

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(); // Initialize game
static void UpdateGame(); // Update game (one frame)
static void DrawGame(); // Draw game (one frame)
static void UnloadGame(); // Unload game
static void UpdateDrawFrame(); // Update and Draw (one frame)

// Additional module functions
static bool Createpiece();

static void GetRandompiece();

static void ResolveFallingMovement(bool *detection, bool *pieceActive);

static bool ResolveLateralMovement();

static bool ResolveTurnMovement();

static void CheckDetection(bool *detection);

static void CheckCompletion(bool *lineToDelete);

static int DeleteCompleteLines();

// Define a Position component for Flecs
struct Position {
    float x, y;
};

// Define a Velocity component for Flecs
struct Velocity {
    float x, y;
};

static void DrawVelocity(flecs::world &ecs);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main() {
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "classic game: tetris");

    // Create Flecs world (entity manager)
    flecs::world ecs;

    // Register components in Flecs
    ecs.component<Position>();
    ecs.component<Velocity>();

    // Create some entities with Position and Velocity components
    ecs.entity("Entity1")
            .set<Position>({100, 100})
            .set<Velocity>({50, 30});

    ecs.entity("Entity2")
            .set<Position>({200, 200})
            .set<Velocity>({-40, 20});

    // Define a system to move entities based on their Velocity
    ecs.system<Position, Velocity>()
            .each([](Position &pos, Velocity &vel) {
                // Update position based on velocity
                pos.x += vel.x * GetFrameTime();
                pos.y += vel.y * GetFrameTime();

                // Bounce off the window edges
                if (pos.x < 0 || pos.x > screenWidth) vel.x = -vel.x;
                if (pos.y < 0 || pos.y > screenHeight) vel.y = -vel.y;
            });


    InitGame();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update ECS
        ecs.progress();

        // Draw velocities
        DrawVelocity(ecs);

        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame(); // Unload loaded data (textures, sounds, models...)

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//--------------------------------------------------------------------------------------
// Game Module Functions Definition
//--------------------------------------------------------------------------------------

// Initialize game variables
void InitGame() {
    // Initialize game statistics
    level = 1;
    lines = 0;

    fadingColor = grayColor;

    piecePositionX = 0;
    piecePositionY = 0;

    pause = false;

    beginPlay = true;
    pieceActive = false;
    detection = false;
    lineToDelete = false;

    // Counters
    gravityMovementCounter = 0;
    lateralMovementCounter = 0;
    turnMovementCounter = 0;
    fastFallMovementCounter = 0;

    fadeLineCounter = 0;
    gravitySpeed = 30;

    // Initialize grid matrices
    for (int i = 0; i < GRID_HORIZONTAL_SIZE; i++) {
        for (int j = 0; j < GRID_VERTICAL_SIZE; j++) {
            if ((j == GRID_VERTICAL_SIZE - 1) || (i == 0) || (i == GRID_HORIZONTAL_SIZE - 1)) grid[i][j] = BLOCK;
            else grid[i][j] = EMPTY;
        }
    }

    // Initialize incoming piece matrices
    for (auto &i: incomingPiece) {
        for (auto &j: i) {
            j = EMPTY;
        }
    }
}

// Update game (one frame)
void UpdateGame() {
    if (!gameOver) {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause) {
            if (!lineToDelete) {
                if (!pieceActive) {
                    // Get another piece
                    pieceActive = Createpiece();

                    // We leave a little time before starting the fast falling down
                    fastFallMovementCounter = 0;
                } else // Piece falling
                {
                    // Counters update
                    fastFallMovementCounter++;
                    gravityMovementCounter++;
                    lateralMovementCounter++;
                    turnMovementCounter++;

                    // We make sure to move if we've pressed the key this frame
                    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) lateralMovementCounter = LATERAL_SPEED;
                    if (IsKeyPressed(KEY_UP)) turnMovementCounter = TURNING_SPEED;

                    // Fall down
                    if (IsKeyDown(KEY_DOWN) && (fastFallMovementCounter >= FAST_FALL_AWAIT_COUNTER)) {
                        // We make sure the piece is going to fall this frame
                        gravityMovementCounter += gravitySpeed;
                    }

                    if (gravityMovementCounter >= gravitySpeed) {
                        // Basic falling movement
                        CheckDetection(&detection);

                        // Check if the piece has collided with another piece or with the boundings
                        ResolveFallingMovement(&detection, &pieceActive);

                        // Check if we fullfilled a line and if so, erase the line and pull down the the lines above
                        CheckCompletion(&lineToDelete);

                        gravityMovementCounter = 0;
                    }

                    // Move laterally at player's will
                    if (lateralMovementCounter >= LATERAL_SPEED) {
                        // Update the lateral movement and if success, reset the lateral counter
                        if (!ResolveLateralMovement()) lateralMovementCounter = 0;
                    }

                    // Turn the piece at player's will
                    if (turnMovementCounter >= TURNING_SPEED) {
                        // Update the turning movement and reset the turning counter
                        if (ResolveTurnMovement()) turnMovementCounter = 0;
                    }
                }

                // Game over logic
                for (int j = 0; j < 2; j++) {
                    for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++) {
                        if (grid[i][j] == FULL) {
                            gameOver = true;
                        }
                    }
                }
            } else {
                // Animation when deleting lines
                fadeLineCounter++;

                fadingColor = (fadeLineCounter % 8 < 4) ? maroonColor : grayColor;

                if (fadeLineCounter >= FADING_TIME) {
                    int deletedLines = 0;
                    deletedLines = DeleteCompleteLines();
                    fadeLineCounter = 0;
                    lineToDelete = false;

                    lines += deletedLines;
                }
            }
        }
    } else {
        if (IsKeyPressed(KEY_ENTER)) {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame() {
    BeginDrawing();

    ClearBackground(whiteColor);

    if (!gameOver) {
        // Draw gameplay area
        Vector2 offset;
        offset.x = (float) screenWidth / 2 - ((float) GRID_HORIZONTAL_SIZE * SQUARE_SIZE / 2) - 50;
        offset.y = (float) screenHeight / 2 - (((float) GRID_VERTICAL_SIZE - 1) * SQUARE_SIZE / 2) + SQUARE_SIZE * 2;

        offset.y -= 50; // NOTE: Harcoded position!

        int controller = (int) offset.x;

        for (int j = 0; j < GRID_VERTICAL_SIZE; j++) {
            for (auto &i: grid) {
                // Draw each square of the grid
                if (i[j] == EMPTY) {
                    DrawLine((int) offset.x, (int) offset.y, (int) offset.x + SQUARE_SIZE, (int) offset.y,
                             lightGrayColor);
                    DrawLine((int) offset.x, (int) offset.y, (int) offset.x, (int) offset.y + SQUARE_SIZE,
                             lightGrayColor);
                    DrawLine((int) offset.x + SQUARE_SIZE, (int) offset.y, (int) offset.x + SQUARE_SIZE,
                             (int) offset.y + SQUARE_SIZE, lightGrayColor);
                    DrawLine((int) offset.x, (int) offset.y + SQUARE_SIZE, (int) offset.x + SQUARE_SIZE,
                             (int) offset.y + SQUARE_SIZE, lightGrayColor);
                    offset.x += SQUARE_SIZE;
                } else if (i[j] == FULL) {
                    DrawRectangle((int) offset.x, (int) offset.y, SQUARE_SIZE, SQUARE_SIZE, grayColor);
                    offset.x += SQUARE_SIZE;
                } else if (i[j] == MOVING) {
                    DrawRectangle((int) offset.x, (int) offset.y, SQUARE_SIZE, SQUARE_SIZE, darkGrayColor);
                    offset.x += SQUARE_SIZE;
                } else if (i[j] == BLOCK) {
                    DrawRectangle((int) offset.x, (int) offset.y, SQUARE_SIZE, SQUARE_SIZE, lightGrayColor);
                    offset.x += SQUARE_SIZE;
                } else if (i[j] == FADING) {
                    DrawRectangle((int) offset.x, (int) offset.y, SQUARE_SIZE, SQUARE_SIZE, fadingColor);
                    offset.x += SQUARE_SIZE;
                }
            }

            offset.x = (float) controller;
            offset.y += SQUARE_SIZE;
        }

        // Draw incoming piece (hardcoded)
        offset.x = 500;
        offset.y = 45;

        int controler = (int) offset.x;

        for (int j = 0; j < 4; j++) {
            for (auto &i: incomingPiece) {
                if (i[j] == EMPTY) {
                    DrawLine((int) offset.x, (int) offset.y, (int) offset.x + SQUARE_SIZE, (int) offset.y,
                             lightGrayColor);
                    DrawLine((int) offset.x, (int) offset.y, (int) offset.x, (int) offset.y + SQUARE_SIZE,
                             lightGrayColor);
                    DrawLine((int) offset.x + SQUARE_SIZE, (int) offset.y, (int) offset.x + SQUARE_SIZE,
                             (int) offset.y + SQUARE_SIZE, lightGrayColor);
                    DrawLine((int) offset.x, (int) offset.y + SQUARE_SIZE, (int) offset.x + SQUARE_SIZE,
                             (int) offset.y + SQUARE_SIZE, lightGrayColor);
                    offset.x += SQUARE_SIZE;
                } else if (i[j] == MOVING) {
                    DrawRectangle((int) offset.x, (int) offset.y, SQUARE_SIZE, SQUARE_SIZE, grayColor);
                    offset.x += SQUARE_SIZE;
                }
            }

            offset.x = (float) controler;
            offset.y += SQUARE_SIZE;
        }

        DrawText("INCOMING:", (int) offset.x, (int) offset.y - 100, 10, grayColor);
        DrawText(TextFormat("LINES:      %04i", lines), (int) offset.x, (int) offset.y + 20, 10, grayColor);

        if (pause)
            DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2, screenHeight / 2 - 40,
                     40, grayColor);
    } else
        DrawText("PRESS [ENTER] TO PLAY AGAIN",
                 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
                 GetScreenHeight() / 2 - 50, 20, grayColor);

    EndDrawing();
}

// Unload game variables
void UnloadGame() {
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame() {
    UpdateGame();
    DrawGame();
}

//--------------------------------------------------------------------------------------
// Additional module functions
//--------------------------------------------------------------------------------------
static bool Createpiece() {
    piecePositionX = (int) ((GRID_HORIZONTAL_SIZE - 4) / 2);
    piecePositionY = 0;

    // If the game is starting and you are going to create the first piece, we create an extra one
    if (beginPlay) {
        GetRandompiece();
        beginPlay = false;
    }

    // We assign the incoming piece to the actual piece
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            piece[i][j] = incomingPiece[i][j];
        }
    }

    // We assign a random piece to the incoming one
    GetRandompiece();

    // Assign the piece to the grid
    for (int i = piecePositionX; i < piecePositionX + 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece[i - (int) piecePositionX][j] == MOVING) grid[i][j] = MOVING;
        }
    }

    return true;
}

static void GetRandompiece() {
    int random = GetRandomValue(0, 6);

    for (auto &i: incomingPiece) {
        for (auto &j: i) {
            j = EMPTY;
        }
    }

    switch (random) {
        case 0: {
            incomingPiece[1][1] = MOVING;
            incomingPiece[2][1] = MOVING;
            incomingPiece[1][2] = MOVING;
            incomingPiece[2][2] = MOVING;
        }
        break; //Cube
        case 1: {
            incomingPiece[1][0] = MOVING;
            incomingPiece[1][1] = MOVING;
            incomingPiece[1][2] = MOVING;
            incomingPiece[2][2] = MOVING;
        }
        break; //L
        case 2: {
            incomingPiece[1][2] = MOVING;
            incomingPiece[2][0] = MOVING;
            incomingPiece[2][1] = MOVING;
            incomingPiece[2][2] = MOVING;
        }
        break; //L inversa
        case 3: {
            incomingPiece[0][1] = MOVING;
            incomingPiece[1][1] = MOVING;
            incomingPiece[2][1] = MOVING;
            incomingPiece[3][1] = MOVING;
        }
        break; //Recta
        case 4: {
            incomingPiece[1][0] = MOVING;
            incomingPiece[1][1] = MOVING;
            incomingPiece[1][2] = MOVING;
            incomingPiece[2][1] = MOVING;
        }
        break; //Creu tallada
        case 5: {
            incomingPiece[1][1] = MOVING;
            incomingPiece[2][1] = MOVING;
            incomingPiece[2][2] = MOVING;
            incomingPiece[3][2] = MOVING;
        }
        break; //S
        case 6: {
            incomingPiece[1][2] = MOVING;
            incomingPiece[2][2] = MOVING;
            incomingPiece[2][1] = MOVING;
            incomingPiece[3][1] = MOVING;
        }
        break; //S inversa
        default: ;
    }
}

static void ResolveFallingMovement(bool *detection, bool *pieceActive) {
    // If we finished moving this piece, we stop it
    if (*detection) {
        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--) {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++) {
                if (grid[i][j] == MOVING) {
                    grid[i][j] = FULL;
                    *detection = false;
                    *pieceActive = false;
                }
            }
        }
    } else // We move down the piece
    {
        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--) {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++) {
                if (grid[i][j] == MOVING) {
                    grid[i][j + 1] = MOVING;
                    grid[i][j] = EMPTY;
                }
            }
        }

        piecePositionY++;
    }
}

static bool ResolveLateralMovement() {
    bool collision = false;

    // Piece movement
    if (IsKeyDown(KEY_LEFT)) // Move left
    {
        // Check if is possible to move to left
        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--) {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++) {
                if (grid[i][j] == MOVING) {
                    // Check if we are touching the left wall or we have a full square at the left
                    if ((i - 1 == 0) || (grid[i - 1][j] == FULL)) collision = true;
                }
            }
        }

        // If able, move left
        if (!collision) {
            for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--) {
                for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++) // We check the matrix from left to right
                {
                    // Move everything to the left
                    if (grid[i][j] == MOVING) {
                        grid[i - 1][j] = MOVING;
                        grid[i][j] = EMPTY;
                    }
                }
            }

            piecePositionX--;
        }
    } else if (IsKeyDown(KEY_RIGHT)) // Move right
    {
        // Check if is possible to move to right
        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--) {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++) {
                if (grid[i][j] == MOVING) {
                    // Check if we are touching the right wall or we have a full square at the right
                    if ((i + 1 == GRID_HORIZONTAL_SIZE - 1) || (grid[i + 1][j] == FULL)) {
                        collision = true;
                    }
                }
            }
        }

        // If able move right
        if (!collision) {
            for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--) {
                for (int i = GRID_HORIZONTAL_SIZE - 1; i >= 1; i--) // We check the matrix from right to left
                {
                    // Move everything to the right
                    if (grid[i][j] == MOVING) {
                        grid[i + 1][j] = MOVING;
                        grid[i][j] = EMPTY;
                    }
                }
            }

            piecePositionX++;
        }
    }

    return collision;
}

static bool ResolveTurnMovement() {
    // Input for turning the piece
    if (IsKeyDown(KEY_UP)) {
        GridSquare aux;
        bool checker = false;

        // Check all turning possibilities
        if ((grid[piecePositionX + 3][piecePositionY] == MOVING) &&
            (grid[piecePositionX][piecePositionY] != EMPTY) &&
            (grid[piecePositionX][piecePositionY] != MOVING))
            checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 3] == MOVING) &&
            (grid[piecePositionX + 3][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY] != MOVING))
            checker = true;

        if ((grid[piecePositionX][piecePositionY + 3] == MOVING) &&
            (grid[piecePositionX + 3][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 3] != MOVING))
            checker = true;

        if ((grid[piecePositionX][piecePositionY] == MOVING) &&
            (grid[piecePositionX][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 3] != MOVING))
            checker = true;

        if ((grid[piecePositionX + 1][piecePositionY] == MOVING) &&
            (grid[piecePositionX][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 2] != MOVING))
            checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 1] == MOVING) &&
            (grid[piecePositionX + 1][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY] != MOVING))
            checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 3] == MOVING) &&
            (grid[piecePositionX + 3][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 1] != MOVING))
            checker = true;

        if ((grid[piecePositionX][piecePositionY + 2] == MOVING) &&
            (grid[piecePositionX + 2][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 3] != MOVING))
            checker = true;

        if ((grid[piecePositionX + 2][piecePositionY] == MOVING) &&
            (grid[piecePositionX][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX][piecePositionY + 1] != MOVING))
            checker = true;

        if ((grid[piecePositionX + 3][piecePositionY + 2] == MOVING) &&
            (grid[piecePositionX + 2][piecePositionY] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY] != MOVING))
            checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 3] == MOVING) &&
            (grid[piecePositionX + 3][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 3][piecePositionY + 2] != MOVING))
            checker = true;

        if ((grid[piecePositionX][piecePositionY + 1] == MOVING) &&
            (grid[piecePositionX + 1][piecePositionY + 3] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 3] != MOVING))
            checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 1] == MOVING) &&
            (grid[piecePositionX + 1][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 2] != MOVING))
            checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 1] == MOVING) &&
            (grid[piecePositionX + 1][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 1][piecePositionY + 1] != MOVING))
            checker = true;

        if ((grid[piecePositionX + 2][piecePositionY + 2] == MOVING) &&
            (grid[piecePositionX + 2][piecePositionY + 1] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 1] != MOVING))
            checker = true;

        if ((grid[piecePositionX + 1][piecePositionY + 2] == MOVING) &&
            (grid[piecePositionX + 2][piecePositionY + 2] != EMPTY) &&
            (grid[piecePositionX + 2][piecePositionY + 2] != MOVING))
            checker = true;

        if (!checker) {
            aux = piece[0][0];
            piece[0][0] = piece[3][0];
            piece[3][0] = piece[3][3];
            piece[3][3] = piece[0][3];
            piece[0][3] = aux;

            aux = piece[1][0];
            piece[1][0] = piece[3][1];
            piece[3][1] = piece[2][3];
            piece[2][3] = piece[0][2];
            piece[0][2] = aux;

            aux = piece[2][0];
            piece[2][0] = piece[3][2];
            piece[3][2] = piece[1][3];
            piece[1][3] = piece[0][1];
            piece[0][1] = aux;

            aux = piece[1][1];
            piece[1][1] = piece[2][1];
            piece[2][1] = piece[2][2];
            piece[2][2] = piece[1][2];
            piece[1][2] = aux;
        }

        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--) {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++) {
                if (grid[i][j] == MOVING) {
                    grid[i][j] = EMPTY;
                }
            }
        }

        for (int i = piecePositionX; i < piecePositionX + 4; i++) {
            for (int j = piecePositionY; j < piecePositionY + 4; j++) {
                if (piece[i - piecePositionX][j - piecePositionY] == MOVING) {
                    grid[i][j] = MOVING;
                }
            }
        }

        return true;
    }

    return false;
}

static void CheckDetection(bool *detection) {
    for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--) {
        for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++) {
            if ((grid[i][j] == MOVING) && ((grid[i][j + 1] == FULL) || (grid[i][j + 1] == BLOCK))) *detection = true;
        }
    }
}

static void CheckCompletion(bool *lineToDelete) {
    int calculator = 0;

    for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--) {
        calculator = 0;
        for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++) {
            // Count each square of the line
            if (grid[i][j] == FULL) {
                calculator++;
            }

            // Check if we completed the whole line
            if (calculator == GRID_HORIZONTAL_SIZE - 2) {
                *lineToDelete = true;
                calculator = 0;
                // points++;

                // Mark the completed line
                for (int z = 1; z < GRID_HORIZONTAL_SIZE - 1; z++) {
                    grid[z][j] = FADING;
                }
            }
        }
    }
}

static int DeleteCompleteLines() {
    int deletedLines = 0;

    // Erase the completed line
    for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--) {
        while (grid[1][j] == FADING) {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++) {
                grid[i][j] = EMPTY;
            }

            for (int j2 = j - 1; j2 >= 0; j2--) {
                for (int i2 = 1; i2 < GRID_HORIZONTAL_SIZE - 1; i2++) {
                    if (grid[i2][j2] == FULL) {
                        grid[i2][j2 + 1] = FULL;
                        grid[i2][j2] = EMPTY;
                    } else if (grid[i2][j2] == FADING) {
                        grid[i2][j2 + 1] = FADING;
                        grid[i2][j2] = EMPTY;
                    }
                }
            }

            deletedLines++;
        }
    }

    return deletedLines;
}

// Flecs circle
void DrawVelocity(flecs::world &ecs) {
    ecs.each<Position>([](flecs::entity e, Position const &pos) {
        DrawCircle((int) pos.x, (int) pos.y, 10.0, BLUE);
    });
}
