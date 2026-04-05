#pragma once
#include <SDL_gpu.h>
#include "render/render.h"
#include <unordered_set>

extern bool dirtyParticles;

enum class ParticleType {
    Burst,
    Line,
    Sine
};

class Particle {
    public:
        SDL_Point gridPos;
        std::vector<Particle_Data> data;

        void move(SDL_Point newPos);

        ParticleType type;
        void update();

        Particle(SDL_Point gridPos, ParticleType type);
        ~Particle();
};

extern std::unordered_set<Particle*> particles;

void loadParticleQuad(SDL_GPUDevice* renderer);
void sortParticles(SDL_GPUDevice* renderer);
