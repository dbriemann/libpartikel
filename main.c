/*******************************************************************************************
*
*   libpartikel example - Show some simple particle systems / effects.
*
*   This example has been created using
*   - raylib 1.8 (www.raylib.com)
*   - libpartikel (https://github.com/dbriemann/libpartikel)
*
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*   libpartikel is licensed under an unmodified zlib/libpng license (View partikel.h for details)
*
*   Copyright (c) 2O17 David Linus Briemann (@Raging_Dave)
*
********************************************************************************************/

#include "stdio.h"
#include "raylib.h"
#include "partikel.h"

// Global data.
//----------------------------------------------------------------------------------
int screenWidth = 1000;
int screenHeight = 800;
u_int32_t counter = 0;
Camera2D camera;

Texture2D texCircle16;
Texture2D texCircle8;

int activeEmitter = 0;
Emitter *emitterFountain1 = NULL;
Emitter *emitterFountain2 = NULL;
Emitter *emitterFountain3 = NULL;

// Define a custom particle deactivator function.
bool Particle_DeactivatorFountain(Particle *p) {
    return (p->position.y > (camera.target.y + camera.offset.y) // bottom
            || p->position.x < (camera.target.x - camera.offset.x) // left
            || p->position.x > (camera.target.x + camera.offset.x) // right
            || Particle_DeactivatorAge(p));
}

// Init sets up all relevant data.
void Init() {
    InitWindow(screenWidth, screenHeight, "libpartikel Examples");
    SetTargetFPS(121);
    HideCursor();

    camera.target = (Vector2) {.x = 0, .y = 0};
    camera.offset = (Vector2) {.x = screenWidth/2, .y = screenHeight/2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Generate some simple textures.
    Image imgCircle16 = GenImageGradientRadial(16, 16, 0.3, WHITE, BLACK);
    texCircle16 = LoadTextureFromImage(imgCircle16);
    Image imgCircle8 = GenImageGradientRadial(8, 8, 0.5, WHITE, BLACK);
    texCircle8 = LoadTextureFromImage(imgCircle8);

    EmitterConfig ecfg1 = {
        .max_particles = 2000,
        .emissionRate = 200,
        .origin = (Vector2){.x = 0, .y = 0},
        .autoRefresh = true,
        .emissionMin = (Vector2){.x = -70, .y = -720},
        .emissionMax = (Vector2){.x = 70, .y = -750},
        .acceleration = (Vector2){.x = 0, .y = 981},
        .startColor = (Color){.r = 0, .g = 20, .b = 240, .a = 255},
        .endColor = (Color){.r = 0, .g = 20, .b = 240, .a = 0},
        .ageMin = 1.0,
        .ageMax = 3.0,
        .texture = texCircle16,

        .particle_Deactivator = Particle_DeactivatorFountain
    };

    emitterFountain1 = Emitter_New(ecfg1);
    Emitter_Start(emitterFountain1);

    EmitterConfig ecfg2 = {
        .max_particles = 1000,
        .emissionRate = 100,
        .origin = (Vector2){.x = 0, .y = 0},
        .autoRefresh = true,
        .emissionMin = (Vector2){.x = -20, .y = -800},
        .emissionMax = (Vector2){.x = 20, .y = -850},
        .acceleration = (Vector2){.x = 0, .y = 981},
        .startColor = (Color){.r = 0, .g = 20, .b = 240, .a = 255},
        .endColor = (Color){.r = 0, .g = 20, .b = 240, .a = 0},
        .ageMin = 1.0,
        .ageMax = 3.0,
        .texture = texCircle8,

        .particle_Deactivator = Particle_DeactivatorFountain
    };
    emitterFountain2 = Emitter_New(ecfg2);
    Emitter_Start(emitterFountain2);

    EmitterConfig ecfg3 = {
        .max_particles = 4000,
        .emissionRate = 1000,
        .origin = (Vector2){.x = 0, .y = 0},
        .autoRefresh = true,
        .emissionMin = (Vector2){.x = -200, .y = -500},
        .emissionMax = (Vector2){.x = 200, .y = -550},
        .acceleration = (Vector2){.x = 0, .y = 981},
        .startColor = (Color){.r = 0, .g = 20, .b = 240, .a = 255},
        .endColor = (Color){.r = 0, .g = 20, .b = 240, .a = 0},
        .ageMin = 1.0,
        .ageMax = 3.0,
        .texture = texCircle16,

        .particle_Deactivator = Particle_DeactivatorFountain
    };
    emitterFountain3 = Emitter_New(ecfg3);
    Emitter_Start(emitterFountain3);
}

// Destroy and free all the global data.
void Destroy() {
    Emitter_Free(emitterFountain1);
    Emitter_Free(emitterFountain2);
    Emitter_Free(emitterFountain3);

    UnloadTexture(texCircle8);
    UnloadTexture(texCircle16);

    // Close window and OpenGL context
    CloseWindow();
}

void Update(double dt) {
    Vector2 m = GetMousePosition();
    m.x -= screenWidth/2;
    m.y -= screenHeight/2;
    counter = 0;

    switch (activeEmitter) {
    case 0:
        emitterFountain1->config.origin.x = m.x;
        emitterFountain1->config.origin.y = m.y;
        counter += Emitter_Update(emitterFountain1, dt);

        emitterFountain2->config.origin.x = m.x;
        emitterFountain2->config.origin.y = m.y;
        counter += Emitter_Update(emitterFountain2, dt);

        emitterFountain3->config.origin.x = m.x;
        emitterFountain3->config.origin.y = m.y;
        counter += Emitter_Update(emitterFountain3, dt);
        break;
    default:
        break;
    }
}

void Draw() {
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(BLACK);

    Begin2dMode(camera);

    // Draw scene here.
    switch (activeEmitter) {
    case 0:
        Emitter_Draw(emitterFountain1, BLEND_ADDITIVE);
        Emitter_Draw(emitterFountain2, BLEND_ADDITIVE);
        Emitter_Draw(emitterFountain3, BLEND_ADDITIVE);
        break;
    default:
        break;
    }

    End2dMode();

    // Draw HUD etc. here.
    DrawFPS(10, 10);
    char pcount[20];
    sprintf(pcount, "Particles: %d", counter);
    DrawText(pcount, 10, 40, 20, DARKGREEN);

    EndDrawing();
}

int main(int argc, char * argv[argc + 1]) {

    // Initialization
    //----------------------------------------------------------------------------------
    Init();

    // Main game loop
    //----------------------------------------------------------------------------------
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        double dt = GetFrameTime();
        Update(dt);

        Draw();
    }

    // De-Initialization
    //----------------------------------------------------------------------------------
    Destroy();

    return 0;
}
