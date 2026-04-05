#include "particle.h"
#include "geometry/geometry.h"
#include "managers/frameManager/frameManager.h"


std::unordered_set<Particle*> particles;
bool dirtyParticles;

Particle::Particle(SDL_Point gridPos, ParticleType type){
    this->gridPos = gridPos;
    this->type = type;

    SDL_FPoint pixelPos = tiles[gridPos.y * BOARD_WIDTH + gridPos.x].tile.surface[0];
    for(int i = 0; i < 200; i++){

        Particle_Data p = {
            {pixelPos.x + i, pixelPos.y},
            gridPos.x, gridPos.y,
            10, -10,
            FRAME_TIME,
            10.0,
            2.5
        };

        this->data.push_back(p);
    }
    SDL_Log("criadas %d particulas", this->data.size());
    particles.insert(this);
    dirtyParticles = true;
}

void Particle::move(SDL_Point newPos){
    SDL_FPoint oldPixelPos = tiles[this->gridPos.y * BOARD_WIDTH + this->gridPos.x].tile.surface[0];
    SDL_FPoint newPixelPos = tiles[newPos.y * BOARD_WIDTH + newPos.x].tile.surface[0];
    this->gridPos = newPos;

    for (Particle_Data& p : this->data)
    {
        SDL_FPoint offset = {p.particlePos.x - oldPixelPos.x, p.particlePos.y - oldPixelPos.y};
        p.gridX = newPos.x;
        p.gridY = newPos.y;
        p.particlePos = {newPixelPos.x + offset.x, newPixelPos.y + offset.y};
    }
    
    dirtyParticles = true;
}

Particle::~Particle(){
    particles.erase(this);
    dirtyParticles = true;
}

void Particle::update(){

}

void loadParticleQuad(SDL_GPUDevice* renderer){
    std::vector<Particle_Vertex> vertices;
    std::vector<int> indices;

    particleIndexSize = 0;

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

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

    size_t vertSize = vertices.size() * sizeof(Particle_Vertex);
    size_t indexSize = indices.size() * sizeof(int);
    Uint32 totalSize = (Uint32)(vertSize + indexSize);

    if(totalSize > 0){

        SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
        SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

        Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);


        particleIndexSize = (Uint32)indices.size();

        if(vertSize > 0){
            memcpy(mapPtr + 0, vertices.data(), vertSize);
        }
        if(indexSize > 0){
            memcpy((Uint8*)mapPtr + vertSize, indices.data(), indexSize);
        }
       
        SDL_GPUTransferBufferLocation vertSrc = { tbuf, 0 };
        SDL_GPUBufferRegion vertDst = { particleVBuf,  0, (Uint32)vertSize};
        SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);

        SDL_GPUTransferBufferLocation indexSrc = { tbuf, (Uint32)vertSize };
        SDL_GPUBufferRegion indexDst = { particleIBuf,  0, (Uint32)indexSize};
        SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDst, false);
        
        SDL_UnmapGPUTransferBuffer(renderer, tbuf);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
    }
}

void sortParticles(SDL_GPUDevice* renderer){

    std::vector<Particle_Data> fragments;

    particleDataSize = 0;


    for (Particle* p : particles)
    {for (Particle_Data pd : p->data){fragments.push_back(pd);}}

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

    size_t fragSize = fragments.size() * sizeof(Particle_Data);
    Uint32 totalSize = (Uint32)(fragSize);

    if(totalSize > 0){

        SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
        SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

        Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);


        particleDataSize = (Uint32)fragments.size();

        if(fragSize > 0){
            memcpy(mapPtr + 0, fragments.data(), fragSize);
        }

        SDL_GPUTransferBufferLocation fragSrc = { tbuf, 0};
        SDL_GPUBufferRegion fragDst = { particleFBuf,  0, (Uint32)fragSize};
        SDL_UploadToGPUBuffer(copyPass, &fragSrc, &fragDst, false);
        
        SDL_UnmapGPUTransferBuffer(renderer, tbuf);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
    }
    dirtyParticles = false;
}

