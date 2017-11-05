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
double multiplier = 1;
int screenWidth = 1000;
int screenHeight = 800;
u_int32_t counter = 0;
Camera2D camera;

size_t rate1 = 200;
size_t rate2 = 100;
size_t rate3 = 1000;

Texture2D texCircle16;
Texture2D texCircle8;
Texture2D texCircle4;

int activeEmitter = 1;
Emitter *emitterFountain1 = NULL;
Emitter *emitterFountain2 = NULL;
Emitter *emitterFountain3 = NULL;

Emitter *emitterFire1 = NULL;
Emitter *emitterFire2 = NULL;
Emitter *emitterFire3 = NULL;

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

// Init sets up all relevant data.
void Init() {
    InitWindow(screenWidth, screenHeight, "libpartikel examples");
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

    EmitterConfig ecfg1 = {
        .maxParticles = 2000,
        .emissionRate = rate1,
        .origin = (Vector2){.x = 0, .y = 0},
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
        .maxParticles = 1000,
        .emissionRate = rate2,
        .origin = (Vector2){.x = 0, .y = 0},
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
        .maxParticles = 4000,
        .emissionRate = rate3,
        .origin = (Vector2){.x = 0, .y = 0},
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

    EmitterConfig ecfg4 = {
        .maxParticles = 1000,
        .emissionRate = 500,
        .origin = (Vector2){.x = 0, .y = 0},
        .emissionMin = (Vector2){.x = -50, .y = -10},
        .emissionMax = (Vector2){.x = 50, .y = -20},
        .acceleration = (Vector2){.x = 0, .y = -800},
        .startColor = (Color){.r = 244, .g = 0, .b = 20, .a = 255},
        .endColor = (Color){.r = 244, .g = 0, .b = 20, .a = 0},
        .ageMin = 0.5,
        .ageMax = 1.0,
        .texture = texCircle16,

        .particle_Deactivator = Particle_DeactivatorFire
    };
    emitterFire1 = Emitter_New(ecfg4);
    Emitter_Start(emitterFire1);

    EmitterConfig ecfg5 = ecfg4;
    ecfg5.emissionRate = 100;
    ecfg5.acceleration = (Vector2){.x = 0, .y = -600},
    ecfg5.emissionMin = (Vector2){.x = -30, .y = -10},
    ecfg5.emissionMax = (Vector2){.x = 30, .y = -20},
    ecfg5.startColor = (Color){.r = 255, .g = 230, .b = 0, .a = 255};
    ecfg5.endColor = (Color){.r = 255, .g = 230, .b = 0, .a = 0};
    emitterFire2 = Emitter_New(ecfg5);
    Emitter_Start(emitterFire2);

    EmitterConfig ecfg6 = ecfg4;
    ecfg6.emissionRate = 20;
    ecfg6.acceleration = (Vector2){.x = 0, .y = -250},
    ecfg6.emissionMin = (Vector2){.x = -15, .y = -10},
    ecfg6.emissionMax = (Vector2){.x = 15, .y = -20},
    ecfg6.ageMin = 5.0,
    ecfg6.ageMax = 8.0,
    ecfg6.startColor = (Color){.r = 222, .g = 222, .b = 222, .a = 70};
    ecfg6.endColor = (Color){.r = 222, .g = 222, .b = 222, .a = 0};
    emitterFire3 = Emitter_New(ecfg6);
    Emitter_Start(emitterFire3);
}

// Destroy and free all the global data.
void Destroy() {
    Emitter_Free(emitterFountain1);
    Emitter_Free(emitterFountain2);
    Emitter_Free(emitterFountain3);

    Emitter_Free(emitterFire1);
    Emitter_Free(emitterFire2);
    Emitter_Free(emitterFire3);

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
    case KEY_DOWN:
        multiplier -= 0.5;
        break;
    case KEY_UP:
        multiplier += 0.5;
        break;
    case KEY_ONE:
        activeEmitter = 1;
        break;
    case KEY_TWO:
        activeEmitter = 2;
        break;
    case KEY_SPACE:
        trigger = true;
        break;
    default:
        break;
    }

    if(multiplier < 0.1) {
        multiplier = 0.1;
    } else if(multiplier > 10.0) {
        multiplier = 10.0;
    }

    switch (activeEmitter) {
    case 1:
        emitterFountain1->config.emissionRate = rate1 * multiplier;
        emitterFountain1->config.origin.x = m.x;
        emitterFountain1->config.origin.y = m.y;
        counter += Emitter_Update(emitterFountain1, dt);

        emitterFountain2->config.emissionRate = rate2 * multiplier;
        emitterFountain2->config.origin.x = m.x;
        emitterFountain2->config.origin.y = m.y;
        counter += Emitter_Update(emitterFountain2, dt);

        emitterFountain3->config.emissionRate = rate3 * multiplier;
        emitterFountain3->config.origin.x = m.x;
        emitterFountain3->config.origin.y = m.y;
        counter += Emitter_Update(emitterFountain3, dt);
        break;
    case 2:
        emitterFire1->config.origin.x = m.x;
        emitterFire1->config.origin.y = m.y;
        counter += Emitter_Update(emitterFire1, dt);

        emitterFire2->config.origin.x = m.x;
        emitterFire2->config.origin.y = m.y;
        counter += Emitter_Update(emitterFire2, dt);

        emitterFire3->config.origin.x = m.x;
        emitterFire3->config.origin.y = m.y;
        counter += Emitter_Update(emitterFire3, dt);
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
    case 1:
        Emitter_Draw(emitterFountain1, BLEND_ADDITIVE);
        Emitter_Draw(emitterFountain2, BLEND_ADDITIVE);
        Emitter_Draw(emitterFountain3, BLEND_ADDITIVE);
        break;
    case 2:
        Emitter_Draw(emitterFire3, BLEND_ADDITIVE);
        Emitter_Draw(emitterFire1, BLEND_ADDITIVE);
        Emitter_Draw(emitterFire2, BLEND_ADDITIVE);
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
