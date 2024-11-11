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
 *   Copyright (c) 2O19 David Linus Briemann (@Raging_Dave)
 *
 ********************************************************************************************/

#define LIBPARTIKEL_IMPLEMENTATION

#include "partikel.h"
#include "raylib.h"
#include "stdio.h"

// Global data.
//----------------------------------------------------------------------------------
static int           screenWidth  = 1000;
static int           screenHeight = 800;
static unsigned long counter      = 0;
static Camera2D      camera;

static Texture2D texCircle16;
static Texture2D texCircle8;
static Texture2D texCircle4;

static int activePS = 1;

static ParticleSystem * ps1              = NULL;
static Emitter *        emitterFountain1 = NULL;
static Emitter *        emitterFountain2 = NULL;
static Emitter *        emitterFountain3 = NULL;

static ParticleSystem * ps2           = NULL;
static Emitter *        emitterSwirl1 = NULL;
static Emitter *        emitterSwirl2 = NULL;
static Emitter *        emitterSwirl3 = NULL;

static ParticleSystem * ps3           = NULL;
static Emitter *        emitterFlame1 = NULL;
static Emitter *        emitterFlame2 = NULL;
static Emitter *        emitterFlame3 = NULL;

// Define a custom particle deactivator function.
bool Particle_DeactivatorFountain(Particle * p) {
	return (p->position.y > (camera.target.y + camera.offset.y)    // bottom
	        || p->position.x < (camera.target.x - camera.offset.x) // left
	        || p->position.x > (camera.target.x + camera.offset.x) // right
	        || Particle_DeactivatorAge(p));
}

bool Particle_DeactivatorOutsideCam(Particle * p) {
	return (p->position.y < (camera.target.y - camera.offset.y) || Particle_DeactivatorAge(p));
}

void OOMExit() {
	printf("OUT OF MEMORY.. BYE\n");
	exit(0);
}

void InitFountain() {
	ps1 = ParticleSystem_New();
	if (ps1 == NULL) {
		OOMExit();
	}

	EmitterConfig ecfg1 = {
		.capacity           = 600,
		.emissionRate       = 200,
		.origin             = (Vector2){.x = 0, .y = 0},
		.originAcceleration = (FloatRange){.min = 0, .max = 0},
		.direction          = (Vector2){.x = 0, .y = -1}, // go up
		.directionAngle     = (FloatRange){.min = -6, .max = 6}, // angle range -8 to +8 degree deviation from direction
		.velocityAngle      = (FloatRange){.min = 0, .max = 0},
		.velocity           = (FloatRange){.min = 700, .max = 730},
		.externalAcceleration = (Vector2){.x = 0, .y = 981},
		.startColor           = (Color){.r = 0, .g = 20, .b = 255, .a = 255},
		.endColor             = (Color){.r = 0, .g = 150, .b = 100, .a = 0},
		.age                  = (FloatRange){.min = 1.0, .max = 3.0},
		.texture              = texCircle16,
		.blendMode            = BLEND_ADDITIVE,

		.particle_Deactivator = Particle_DeactivatorFountain
    };
	emitterFountain1 = Emitter_New(ecfg1);
	if (emitterFountain1 == NULL) {
		OOMExit();
	}
	ParticleSystem_Register(ps1, emitterFountain1);

	ecfg1.directionAngle = (FloatRange){.min = -1.5, .max = 1.5};
	ecfg1.velocity       = (FloatRange){.min = 800, .max = 850};
	ecfg1.texture        = texCircle8;
	emitterFountain2     = Emitter_New(ecfg1);
	if (emitterFountain2 == NULL) {
		OOMExit();
	}
	ParticleSystem_Register(ps1, emitterFountain2);

	ecfg1.capacity       = 3000;
	ecfg1.emissionRate   = 1000;
	ecfg1.directionAngle = (FloatRange){.min = -20, .max = 20};
	ecfg1.velocity       = (FloatRange){.min = 500, .max = 550};
	ecfg1.texture        = texCircle16;
	ecfg1.age            = (FloatRange){.min = 0.0, .max = 3.0};
	emitterFountain3     = Emitter_New(ecfg1);
	if (emitterFountain3 == NULL) {
		OOMExit();
	}
	ParticleSystem_Register(ps1, emitterFountain3);

	ParticleSystem_Start(ps1);
}

