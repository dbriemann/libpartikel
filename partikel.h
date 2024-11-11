/**********************************************************************************************
 *
 *   libpartikel v0.0.3 ALPHA
 *   [https://github.com/dbriemann/libpartikel]
 *
 *
 *   A simple particle system built with and for raylib, to be used as header
 *only library.
 *
 *
 *   FEATURES:
 *       - Supports all platforms that raylib supports
 *
 *   DEPENDENCIES:
 *       raylib >= v2.5.0 and all of its dependencies
 *
 *   CONFIGURATION:
 *   #define LIBPARTIKEL_IMPLEMENTATION
 *       Generates the implementation of the library into the included file.
 *       If not defined, the library is in header only mode and can be included
 *in other headers or source files without problems. But only ONE file should
 *hold the implementation.
 *
 *   LICENSE: zlib/libpng
 *
 *   libpartikel is licensed under an unmodified zlib/libpng license, which is
 *an OSI-certified, BSD-like license that allows static linking with closed
 *source software:
 *
 *   Copyright (c) 2017 David Linus Briemann (@Raging_Dave)
 *
 *   This software is provided "as-is", without any express or implied warranty.
 *In no event will the authors be held liable for any damages arising from the
 *use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *including commercial applications, and to alter it and redistribute it freely,
 *subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *claim that you wrote the original software. If you use this software in a
 *product, an acknowledgment in the product documentation would be appreciated
 *but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not
 *be misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *distribution.
 *
 **********************************************************************************************/

#pragma once

#include "raylib.h"

/**  TODOs
 *
 * 0) MAYBE switch to purely function pointer based system.. handle Init,
 * Update, Draw etc. all as function pointers and make it possible to use a
 * custom one. See current handling of Particle deactivation functions as
 * example.
 *
 */

// -----------------------------------------------------------------------
// UNCOMMENT THE FOLLOWING LINE FOR DEVELOPMENT OF THIS HEADER FILE ONLY.
// If you don't most tools, such as lsp, static analysis, etc. might not work.
#define LIBPARTIKEL_IMPLEMENTATION
// -----------------------------------------------------------------------

// Allow custom memory allocators.
#ifndef PARTIKEL_ALLOC
	#define PARTIKEL_ALLOC(n,sz) calloc(n,sz)
#endif
#ifndef PARTIKEL_FREE
	#define PARTIKEL_FREE(p) free(p)
#endif

// Needed forward declarations.
//----------------------------------------------------------------------------------
typedef struct Particle Particle;
typedef struct EmitterConfig EmitterConfig;
typedef struct Emitter Emitter;
typedef struct ParticleSystem ParticleSystem;

// Function signatures (comments are found in implementation below)
//----------------------------------------------------------------------------------
float GetRandomFloat(float min, float max);
Vector2 NormalizeV2(Vector2 v);
Vector2 RotateV2(Vector2 v, float degrees);
Color LinearFade(Color c1, Color c2, float fraction);

bool Particle_DeactivatorAge(Particle *p);
Particle *Particle_New(bool (*deactivatorFunc)(struct Particle *));
void Particle_Free(Particle *p);
void Particle_Init(Particle *p, EmitterConfig *cfg);
void Particle_Update(Particle *p, float dt);

Emitter *Emitter_New(EmitterConfig cfg);
bool Emitter_Reinit(Emitter *e, EmitterConfig cfg);
void Emitter_Start(Emitter *e);
void Emitter_Stop(Emitter *e);
void Emitter_Free(Emitter *e);
void Emitter_Burst(Emitter *e);
unsigned long Emitter_Update(Emitter *e, float dt);
void Emitter_Draw(Emitter *e);

ParticleSystem *ParticleSystem_New(void);
bool ParticleSystem_Register(ParticleSystem *ps, Emitter *emitter);
bool ParticleSystem_Deregister(ParticleSystem *ps, Emitter *emitter);
void ParticleSystem_SetOrigin(ParticleSystem *ps, Vector2 origin);
void ParticleSystem_Start(ParticleSystem *ps);
void ParticleSystem_Stop(ParticleSystem *ps);
void ParticleSystem_Burst(ParticleSystem *ps);
void ParticleSystem_Draw(ParticleSystem *ps);
unsigned long ParticleSystem_Update(ParticleSystem *ps, float dt);
void ParticleSystem_Free(ParticleSystem *p);

