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


#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <map>

#include "boost/di.hpp"
#include "boost/sml.hpp"
#include "boost/sml/utility/dispatch_table.hpp"
#include "lib/Renderer.hpp"

#include "Components.hpp"
#include "raylib.h"
#include "flecs.h"

const int CHARACTER_IDLE_WIDTH = 32;
const int CHARACTER_IDLE_HEIGHT = 32;
const int screenWidth = 800;
const int screenHeight = 450;
const int TARGET_FPS = 120;

#ifdef _WIN32
int WinMain() {
#else
int main() {
#endif
    Renderer renderer = Renderer({
        {CHARACTERS, "resources/characters.json"},
    });

    // Instantiate an ECS world
    flecs::world ecsWorld;

    // Register components
    ecsWorld.component<Render>();
    ecsWorld.component<Animation>();

    // Create a new entity with Render and Animation components
    ecsWorld.entity()
        .set<Render>(
            {
                .z_index = 1,
                .position = { static_cast<float>(screenWidth)/3, static_cast<float>(screenHeight)/3 },
                .sprite = renderer.getSpriteMap().at("attack/thief_0001")
            }
        )
        .set<Animation>({ .name = "job1/m_bald_0001", .actual_frame = 1, .total_frames = 8, .current_frame = 0.0f });

    // Create a new entity with Render and Animation components
    auto entity = ecsWorld.entity()
        .set<Render>(
            {
                .z_index = 2,
                .position = { static_cast<float>(screenWidth)/3 + 5, static_cast<float>(screenHeight)/3 + 5 },
                .sprite = renderer.getSpriteMap().at("attack/thief_0001")
            }
        )
        .set<Animation>({ .name = "job1/w_blonde_0001", .actual_frame = 1, .total_frames = 8, .current_frame = 0.0f });

    // Number of entities on the world
    std::cout << "Entities with Render component: " << ecsWorld.count<Render>() << std::endl;

    renderer.Execute(&ecsWorld);

    // Initialization
    //--------------------------------------------------------------------------------------

    Image image = LoadImage("resources/icon.png");
    InitWindow(screenWidth, screenHeight, "Simple Raylib Game");
    SetWindowIcon(image);

    renderer.LoadTextures({
        {CHARACTERS, "resources/characters.png"},
    });

    Vector2 ballPosition = {
        static_cast<float>(screenWidth)/2,
        static_cast<float>(screenHeight)/2
    };

    const float BALL_SPEED = 5.0f;
    const float BALL_RADIUS = 50.0f;
    float ballSpeed = BALL_SPEED;


    SetTargetFPS(TARGET_FPS);
    //--------------------------------------------------------------------------------------

    const int TEXT_POSITION_X = 10;
    const int TEXT_POSITION_Y = 10;
    const int TEXT_FONT_SIZE = 20;

    // Main game loop
    while (!WindowShouldClose()) {    // Detect window close button or ESC key
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_RIGHT)) ballPosition.x += ballSpeed;
        if (IsKeyDown(KEY_LEFT)) ballPosition.x -= ballSpeed;
        if (IsKeyDown(KEY_UP)) ballPosition.y -= ballSpeed;
        if (IsKeyDown(KEY_DOWN)) ballPosition.y += ballSpeed;
        ecsWorld.progress();
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawText("Move the ball with arrow keys",
            TEXT_POSITION_X, TEXT_POSITION_Y,
            TEXT_FONT_SIZE,
            DARKGRAY);
        DrawCircleV(ballPosition, BALL_RADIUS, MAROON);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
