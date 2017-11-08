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

int activePS = 2;

ParticleSystem *ps1 = NULL;
Emitter *emitterFountain1 = NULL;
Emitter *emitterFountain2 = NULL;
Emitter *emitterFountain3 = NULL;

ParticleSystem *ps2 = NULL;
Emitter *emitterSwirl1 = NULL;


// Define a custom particle deactivator function.
bool Particle_DeactivatorFountain(Particle *p) {
    return (p->position.y > (camera.target.y + camera.offset.y) // bottom
            || p->position.x < (camera.target.x - camera.offset.x) // left
            || p->position.x > (camera.target.x + camera.offset.x) // right
            || Particle_DeactivatorAge(p));
}

bool Particle_DeactivatorOutsideCam(Particle *p) {
    return (p->position.y < (camera.target.y - camera.offset.y)
            || Particle_DeactivatorAge(p));
}

void InitFountain() {
    ps1 = ParticleSystem_New();

    EmitterConfig ecfg1 = {
        .capacity = 600,
        .emissionRate = 200,
        .origin = (Vector2){.x = 0, .y = 0},
        .originAcceleration = (FloatMinMax){.min = 0, .max = 0},
        .direction = (Vector2){.x = 0, .y = -1}, // go up
        .directionAngle = (FloatMinMax){.min = -6, .max = 6}, // angle range -8 to +8 degree deviation from direction
        .velocityAngle = (FloatMinMax){.min = 0, .max = 0},
        .velocity = (FloatMinMax){.min = 700, .max = 730},
        .externalAcceleration = (Vector2){.x = 0, .y = 981},
        .startColor = (Color){.r = 0, .g = 20, .b = 255, .a = 255},
        .endColor = (Color){.r = 0, .g = 150, .b = 100, .a = 0},
        .age = (FloatMinMax){.min = 1.0, .max = 3.0},
//        .offset = (FloatMinMax){.min = -10.0, .max = -100.0},
        .texture = texCircle16,

        .particle_Deactivator = Particle_DeactivatorFountain
    };
    emitterFountain1 = Emitter_New(ecfg1);
    ParticleSystem_Register(ps1, emitterFountain1);

    ecfg1.directionAngle = (FloatMinMax){.min = -1.5, .max = 1.5},
    ecfg1.velocity = (FloatMinMax){.min = 800, .max = 850},
    ecfg1.texture = texCircle8;
    emitterFountain2 = Emitter_New(ecfg1);
    ParticleSystem_Register(ps1, emitterFountain2);

    ecfg1.capacity = 3000;
    ecfg1.emissionRate = 1000;
    ecfg1.directionAngle = (FloatMinMax){.min = -20, .max = 20},
    ecfg1.velocity = (FloatMinMax){.min = 500, .max = 550},
    ecfg1.texture = texCircle16;
    ecfg1.age = (FloatMinMax){.min = 0.0, .max = 3.0};
    emitterFountain3 = Emitter_New(ecfg1);
    ParticleSystem_Register(ps1, emitterFountain3);

    ParticleSystem_Start(ps1);
}

void InitSwirl() {
    ps2 = ParticleSystem_New();

    EmitterConfig ecfg = {
        .capacity = 10000,
        .emissionRate = 2000,
        .origin = (Vector2){.x = 0, .y = 0},
        .originAcceleration = (FloatMinMax){.min = 100, .max = 200},
        .offset = (FloatMinMax){.min = 10, .max = 20},
        .direction = (Vector2){.x = 0, .y = -1}, // go up
        .directionAngle = (FloatMinMax){.min = -180, .max = 180}, // angle range -8 to +8 degree deviation from direction
        .velocityAngle = (FloatMinMax){.min = 80, .max = 100},
        .velocity = (FloatMinMax){.min = 100, .max = 300},
//        .externalAcceleration = (Vector2){.x = 0, .y = 981},
        .startColor = (Color){.r = 244, .g = 20, .b = 0, .a = 255},
        .endColor = (Color){.r = 244, .g = 20, .b = 0, .a = 0},
        .age = (FloatMinMax){.min = 2.5, .max = 5.0},
        .texture = texCircle8,

        .particle_Deactivator = Particle_DeactivatorOutsideCam
    };

    emitterSwirl1 = Emitter_New(ecfg);
    ParticleSystem_Register(ps2, emitterSwirl1);

    ParticleSystem_Start(ps2);
}

void DestroyFountain() {
    Emitter_Free(emitterFountain1);
    Emitter_Free(emitterFountain2);
    Emitter_Free(emitterFountain3);

    ParticleSystem_Free(ps1);
}

void DestroySwirl() {
    Emitter_Free(emitterSwirl1);

    ParticleSystem_Free(ps2);
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
    InitSwirl();
}

// Destroy and free all the global data.
void Destroy() {
    DestroyFountain();
    DestroySwirl();

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
        ParticleSystem_SetOrigin(ps2, (Vector2){.x = m.x, .y = m.y});
        counter += ParticleSystem_Update(ps2, dt);
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
        ParticleSystem_Draw(ps2, BLEND_ADDITIVE);
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