#ifdef LIBPARTIKEL_IMPLEMENTATION

#include "math.h"
#include "stdlib.h"

// Utility functions & structs.
//----------------------------------------------------------------------------------

// GetRandomFloat returns a random float between 0.0 and 1.0.
float GetRandomFloat(float min, float max) {
  float range = max - min;
  float n = (float)GetRandomValue(0, RAND_MAX) / (float)RAND_MAX;
  return n * range + min;
}

// NormalizeV2 normalizes a 2d Vector and returns its unit vector.
Vector2 NormalizeV2(Vector2 v) {
  if (v.x == 0 && v.y == 0) {
    return v;
  }
  float len = sqrt(v.x * v.x + v.y * v.y);
  return (Vector2){.x = v.x / len, .y = v.y / len};
}

Vector2 RotateV2(Vector2 v, float degrees) {
  float rad = degrees * DEG2RAD;
  Vector2 res = {.x = cos(rad) * v.x - sin(rad) * v.y,
                 .y = sin(rad) * v.x + cos(rad) * v.y};
  return res;
}

// LinearFade fades from Color c1 to Color c2. Fraction is a value between 0
// and 1. The interpolation is linear.
Color LinearFade(Color c1, Color c2, float fraction) {
  unsigned char newr =
      (unsigned char)((float)((int)c2.r - (int)c1.r) * fraction + (float)c1.r);
  unsigned char newg =
      (unsigned char)((float)((int)c2.g - (int)c1.g) * fraction + (float)c1.g);
  unsigned char newb =
      (unsigned char)((float)((int)c2.b - (int)c1.b) * fraction + (float)c1.b);
  unsigned char newa =
      (unsigned char)((float)((int)c2.a - (int)c1.a) * fraction + (float)c1.a);

  Color c = {.r = newr, .g = newg, .b = newb, .a = newa};

  return c;
}

// Min/Max pair structs for various types.
typedef struct FloatRange {
  float min;
  float max;
} FloatRange;

typedef struct IntRange {
  int min;
  int max;
} IntRange;

// EmitterConfig type.
//----------------------------------------------------------------------------------
struct EmitterConfig {
  Vector2 direction;         // Direction vector will be normalized.
  FloatRange velocity;       // The possible range of the particle velocities.
                             // Velocity is a scalar defining the length of the
                             // direction vector.
  FloatRange directionAngle; // The angle range modiying the direction vector.
  FloatRange velocityAngle;  // The angle range to rotate the velocity vector.
  FloatRange
      offset; // The min and max offset multiplier for the particle origin.
  FloatRange originAcceleration; // An acceleration towards or from
                                 // (centrifugal) the origin.
  IntRange burst;                // The range of sudden particle bursts.
  size_t capacity;               // Maximum amounts of particles in the system.
  size_t emissionRate;           // Rate of emitted particles per second.
  Vector2 origin;                // Origin is the source of the emitter.
  Vector2 externalAcceleration; // External constant acceleration. e.g. gravity.
  Color startColor;    // The color the particle starts with when it spawns.
  Color endColor;      // The color the particle ends with when it disappears.
  FloatRange age;      // Age range of particles in seconds.
  BlendMode blendMode; // Color blending mode for all particles of this Emitter.
  Texture2D texture;   // The texture used as particle texture.

  bool (*particle_Deactivator)(
      struct Particle *); // Pointer to a function that determines when
                          // a particle is deactivated.
};

// Particle type.
//----------------------------------------------------------------------------------

// Particle describes one particle in a particle system.
struct Particle {
  Vector2 origin;               // The origin of the particle (never changes).
  Vector2 position;             // Position of the particle in 2d space.
  Vector2 velocity;             // Velocity vector in 2d space.
  Vector2 externalAcceleration; // Acceleration vector in 2d space.
  float originAcceleration;     // Accelerates velocity vector
  float age;                    // Age is measured in seconds.
  float ttl;                    // Ttl is the time to live in seconds.
  bool active; // Inactive particles are neither updated nor drawn.

  bool (*particle_Deactivator)(
      struct Particle *); // Pointer to a function that determines
                          // when a particle is deactivated.
};

// Particle_DeactivatorAge is the default deactivator function that
// disables particles only if their age exceeds their time to live.
bool Particle_DeactivatorAge(Particle *p) { return p->age > p->ttl; }

