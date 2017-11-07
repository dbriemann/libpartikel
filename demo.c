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
Texture2D texCircle4;

int activePS = 1;

ParticleSystem *ps1 = NULL;
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

bool Particle_DeactivatorFire(Particle *p) {
    return (p->position.y < (camera.target.y - camera.offset.y)
            || Particle_DeactivatorAge(p));
}

void InitFountain() {
    ps1 = ParticleSystem_New();

    EmitterConfig ecfg1 = {
        .capacity = 2000,
        .emissionRate = 200,
        .origin = (Vector2){.x = 0, .y = 0},
        .emissionMin = (Vector2){.x = -70, .y = -720},
        .emissionMax = (Vector2){.x = 70, .y = -750},
        .acceleration = (Vector2){.x = 0, .y = 981},
        .startColor = (Color){.r = 0, .g = 20, .b = 255, .a = 255},
        .endColor = (Color){.r = 0, .g = 150, .b = 100, .a = 0},
        .age = (FloatMinMax){.min = 1.0, .max = 3.0},
        .texture = texCircle16,

        .particle_Deactivator = Particle_DeactivatorFountain
    };
    emitterFountain1 = Emitter_New(ecfg1);
    ParticleSystem_Register(ps1, emitterFountain1);

    ecfg1.capacity = 1000;
    ecfg1.emissionRate = 200;
    ecfg1.emissionMin = (Vector2){.x = -20, .y = -800};
    ecfg1.emissionMax = (Vector2){.x = 20, .y = -850};
    ecfg1.texture = texCircle8;
    emitterFountain2 = Emitter_New(ecfg1);
    ParticleSystem_Register(ps1, emitterFountain2);

    ecfg1.capacity = 4000;
    ecfg1.emissionRate = 1000;
    ecfg1.emissionMin = (Vector2){.x = -200, .y = -500};
    ecfg1.emissionMax = (Vector2){.x = 200, .y = -550};
    ecfg1.texture = texCircle16;
    emitterFountain3 = Emitter_New(ecfg1);
    ParticleSystem_Register(ps1, emitterFountain3);

    ParticleSystem_Start(ps1);
}

void DestroyFountain() {
    Emitter_Free(emitterFountain1);
    Emitter_Free(emitterFountain2);
    Emitter_Free(emitterFountain3);

    ParticleSystem_Free(ps1);
}

// Init sets up all relevant data.
void Init() {
    InitWindow(screenWidth, screenHeight, "libpartikel demo");
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
    Image imgCircle4 = GenImageGradientRadial(4, 4, 0.5, WHITE, BLACK);
    texCircle4 = LoadTextureFromImage(imgCircle4);

    UnloadImage(imgCircle4);
    UnloadImage(imgCircle8);
    UnloadImage(imgCircle16);

    InitFountain();
}

// Destroy and free all the global data.
void Destroy() {
    DestroyFountain();

    UnloadTexture(texCircle4);
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

    bool trigger = false;

    int key = GetKeyPressed();
    switch(key) {
    case KEY_ONE:
        activePS = 1;
        break;
    case KEY_TWO:
        activePS = 2;
        break;
    case KEY_SPACE:
        trigger = true;
        break;
    default:
        break;
    }

    switch (activePS) {
    case 1:
        ParticleSystem_SetOrigin(ps1, (Vector2){.x = m.x, .y = m.y});
        counter += ParticleSystem_Update(ps1, dt);
        break;
    case 2:
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
    switch (activePS) {
    case 1:
        ParticleSystem_Draw(ps1, BLEND_ADDITIVE);
        break;
    case 2:
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
