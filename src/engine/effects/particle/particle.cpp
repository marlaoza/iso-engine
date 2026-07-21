#include "particle.h"
#include "geometry/geometry.h"
#include "managers/frameManager/frameManager.h"
#include <colors.h>
#include <cstdlib>
#include <math.h>


std::unordered_set<Particle*> particles;
bool dirtyParticles;

Particle::Particle(SDL_Point gridPos, SDL_FPoint pixelPos, ParticleInfo info){
    this->gridPos = gridPos;
    this->type = info.type;
    this->constant = info.constant;
    this->fixed = info.fixed;
    this->amount = info.amount;
    this->generateParticles = info.generateParticles;

    this->pixelPos = pixelPos;

    this->isPlaying = false;

    this->speedX = 0;
    this->speedY = 0;
   
    particles.insert(this);
    dirtyParticles = true;
}

std::vector<Particle_Data> Particle::getData() {
    return this->data;
}

void Particle::play() {
    for(int i = 0; i < this->amount; i++){
        Particle_Data p = this->generateParticles(gridPos, pixelPos, i, this->speedX, this->speedY);
        this->data.push_back(p);
    }
    dirtyParticles = true;
    this->isPlaying = true;
}
void Particle::pause() {
    this->isPlaying = false;
}

void Particle::setSpeed(float x, float y){
    this->speedX = x;
    this->speedY = y;

    // for (int i = 0; i<this->data.size(); i++)
    // {
    //     Particle_Data& p = this->data[i];
    //     p = this->generateParticles(this->gridPos, this->pixelPos, i, this->speedX, this->speedY);        
    // }

}

void Particle::move(SDL_Point newGridPos, SDL_FPoint newPixelPos){
    if(fixed && isPlaying){
        
        for (Particle_Data& p : this->data)
        {
            SDL_FPoint offset = {p.particlePos.x - this->pixelPos.x, p.particlePos.y - this->pixelPos.y};
            p.gridX = newGridPos.x;
            p.gridY = newGridPos.y;
            p.particlePos = {newPixelPos.x + offset.x, newPixelPos.y + offset.y};
        }
        
        dirtyParticles = true;
    }
    this->gridPos = newGridPos;
    this->pixelPos = newPixelPos;
}

Particle::~Particle(){
    particles.erase(this);
    dirtyParticles = true;
}


void Particle::update(){
    if(isPlaying){
        int dC = 0;
        for (int i = 0; i<this->data.size(); i++)
        {
            Particle_Data& p = this->data[i];
            if(FRAME_TIME < p.instanceTime || FRAME_TIME - p.instanceTime >= p.lifeTime){
                if(constant){
                    p = this->generateParticles(this->gridPos, this->pixelPos, i, this->speedX, this->speedY);
                    dirtyParticles = true;
                }else{
                    dC ++;
                }
            
            }
        }

        if(dC >= this->data.size()){
            delete this;
        }
    }
}

void loadParticleQuad(SDL_GPUDevice* renderer){
    std::vector<Base_Vertex> vertices;
    std::vector<int> indices;

    IsoObject body = tiles[1 * BOARD_WIDTH + 1].tile;
                        
    vertices.push_back({{-0.5f, -0.5f}, {0.0f, 0.0f}});
    vertices.push_back({{ 0.5f, -0.5f}, {1.0f, 0.0f}});
    vertices.push_back({{-0.5f,  0.5f}, {0.0f, 1.0f}});
    vertices.push_back({{ 0.5f,  0.5f}, {1.0f, 1.0f}});
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

    renderLayers[PARTICLE_RENDER_LAYER]->writeBuffers(renderer, indices, vertices);    
}

void sortParticles(SDL_GPUDevice* renderer){

    std::vector<Particle_Data> fragments;
    for (Particle* p : particles)
    {for (Particle_Data pd : p->getData()){fragments.push_back(pd);}}
    renderLayers[PARTICLE_RENDER_LAYER]->writeBuffers(renderer, fragments);
    dirtyParticles = false;
}


ParticleInfo* explosionParticle;
ParticleInfo* trailParticle;

void loadParticlePrefabs() {
    explosionParticle = new ParticleInfo{
    true,
    false,
    ParticleType::Line,
    200,
    [](SDL_Point gridPos, SDL_FPoint pixelPos, int index, float speedX, float speedY) -> Particle_Data {
        Particle_Data p;
        float angle = ((float)rand() / (float)RAND_MAX) * 2.0f * M_PI;
        
        float speed = (float)(rand() % 50 + 25); 

        float vx = cosf(angle) * speed;
        float vy = sinf(angle) * speed;

        vy *= 0.5f;

        p = {
            {pixelPos.x, pixelPos.y},    
            gridPos.x, gridPos.y,        
            vx, vy,                      
            (float)FRAME_TIME,           
            1.0f,                       
            LIGHT_RED_50,    
            {0.5, 0, 0, 0},                     
            20.0f,                       
            0.0f                         
        };
        
        return p;
    }
    
};

    trailParticle = new ParticleInfo{
    true,
    true,
    ParticleType::Line,
    800,
    [](SDL_Point gridPos, SDL_FPoint pixelPos, int index, float speedX, float speedY) -> Particle_Data {
        Particle_Data p;
        float spiralAngle = index * 0.5f; 
        float radius = 2.0f + (float)(rand() % 4);
        
        float startOffsetX = cosf(spiralAngle) * radius;
        float startOffsetY = sinf(spiralAngle) * radius * 0.5f; 

        float vx = speedX * (100.0f + (float)(rand() % 40 - 20)); 
        float vy = speedY * (100.0f + (float)(rand() % 40 - 20)); 

        p = {
            {pixelPos.x + startOffsetX, pixelPos.y + startOffsetY},
            gridPos.x, gridPos.y,
            vx, vy,
            FRAME_TIME + (index % 100) * 0.002f,
            0.4f,              
            {1.0f, 0.8f, 0.2f, 0.8f}, 
            {0.5f, 0.0f, 0.5f, 0.0f},
            6.0f,               
            6.0f                
        };
        
        return p;
    }
    
};
}