// Particle_new creates a new Particle object.
// The deactivator function may be omitted by passing NULL.
Particle *Particle_New(bool (*deactivatorFunc)(struct Particle *)) {
  Particle *p = PARTIKEL_ALLOC(1, sizeof(Particle));
  if (p == NULL) {
    return NULL;
  }
  *p = (Particle){.position = (Vector2){.x = 0, .y = 0},
                  .velocity = (Vector2){.x = 0, .y = 0},
                  .externalAcceleration = (Vector2){.x = 0, .y = 0},
                  .originAcceleration = 0,
                  .age = 0,
                  .ttl = 0,
                  .active = false,

                  .particle_Deactivator = Particle_DeactivatorAge};
  if (deactivatorFunc != NULL) {
    p->particle_Deactivator = deactivatorFunc;
  }

  return p;
}

// Particle_free frees all memory used by the Particle.
void Particle_Free(Particle *p) { PARTIKEL_FREE(p); }

// Particle_Init inits a particle. It is then ready to be updated and drawn.
void Particle_Init(Particle *p, EmitterConfig *cfg) {
  p->age = 0;
  p->origin = cfg->origin;

  // Get a random angle to find an random velocity.
  float randa =
      GetRandomFloat(cfg->directionAngle.min, cfg->directionAngle.max);

  // Rotate base direction with the given angle.
  Vector2 res = RotateV2(cfg->direction, randa);

  // Get a random value for velocity range (direction is normalized).
  float randv = GetRandomFloat(cfg->velocity.min, cfg->velocity.max);

  // Multiply direction with factor to set actual velocity in the Particle.
  p->velocity = (Vector2){.x = res.x * randv, .y = res.y * randv};

  // Get a random angle to rotate the velocity vector.
  randa = GetRandomFloat(cfg->velocityAngle.min, cfg->velocityAngle.max);

  // Rotate velocity vector with given angle.
  p->velocity = RotateV2(p->velocity, randa);

  // Get a random value for origin offset and apply it to position.
  float rando = GetRandomFloat(cfg->offset.min, cfg->offset.max);
  p->position.x = cfg->origin.x + res.x * rando;
  p->position.y = cfg->origin.y + res.y * rando;

  // Get a random value for the intrinsic particle acceleration
  float rands =
      GetRandomFloat(cfg->originAcceleration.min, cfg->originAcceleration.max);
  p->originAcceleration = rands;
  p->externalAcceleration = cfg->externalAcceleration;
  p->ttl = GetRandomFloat(cfg->age.min, cfg->age.max);
  p->active = true;
}

// Particle_update updates all properties according to the delta time (in
// seconds). Deactivates the particle if the deactivator function returns true.
void Particle_Update(Particle *p, float dt) {
  if (!p->active) {
    return;
  }

  p->age += dt;

  if (p->particle_Deactivator(p)) {
    p->active = false;
    return;
  }

  Vector2 toOrigin = NormalizeV2((Vector2){.x = p->origin.x - p->position.x,
                                           .y = p->origin.y - p->position.y});

  // Update velocity by internal acceleration.
  p->velocity.x += toOrigin.x * p->originAcceleration * dt;
  p->velocity.y += toOrigin.y * p->originAcceleration * dt;

  // Update velocity by external acceleration.
  p->velocity.x += p->externalAcceleration.x * dt;
  p->velocity.y += p->externalAcceleration.y * dt;

  // Update position by velocity.
  p->position.x += p->velocity.x * dt;
  p->position.y += p->velocity.y * dt;
}

// Emitter type.
//----------------------------------------------------------------------------------

// Emitter is a single (point) source emitting many particles.
struct Emitter {
  EmitterConfig config;
  float mustEmit; // Amount of particles to be emitted within next update call.
  Vector2 offset; // Offset holds half the width and height of the texture.
  bool isEmitting;
  Particle **particles; // Array of all particles (by pointer).
};

// Emitter_New creates a new Emitter object.
Emitter *Emitter_New(EmitterConfig cfg) {
  Emitter *e = PARTIKEL_ALLOC(1, sizeof(Emitter));
  if (e == NULL) {
    return NULL;
  }
  e->config = cfg;
  e->offset.x = e->config.texture.width / 2;
  e->offset.y = e->config.texture.height / 2;
  e->particles = PARTIKEL_ALLOC(e->config.capacity, sizeof(Particle *));
  if (e->particles == NULL) {
    PARTIKEL_FREE(e);
    return NULL;
  }
  e->mustEmit = 0;
  // Normalize direction for future uses.
  e->config.direction = NormalizeV2(e->config.direction);

  for (size_t i = 0; i < e->config.capacity; i++) {
    e->particles[i] = Particle_New(e->config.particle_Deactivator);
  }

  return e;
}

