#include "projectile.h"

bool dirtyProjectiles = false;

Projectile::Projectile(const ProjectileData& pData, SDL_Point origin, SDL_FPoint originOffset, SDL_Point end, SDL_FPoint endOffset) : Entity(){
    this->animations["idle"] = pData.animation;
    this->width = pData.width;
    this->height = pData.height;
    this->moveSpeed = pData.speed;
    this->baseAnimSpeed = 18;

    this->z = 0.0;

    this->gridPos = origin;
    this->gridOffset = originOffset;

    SDL_FPoint startPixel = tiles[origin.y*BOARD_WIDTH + origin.x].tile.surface[0];
    this->startPos = {startPixel.x + originOffset.x, startPixel.y + originOffset.y};
    this->startOffset = originOffset;

    SDL_FPoint endPixel = tiles[end.y*BOARD_WIDTH + end.x].tile.surface[0];
    this->endPos = {endPixel.x + endOffset.x, endPixel.y + endOffset.y};

    std::vector<SDL_Point> path;

    float dx = endPos.x - startPos.x;
    float dy = endPos.y - startPos.y;
    this->trajectoryDistance = sqrtf(dx*dx + dy*dy);
    this->trajectory = pData.trajectory;

    switch (pData.trajectory)
    {
        case ProjectileTrajectory::Line:
        case ProjectileTrajectory::Arc:
            path = getStraightPath(origin, end, 200);
        break;
        case ProjectileTrajectory::Path:
            path = getPath(origin, end, 200);
        break;
    
        default: break;
    }

    if(path.size() > 0)this->setPath(path);
    
    projectiles.insert(this);

    this->state = EntityState::Moving;
    dirtyProjectiles = true;
}

Projectile::~Projectile(){
    this->animations.clear();
    projectiles.erase(this);
    dirtyProjectiles = true;
}

Animation& Projectile::getCurrentAnimation(){return *this->animations["idle"];}

void Projectile::setTile(SDL_Point target){

    this->gridOffset = this->startOffset;
    this->gridPos = target;

};

void Projectile::move(){
    SDL_FPoint current = tiles[this->gridPos.y*BOARD_WIDTH + this->gridPos.x].tile.surface[0];
    SDL_FPoint currentPixelPosition = {current.x + this->gridOffset.x, current.y + this->gridOffset.y};

    if(this->trajectory == ProjectileTrajectory::Arc){
        float traveledX = currentPixelPosition.x - this->startPos.x;
        float traveledY = currentPixelPosition.y - this->startPos.y;
        float currentDist = sqrtf(traveledX*traveledX + traveledY*traveledY);
        float progress = currentDist / this->trajectoryDistance;
        if (progress > 1.0f) progress = 1.0f;
        this->z = sin(progress * M_PI) * 80.0f;
    }

    SDL_FPoint target;
    if (this->poolIndex < this->targetPool.size()) {
        target = {tiles[this->targetPos.y*BOARD_WIDTH+this->targetPos.x].tile.surface[0].x + this->startOffset.x, tiles[this->targetPos.y*BOARD_WIDTH+this->targetPos.x].tile.surface[0].y + this->startOffset.y};
    } else {
        target = this->endPos;
    }
    
    SDL_FPoint targetOffset = {target.x - current.x, target.y - current.y};
    float distX = this->gridOffset.x - targetOffset.x;
    float distY = this->gridOffset.y - targetOffset.y;
    float dist = sqrtf(distX*distX + distY*distY);

    if(
        (dist > 1.0f) 
        || 
        (!(this->poolIndex < this->targetPool.size()) && dist > 0.5f)
    ){ 
        this->gridOffset.x -= (distX / dist) * this->moveSpeed * DELTA_TIME;
        this->gridOffset.y -= (distY / dist) * this->moveSpeed * DELTA_TIME;
    } else {
        this->setTile(this->targetPos);
        this->poolIndex ++;
        if(this->poolIndex < this->targetPool.size()){
            this->targetPos = this->targetPool[this->poolIndex];
            this->direction = getDirection(this->gridPos, this->targetPos);
        }else{
            this->targetPool.clear();
            this->state = EntityState::Idle;
            projectileDeleteList.push_back(this);
        }
    }
    
    dirtyProjectiles = true;
}

