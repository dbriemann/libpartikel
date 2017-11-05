/**********************************************************************************************
*
*   libpartikel v0.0.1 ALPHA
*   [https://github.com/dbriemann/libpartikel]
*
*
*   A simple particle system built with and for raylib, to be used as header only library.
*
*
*   FEATURES:
*       - Written in plain C code (C99) in PascalCase/camelCase notation
*       - Supports all platforms that raylib supports
*
*   DEPENDENCIES:
*       raylib and all of its dependencies
*
*   LICENSE: zlib/libpng
*
*   libpartikel is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   Copyright (c) 2017 David Linus Briemann (@Raging_Dave)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#pragma once

#include "stdlib.h"
#include "stdbool.h"
#include "math.h"
#include "raylib.h"


// TODO -- check all alloc return values for NULL
// TODO -- add non-continous emission


// Needed forward declarations.
//----------------------------------------------------------------------------------
typedef struct Particle Particle;


// Utility functions.
//----------------------------------------------------------------------------------

// GetRandomDouble returns a random double between 0.0 and 1.0.
double GetRandomDouble(double min, double max) {
    double range = max - min;
    double n = (double) GetRandomValue(0, RAND_MAX) / (double) RAND_MAX;
    return n*range + min;
}

// NormalizeV2 normalizes a 2d Vector and returns its unit vector.
Vector2 NormalizeV2(Vector2 v) {
    double len = sqrt(v.x*v.x + v.y*v.y);
    return (Vector2) {.x = v.x/len, .y = v.y/len};
}

// LinearFade fades from Color c1 to Color c2. Fraction is a value between 0 and 1.
// The interpolation is linear.
Color LinearFade(Color c1, Color c2, double fraction) {
    unsigned char newr = (unsigned char)((double)((int)c2.r - (int)c1.r) * fraction + (double)c1.r);
    unsigned char newg = (unsigned char)((double)((int)c2.g - (int)c1.g) * fraction + (double)c1.g);
    unsigned char newb = (unsigned char)((double)((int)c2.b - (int)c1.b) * fraction + (double)c1.b);
    unsigned char newa = (unsigned char)((double)((int)c2.a - (int)c1.a) * fraction + (double)c1.a);

    Color c = {
        .r = newr,
        .g = newg,
        .b = newb,
        .a = newa
    };

    return c;
}


// EmitterConfig type.
//----------------------------------------------------------------------------------
typedef struct EmitterConfig {
    size_t maxParticles;       // Maximum amounts of particles in the system.
    size_t emissionRate;        // Amount of particles emitted each second.
    Vector2 origin;             // Origin is the point, where the particles are emitted from.
    Vector2 emissionMin;        // Defines the minimum emission x,y.
    Vector2 emissionMax;        // Defines the maximum emission x,y.
    Vector2 acceleration;       // Constant acceleration. e.g. gravity.
    Color startColor;           // The color the particle starts with when it spawns.
    Color endColor;             // The color the particle ends with when it disappears.
    double ageMin;              // Minimum age of a particle in seconds.
    double ageMax;              // Maximum age of a particle in seconds.
    Texture2D texture;          // The texture used as particle texture.

    bool (*particle_Deactivator)(struct Particle *); // Pointer to a function that determines when
                                                     // a particle is deactivated.
} EmitterConfig;


// Particle type.
//----------------------------------------------------------------------------------

// Particle describes one particle in a particle system.
struct Particle {
    Vector2 position;       // Position of the particle in 2d space.
    Vector2 velocity;       // Velocity vector in 2d space.
    Vector2 acceleration;   // (Uniform) acceleration vector in 2d space.
    double age;             // Age is measured in seconds.
    double ttl;             // Ttl is the time to live in seconds.
    bool active;            // Inactive particles are neither updated nor drawn.

    bool (*particle_Deactivator)(struct Particle *); // Pointer to a function that determines
                                                     // when a particle is deactivated.
};

// Particle_DeactivatorAge is the default deactivator function that
// disables particles only if their age exceeds their time to live.
bool Particle_DeactivatorAge(Particle *p) {
    return p->age > p->ttl;
}

// Particle_new creates a new Particle object.
// The deactivator function may be omitted by passing NULL.
Particle * Particle_New(bool (*deactivatorFunc)(struct Particle *)) {
    Particle *p = calloc(1, sizeof(Particle));
    *p = (Particle){
        .position = (Vector2){.x = 0, .y = 0},
        .velocity = (Vector2){.x = 0, .y = 0},
        .acceleration = (Vector2){.x = 0, .y = 0},
        .age = 0,
        .ttl = 0,
        .active = false,

        .particle_Deactivator = Particle_DeactivatorAge
    };
    if(deactivatorFunc != NULL) {
        p->particle_Deactivator = deactivatorFunc;
    }

    return p;
}

// Particle_free frees all memory used by the Particle.
void Particle_Free(Particle *p) {
    free(p);
}

// Particle_Init inits a particle. It is then ready to be updated and drawn.
void Particle_Init(Particle *p, EmitterConfig *cfg) {
    p->position = cfg->origin;
    p->age = 0;
    double rx = GetRandomDouble(cfg->emissionMin.x, cfg->emissionMax.x);
    double ry = GetRandomDouble(cfg->emissionMin.y, cfg->emissionMax.y);
    p->velocity = (Vector2){.x = rx, .y = ry};
    p->acceleration = cfg->acceleration;
    p->ttl = GetRandomDouble(cfg->ageMin, cfg->ageMax);
    p->active = true;
}

// Particle_update updates all properties according to the delta time (in seconds).
// Deactivates the particle if the deactivator function returns true.
void Particle_Update(Particle *p, double dt) {
    if(!p->active) {
        return;
    }

    p->age += dt;

    if(p->particle_Deactivator(p)) {
        p->active = false;
        return;
    }    

    // Update velocity by acceleration.
    p->velocity.x += p->acceleration.x*dt;
    p->velocity.y += p->acceleration.y*dt;

    // Update position by velocity.
    p->position.x += p->velocity.x * dt;
    p->position.y += p->velocity.y * dt;
}


// Emitter type.
//----------------------------------------------------------------------------------

// Emitter is a single (point) source emitting many particles.
typedef struct Emitter {    
    EmitterConfig config;
    double mustEmit;            // Amount of particles to be emitted within next update call.
    Vector2 offset;             // Offset holds half the width and height of the texture.
    bool isEmitting;
    Particle **particles;       // Array of all particles (by pointer).
} Emitter;

// Emitter_New creates a new Emitter object.
Emitter * Emitter_New(EmitterConfig cfg) {
    Emitter *e = calloc(1, sizeof(Emitter));
    e->config = cfg;
    e->offset.x = e->config.texture.width/2;
    e->offset.y = e->config.texture.height/2;
    e->particles = calloc(e->config.maxParticles, sizeof(Particle));
    e->mustEmit = 0;

    for(size_t i = 0; i < e->config.maxParticles; i++) {
        e->particles[i] = Particle_New(e->config.particle_Deactivator);
    }

    return e;
}

void Emitter_Start(Emitter *e) {
    e->isEmitting = true;
}

void Emitter_Stop(Emitter *e) {
    e->isEmitting = false;
}

// Emitter_Free frees all allocated resources.
void Emitter_Free(Emitter *e) {
    free(e->particles);
    free(e);
}

// Emitter_Burst emits a specified amount of particles at once,
// ignoring the state of e->isEmitting. Use this for singular events
// instead of continuous output.
void Emitter_Burst(Emitter *e, size_t amount) {
    Particle *p = NULL;
    size_t emitted = 0;

    for(size_t i = 0; i < e->config.maxParticles; i++) {
        p = e->particles[i];
        if(!p->active) {
            Particle_Init(p, &e->config);
            p->position = e->config.origin;
            emitted++;
        }
        if(emitted >= amount) {
            return;
        }
    }
}

// Emitter_Update updates all particles and returns
// the current amount of active particles.
u_int32_t Emitter_Update(Emitter *e, double dt) {
    size_t emitNow = 0;
    Particle *p = NULL;
    u_int32_t counter = 0;

    if(e->isEmitting) {
        e->mustEmit += dt * (double)e->config.emissionRate;
        emitNow = (size_t)e->mustEmit; // floor
    }

    for(size_t i = 0; i < e->config.maxParticles; i++) {
        p = e->particles[i];
        if(p->active) {
            Particle_Update(p, dt);
            counter++;
        } else if(e->isEmitting && emitNow > 0) {
            // emit new particles here
            Particle_Init(p, &e->config);
            p->position = e->config.origin;
            Particle_Update(p, dt);
            emitNow--;
            e->mustEmit--;
            counter++;
        }
    }

    return counter;
}

// Emitter_Draw draws all active particles.
void Emitter_Draw(Emitter *e, BlendMode bm) {
    BeginBlendMode(bm);
    for(size_t i = 0; i < e->config.maxParticles; i++) {
        Particle *p = e->particles[i];
        if(p->active) {
            DrawTexture(e->config.texture,
                        e->particles[i]->position.x - e->offset.x,
                        e->particles[i]->position.y - e->offset.y,
                        LinearFade(e->config.startColor, e->config.endColor,p->age/p->ttl));
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
typedef struct ParticleSystem {
    bool active;
    Emitter *emitters;
} ParticleSystem;

// Particlesystem_New creates a new particle system
// with the given amount of emitters.
ParticleSystem * ParticleSystem_New(size_t emitters) {
    ParticleSystem *ps = calloc(1, sizeof(ParticleSystem));
    ps->active = false;
    ps->emitters = calloc(emitters, sizeof(Emitter));
    return ps;
}

void ParticleSystem_Free(ParticleSystem *p) {
    // TODO
    // run emitter->free
    //..
    // free emitter array
    //..
    // free particle system
    free(p);
}