// Emitter_Reinit reinits the given Emitter with a new EmitterConfig.
bool Emitter_Reinit(Emitter *e, EmitterConfig cfg) {
  if (cfg.capacity > e->config.capacity) {
    // Array needs to be grown to the new size.
    Particle **newParticles =
        realloc(e->particles, cfg.capacity * sizeof(Particle *));
    if (newParticles == NULL) {
      return false;
    }
    e->particles = newParticles;

    // Create new Particles
    for (size_t i = e->config.capacity; i < cfg.capacity; i++) {
      e->particles[i] = Particle_New(cfg.particle_Deactivator);
    }
  } else if (cfg.capacity < e->config.capacity) {
    // First we free the now obsolete Particles.
    for (size_t i = cfg.capacity; i < e->config.capacity; i++) {
      Particle_Free(e->particles[i]);
    }

    // Array needs to be shrunk to the new size.
    Particle **newParticles =
        realloc(e->particles, cfg.capacity * sizeof(Particle *));
    if (newParticles == NULL) {
      return false;
    }
    e->particles = newParticles;
  }

  // Set new config.
  e->config = cfg;

  // Set new Particle deactivator function for all Particles.
  for (size_t i = 0; i < e->config.capacity; i++) {
    e->particles[i]->particle_Deactivator = e->config.particle_Deactivator;
  }

  return true;
}

// Emitter_Start activates Particle emission.
void Emitter_Start(Emitter *e) { e->isEmitting = true; }

// Emitter_Start deactivates Particle emission.
void Emitter_Stop(Emitter *e) { e->isEmitting = false; }

// Emitter_Free frees all allocated resources.
void Emitter_Free(Emitter *e) {
  for (size_t i = 0; i < e->config.capacity; i++) {
    Particle_Free(e->particles[i]);
  }
  free(e->particles);
  PARTIKEL_FREE(e);
}

// Emitter_Burst emits a specified amount of particles at once,
// ignoring the state of e->isEmitting. Use this for singular events
// instead of continuous output.
void Emitter_Burst(Emitter *e) {
  Particle *p = NULL;
  size_t emitted = 0;

  int amount = GetRandomValue(e->config.burst.min, e->config.burst.max);

  for (size_t i = 0; i < e->config.capacity; i++) {
    p = e->particles[i];
    if (!p->active) {
      Particle_Init(p, &e->config);
      p->position = e->config.origin;
      emitted++;
    }
    if (emitted >= amount) {
      return;
    }
  }
}

// Emitter_Update updates all particles and returns
// the current amount of active particles.
unsigned long Emitter_Update(Emitter *e, float dt) {
  size_t emitNow = 0;
  Particle *p = NULL;
  unsigned long counter = 0;

  if (e->isEmitting) {
    e->mustEmit += dt * (float)e->config.emissionRate;
    emitNow = (size_t)e->mustEmit; // floor
  }

  for (size_t i = 0; i < e->config.capacity; i++) {
    p = e->particles[i];
    if (p->active) {
      Particle_Update(p, dt);
      counter++;
    } else if (e->isEmitting && emitNow > 0) {
      // emit new particles here
      Particle_Init(p, &e->config);
      Particle_Update(p, dt);
      emitNow--;
      e->mustEmit--;
      counter++;
    }
  }

  return counter;
}

// Emitter_Draw draws all active particles.
void Emitter_Draw(Emitter *e) {
  BeginBlendMode(e->config.blendMode);
  for (size_t i = 0; i < e->config.capacity; i++) {
    Particle *p = e->particles[i];
    if (p->active) {
      DrawTexture(e->config.texture, e->particles[i]->position.x - e->offset.x,
                  e->particles[i]->position.y - e->offset.y,
                  LinearFade(e->config.startColor, e->config.endColor,
                             p->age / p->ttl));
    }
  }
  EndBlendMode();
}

// ParticleSystem type.
//----------------------------------------------------------------------------------