std::unordered_set<Projectile*> projectiles;
std::vector<Projectile*> projectileDeleteList;

void sortProjectiles(SDL_GPUDevice* renderer){
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

    size_t maxVerts = projectiles.size() * 4 * sizeof(Entity_Vertex);
    size_t maxInds = projectiles.size() * 6 * sizeof(int);
    Uint32 totalSize = (Uint32)(maxVerts + maxInds);

    SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
    SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

    Uint8* buffPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

    int vertexOffset = 0;
    std::vector<Entity_Vertex> vertices;
    std::vector<int> indices;
    
    projectileIndexSize = 0;

    for (Projectile* p : projectiles)
    { 
        SDL_FPoint tileOrigin = tiles[p->gridPos.y*BOARD_WIDTH + p->gridPos.x].tile.surface[0];
        SDL_FPoint tl = {tileOrigin.x + p->gridOffset.x, tileOrigin.y + p->gridOffset.y};
 
        SDL_FPoint ptr = {tl.x, tl.y + (TILE_SIZE/2)};
        SDL_FPoint points_p[4] = {
            {ptr.x - p->width, ptr.y - p->height - p->z},
            {ptr.x + p->width, ptr.y - p->height - p->z},
            {ptr.x - p->width, ptr.y - p->z},
            {ptr.x + p->width, ptr.y - p->z},
        };

        int indexSum = 0;

        if(p->state == EntityState::Moving){
            SDL_FPoint normOffset = {fabs(p->gridOffset.x), fabs(p->gridOffset.y)};
            if(p->gridPos.x < p->targetPos.x){if(normOffset.x > (float)(TILE_SIZE/3.0f)){indexSum = 1;}}
            if(p->gridPos.x > p->targetPos.x){if(normOffset.x > (float)(TILE_SIZE/1.5f)){indexSum = -1;}}
            if(p->gridPos.y < p->targetPos.y){if(normOffset.y > (float)((TILE_SIZE)/2)/3.0f){indexSum = 1;}}
            if(p->gridPos.y > p->targetPos.y){if(normOffset.y > (float)((TILE_SIZE)/2)/1.5f){indexSum = -1;}}
        }

        const Animation& anim = p->getCurrentAnimation();
        SDL_FPoint UVS[4] = {
            {0.0f, 0.0f},{1.0f, 0.0f},
            {0.0f, 1.0f},{1.0f, 1.0f}
        };

        for (int i = 0; i < 4; i++){vertices.push_back({points_p[i], UVS[i], anim.sheet->id, anim.frameWidth, anim.frameHeight, anim.frames, 0, p->baseAnimSpeed * anim.speed, p->gridPos.x + p->gridPos.y, indexSum});}
        indices.push_back(vertexOffset);
        indices.push_back(vertexOffset + 1);
        indices.push_back(vertexOffset + 2);
        indices.push_back(vertexOffset + 1);
        indices.push_back(vertexOffset + 2);
        indices.push_back(vertexOffset + 3);

        vertexOffset+=4;
    }

    projectileIndexSize = (Uint32)indices.size();

    if(projectileIndexSize > 0){

        size_t vertSize = vertices.size() * sizeof(Entity_Vertex);
        size_t indexSize = indices.size() * sizeof(int);
        if(vertSize > 0){
            memcpy(buffPtr + 0, vertices.data(), vertSize);
        }
        if(indexSize > 0){
            memcpy((Uint8*)buffPtr + vertSize, indices.data(), indexSize);
        }

        SDL_GPUTransferBufferLocation vertSrc = { tbuf, 0 };
        SDL_GPUBufferRegion vertDst = { projectileVBuf,  0, (Uint32)vertSize};
        SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);

        SDL_GPUTransferBufferLocation indexSrc = { tbuf, (Uint32)vertSize };
        SDL_GPUBufferRegion indexDst = { projectileIBuf,  0, (Uint32)indexSize};
        SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDst, false);
        
        SDL_UnmapGPUTransferBuffer(renderer, tbuf);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
    }
    dirtyProjectiles = false;

}



ProjectileData* testProjectile;

void loadProjectilePrefabs(){
    testProjectile = new ProjectileData{
        11,
        22,
        90.0,
        ProjectileTrajectory::Arc,
        projectileAnimation,
    };
}