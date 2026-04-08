#pragma once
#include <SDL_gpu.h>
#include "render/render.h"
#include <unordered_set>
#include <functional>

extern bool dirtyParticles;

enum class ParticleType {
    Burst,
    Line,
    Sine,
    None
};

struct ParticleInfo{
    bool fixed;
    bool constant;
    ParticleType type;
    int amount;
    std::function<Particle_Data(SDL_Point, SDL_FPoint, int, float, float)> generateParticles;
};

class Particle {
    public:
        void play();
        void pause();

        void move(SDL_Point newGridPos, SDL_FPoint newPixelPos);

        void setSpeed(float x, float y);
        
        void update();

        std::vector<Particle_Data> getData();

        Particle(SDL_Point gridPos, SDL_FPoint pixelPos, ParticleInfo info);
        ~Particle();
    private:
        bool isPlaying;
        int amount;
        bool constant;
        bool fixed;
        float speedX;
        float speedY;
        ParticleType type;
        std::function<Particle_Data(SDL_Point, SDL_FPoint, int, float, float)> generateParticles;
        SDL_Point gridPos;
        SDL_FPoint pixelPos;
        std::vector<Particle_Data> data;

};

extern std::unordered_set<Particle*> particles;

void loadParticleQuad(SDL_GPUDevice* renderer);
void sortParticles(SDL_GPUDevice* renderer);

extern ParticleInfo* explosionParticle;
extern ParticleInfo* trailParticle; 

void loadParticlePrefabs();