// ParticleSystem is a set of emitters grouped logically
// together to achieve a specific visual effect.
// While Emitters can be used independently, ParticleSystem
// offers some convenience for handling many Emitters at once.
struct ParticleSystem {
  bool active;
  size_t length;
  size_t capacity;
  Vector2 origin;
  Emitter **emitters;
};

// Particlesystem_New creates a new particle system
// with the given amount of emitters.
ParticleSystem *ParticleSystem_New(void) {
  ParticleSystem *ps = PARTIKEL_ALLOC(1, sizeof(ParticleSystem));
  if (ps == NULL) {
    return NULL;
  }
  ps->active = false;
  ps->length = 0;
  ps->capacity = 1;
  ps->origin = (Vector2){.x = 0, .y = 0};
  ps->emitters = PARTIKEL_ALLOC(ps->capacity, sizeof(Emitter *));
  if (ps->emitters == NULL) {
    PARTIKEL_FREE(ps);
    return NULL;
  }
  return ps;
}

// ParticleSystem_Register registers an emitter to the system.
// The emitter will be controlled by all particle system functions.
// Returns true on success and false otherwise.
bool ParticleSystem_Register(ParticleSystem *ps, Emitter *emitter) {
  // If there is no space for another emitter we have to realloc.
  if (ps->length >= ps->capacity) {
    // Double capacity.
    Emitter **newEmitters =
        realloc(ps->emitters, 2 * ps->capacity * sizeof(Emitter *));
    if (newEmitters == NULL) {
      return false;
    }
    ps->emitters = newEmitters;
    ps->capacity *= 2;
  }

  // Now the new Emitter can be registered.
  ps->emitters[ps->length] = emitter;
  ps->length++;

  return true;
}

// ParticleSystem_Deregister deregisters an Emitter by its pointer.
// Returns true on success and false otherwise.
bool ParticleSystem_Deregister(ParticleSystem *ps, Emitter *emitter) {
  for (size_t i = 0; i < ps->length; i++) {
    if (ps->emitters[i] == emitter) {
      // Remove this emitter by replacing its pointer with the
      // last pointer, if it is not the only Emitter.
      if (i != ps->length - 1) {
        ps->emitters[i] = ps->emitters[ps->length - 1];
      }
      // Then NULL the last emitter. It is either a duplicate or
      // the removed one.
      ps->length--;
      ps->emitters[ps->length] = NULL;

      return true;
    }
  }
  // Emitter not found.
  return false;
}

// ParticleSystem_SetOrigin sets the origin for all registered Emitters.
void ParticleSystem_SetOrigin(ParticleSystem *ps, Vector2 origin) {
  ps->origin = origin;
  for (size_t i = 0; i < ps->length; i++) {
    ps->emitters[i]->config.origin = origin;
  }
}

// ParticleSystem_Start runs Emitter_Start on all registered Emitters.
void ParticleSystem_Start(ParticleSystem *ps) {
  for (size_t i = 0; i < ps->length; i++) {
    Emitter_Start(ps->emitters[i]);
  }
}

// ParticleSystem_Stop runs Emitter_Stop on all registered Emitters.
void ParticleSystem_Stop(ParticleSystem *ps) {
  for (size_t i = 0; i < ps->length; i++) {
    Emitter_Stop(ps->emitters[i]);
  }
}

// ParticleSystem_Burst runs Emitter_Burst on all registered Emitters.
void ParticleSystem_Burst(ParticleSystem *ps) {
  for (size_t i = 0; i < ps->length; i++) {
    Emitter_Burst(ps->emitters[i]);
  }
}

// ParticleSystem_Draw runs Emitter_Draw on all registered Emitters.
void ParticleSystem_Draw(ParticleSystem *ps) {
  for (size_t i = 0; i < ps->length; i++) {
    Emitter_Draw(ps->emitters[i]);
  }
}

// ParticleSystem_Update runs Emitter_Update on all registered Emitters.
unsigned long ParticleSystem_Update(ParticleSystem *ps, float dt) {
  size_t counter = 0;
  for (size_t i = 0; i < ps->length; i++) {
    counter += Emitter_Update(ps->emitters[i], dt);
  }
  return counter;
}

// ParticleSystem_Free only frees its own resources.
// The emitters referenced here must be freed on their own.
void ParticleSystem_Free(ParticleSystem *p) {
  PARTIKEL_FREE(p->emitters);
  PARTIKEL_FREE(p);
}

#endif // LIBPARTIKEL_IMPLEMENTATION