void InitSwirl() {
	ps2 = ParticleSystem_New();
	if (ps2 == NULL) {
		OOMExit();
	}

	EmitterConfig ecfg = {
		.capacity           = 2500,
		.emissionRate       = 500,
		.origin             = (Vector2){.x = 0, .y = 0},
		.originAcceleration = (FloatRange){.min = 400, .max = 500},
		.offset             = (FloatRange){.min = 30, .max = 40},
		.direction          = (Vector2){.x = 0, .y = -1}, // go up
		.directionAngle     = (FloatRange){.min = -180, .max = 180},
		.velocityAngle      = (FloatRange){.min = 90, .max = 90},
		.velocity           = (FloatRange){.min = 200, .max = 500},
		.startColor         = (Color){.r = 244, .g = 20, .b = 0, .a = 255},
		.endColor           = (Color){.r = 244, .g = 20, .b = 0, .a = 0},
		.age                = (FloatRange){.min = 2.5, .max = 5.0},
		.texture            = texCircle8,
		.blendMode          = BLEND_ADDITIVE,

		.particle_Deactivator = Particle_DeactivatorOutsideCam
    };

	emitterSwirl1 = Emitter_New(ecfg);
	if (emitterSwirl1 == NULL) {
		OOMExit();
	}
	ParticleSystem_Register(ps2, emitterSwirl1);

	ecfg.capacity     = 1000;
	ecfg.emissionRate = 200;
	ecfg.offset       = (FloatRange){.min = 40, .max = 50};
	ecfg.startColor   = (Color){.r = 244, .g = 0, .b = 111, .a = 255};
	ecfg.endColor     = (Color){.r = 244, .g = 0, .b = 111, .a = 0};

	emitterSwirl2 = Emitter_New(ecfg);
	if (emitterSwirl2 == NULL) {
		OOMExit();
	}
	ParticleSystem_Register(ps2, emitterSwirl2);

	ecfg.capacity     = 150;
	ecfg.emissionRate = 30;
	ecfg.offset       = (FloatRange){.min = 20, .max = 30};
	ecfg.velocity     = (FloatRange){.min = 100, .max = 200};
	ecfg.startColor   = (Color){.r = 255, .g = 211, .b = 0, .a = 255};
	ecfg.endColor     = (Color){.r = 255, .g = 211, .b = 0, .a = 0};

	emitterSwirl3 = Emitter_New(ecfg);
	if (emitterSwirl3 == NULL) {
		OOMExit();
	}
	ParticleSystem_Register(ps2, emitterSwirl3);

	ParticleSystem_Start(ps2);
}

void InitFlame() {
	ps3 = ParticleSystem_New();
	if (ps3 == NULL) {
		OOMExit();
	}

	EmitterConfig ecfg = {
		.capacity           = 1000,
		.emissionRate       = 500,
		.origin             = (Vector2){.x = 0, .y = 0},
		.originAcceleration = (FloatRange){.min = 50, .max = 100},
		.offset             = (FloatRange){.min = 0, .max = 10},
		.direction          = (Vector2){.x = 0, .y = -1}, // go up
		.directionAngle     = (FloatRange){.min = -90, .max = -90},
		.velocityAngle      = (FloatRange){.min = 90, .max = 90},
		.velocity           = (FloatRange){.min = 30, .max = 150},
		.startColor         = (Color){.r = 255, .g = 20, .b = 0, .a = 255},
		.endColor           = (Color){.r = 255, .g = 20, .b = 0, .a = 0},
		.age                = (FloatRange){.min = 1.0, .max = 2.0},
		.texture            = texCircle16,
		.blendMode          = BLEND_ADDITIVE,

		.particle_Deactivator = Particle_DeactivatorFountain
    };

	emitterFlame1 = Emitter_New(ecfg);
	if (emitterFlame1 == NULL) {
		OOMExit();
	}
	ParticleSystem_Register(ps3, emitterFlame1);

	ecfg.capacity     = 20;
	ecfg.emissionRate = 20;
	ecfg.startColor   = (Color){.r = 255, .g = 255, .b = 255, .a = 255};
	ecfg.endColor     = (Color){.r = 255, .g = 255, .b = 255, .a = 0};
	ecfg.age          = (FloatRange){.min = 0.5, .max = 1.0};

	emitterFlame2 = Emitter_New(ecfg);
	if (emitterFlame2 == NULL) {
		OOMExit();
	}
	ParticleSystem_Register(ps3, emitterFlame2);

	ecfg.capacity           = 500;
	ecfg.emissionRate       = 100;
	ecfg.directionAngle     = (FloatRange){.min = -3, .max = 3};
	ecfg.velocityAngle      = (FloatRange){.min = 0, .max = 0};
	ecfg.originAcceleration = (FloatRange){.min = 0, .max = 0};
	ecfg.startColor         = (Color){.r = 125, .g = 125, .b = 125, .a = 30};
	ecfg.endColor           = (Color){.r = 125, .g = 125, .b = 125, .a = 10};
	ecfg.age                = (FloatRange){.min = 3.0, .max = 5.0};

	emitterFlame3 = Emitter_New(ecfg);
	if (emitterFlame3 == NULL) {
		OOMExit();
	}
	ParticleSystem_Register(ps3, emitterFlame3);

	ParticleSystem_Start(ps3);
}

