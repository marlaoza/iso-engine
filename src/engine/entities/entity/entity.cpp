#include "entity.h"
#include "managers/frameManager/frameManager.h"
#include "highlight/highlight.h"
#include <cmath>


Entity::Entity(){
    this->currentClip = nullptr;
    this->clipStartFrame = 0;
    
    this->targetPos = {-1, -1};
    this->targetPool = {};
    this->poolIndex = 0;

    this->moveSpeed = 70.0f;

    this->direction = Direction::Down;
    this->state = EntityState::Idle;
    this->baseAnimSpeed = 1;
}
void Entity::setTile(SDL_Point target){
    this->lastPos = gridPos;
    this->gridOffset = {0, 0};
    this->gridPos = target;
};

void Entity::playClip(std::string clipName){
    this->currentClip = this->animations[clipName];
    this->clipStartFrame = FRAME_TIME;
}

int Entity::getClipStartFrame(){return this->clipStartFrame;}

Animation& Entity::getCurrentAnimation(){
    if(this->currentClip != nullptr){return *this->currentClip;}
    return *this->animations["idle"];
}

void Entity::setPath(std::vector<SDL_Point> path){
    if(path.size() <= 0) return;
    this->poolIndex = 0;
    this->targetPool = std::move(path);
    if(this->targetPool.size() > 0){
        this->targetPos = targetPool[0];
        this->direction = getDirection(this->gridPos, this->targetPos);
        this->state = EntityState::Moving;
    }
}

void Entity::move(){
    SDL_FPoint target = tiles[this->targetPos.y*BOARD_WIDTH + this->targetPos.x].tile.surface[0];
    SDL_FPoint current = tiles[this->gridPos.y*BOARD_WIDTH + this->gridPos.x].tile.surface[0];
    SDL_FPoint targetOffset = {target.x - current.x, target.y - current.y};
    float distX = this->gridOffset.x - targetOffset.x;
    float distY = this->gridOffset.y - targetOffset.y;
    float dist = sqrtf(distX*distX + distY*distY);
    
    int cMoveSpeed = this->moveSpeed;
    if(this->state == EntityState::ForcedMoving){cMoveSpeed *= 2;}

    float step = cMoveSpeed * DELTA_TIME;

    if(dist > step && dist > 0.5f){ 
        this->gridOffset.x -= (distX / dist) * step;
        this->gridOffset.y -= (distY / dist) * step;
    } else {
        this->setTile(this->targetPos);
        this->poolIndex ++;
        if(this->poolIndex < this->targetPool.size()){
            this->targetPos = this->targetPool[this->poolIndex];
            this->direction = getDirection(this->gridPos, this->targetPos);
        }else{
            this->state = EntityState::Idle;
            this->targetPos = {-1,-1};
            this->poolIndex = 0;
            for (SDL_Point p : this->targetPool){clearHighlight(p);}
            this->targetPool.clear();
        }
    }
}

void Entity::update(){
    if(this->currentClip != nullptr){
        float frameCheck = FRAME_TIME;
        if(FRAME_TIME < this->getClipStartFrame()){frameCheck += 50.0f;}
        if(frameCheck - this->getClipStartFrame() >= this->currentClip->frames){ this->currentClip = nullptr;}
    }

    if(this->state == EntityState::Moving || this->state == EntityState::ForcedMoving) this->move();
}