void DestroyFountain() {
	Emitter_Free(emitterFountain1);
	Emitter_Free(emitterFountain2);
	Emitter_Free(emitterFountain3);

	ParticleSystem_Free(ps1);
}

void DestroySwirl() {
	Emitter_Free(emitterSwirl1);
	Emitter_Free(emitterSwirl2);
	Emitter_Free(emitterSwirl3);

	ParticleSystem_Free(ps2);
}

void DestroyFlame() {
	Emitter_Free(emitterFlame1);
	Emitter_Free(emitterFlame2);
	Emitter_Free(emitterFlame3);

	ParticleSystem_Free(ps3);
}

// Init sets up all relevant data.
void Init() {
	InitWindow(screenWidth, screenHeight, "libpartikel demo");
	SetTargetFPS(121);
	HideCursor();

	camera.target   = (Vector2){.x = 0, .y = 0};
	camera.offset   = (Vector2){.x = (float)screenWidth / 2, .y = (float)screenHeight / 2};
	camera.rotation = 0.0f;
	camera.zoom     = 1.0f;

	// Generate some simple textures.
	Image imgCircle16 = GenImageGradientRadial(16, 16, 0.3f, WHITE, BLACK);
	texCircle16       = LoadTextureFromImage(imgCircle16);
	Image imgCircle8  = GenImageGradientRadial(8, 8, 0.5f, WHITE, BLACK);
	texCircle8        = LoadTextureFromImage(imgCircle8);
	Image imgCircle4  = GenImageGradientRadial(4, 4, 0.5f, WHITE, BLACK);
	texCircle4        = LoadTextureFromImage(imgCircle4);

	UnloadImage(imgCircle4);
	UnloadImage(imgCircle8);
	UnloadImage(imgCircle16);

	InitFountain();
	InitSwirl();
	InitFlame();
}

// Destroy and free all the global data.
void Destroy() {
	DestroyFountain();
	DestroySwirl();
	DestroyFlame();

	UnloadTexture(texCircle4);
	UnloadTexture(texCircle8);
	UnloadTexture(texCircle16);

	// Close window and OpenGL context
	CloseWindow();
}

void Update(double dt) {
	Vector2 m = GetMousePosition();
	m.x -= (float)screenWidth / 2;
	m.y -= (float)screenHeight / 2;
	counter = 0;

	bool trigger = false;

	int key = GetKeyPressed();
	switch (key) {
	case KEY_ONE:
		activePS = 1;
		break;
	case KEY_TWO:
		activePS = 2;
		break;
	case KEY_THREE:
		activePS = 3;
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
		counter += ParticleSystem_Update(ps1, (float)dt);
		break;
	case 2:
		ParticleSystem_SetOrigin(ps2, (Vector2){.x = m.x, .y = m.y});
		counter += ParticleSystem_Update(ps2, (float)dt);
		break;
	case 3:
		ParticleSystem_SetOrigin(ps3, (Vector2){.x = m.x, .y = m.y});
		counter += ParticleSystem_Update(ps3, (float)dt);
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

	BeginMode2D(camera);

	// Draw scene here.
	switch (activePS) {
	case 1:
		ParticleSystem_Draw(ps1);
		break;
	case 2:
		ParticleSystem_Draw(ps2);
		break;
	case 3:
		ParticleSystem_Draw(ps3);
		break;
	default:
		break;
	}

	EndMode2D();

	// Draw HUD etc. here.
	DrawFPS(10, 10);
	char pcount[40];
	sprintf(pcount, "Particles: %lu", counter);
	DrawText(pcount, 10, 40, 20, DARKGREEN);
	sprintf(pcount, "Press number keys to switch demo.");
	DrawText(pcount, 580, 10, 20, DARKGREEN);
	sprintf(pcount, "1: Fountain / 2: Swirl / 3: Flame");
	DrawText(pcount, 580, 40, 20, DARKGREEN);

	EndDrawing();
}

int main(int argc, char * argv[argc + 1]) {

	(void)argv[0];

	// Initialization
	//----------------------------------------------------------------------------------
	Init();

	// Main game loop
	//----------------------------------------------------------------------------------
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		double dt = (double)GetFrameTime();
		Update(dt);

		Draw();
	}

	// De-Initialization
	//----------------------------------------------------------------------------------
	Destroy();

	return 0;
}
