#include "unit.h"
#include "SDL_rect.h"
#include <vector>
#include "constants.h"
#include "geometry.h"
#include <cmath>
#include "enginedata.h"
#include "map.h"
#include <cstdio>
#include "highlight.h"
#include "skill.h"


bool dirtyUnits = false;

 Unit::Unit(char name[32], SDL_Point gridPos, int height, int width, int maxHp, int maxSpeed) {
    this->id = units.size();
    memcpy(this->name,name, 32*sizeof(char));
    this->gridOffset = {0, 0};
    this->gridPos = gridPos;
    this->height = height;
    this->width = width;
    this->maxHp = maxHp;
    this->currentHp = maxHp;
    this->maxSpeed = maxSpeed;
    this->currentSpeed = maxSpeed;
    this->shield = 0;
    this->state = UnitState::Idle;
    this->direction = Direction::Down;
    this->selectedSkill = -1;
    units.push_back(this);
    unitMap[gridPos.y*BOARD_WIDTH + gridPos.x] = this;
    dirtyUnits = true;
    this->targetPos = {-1, -1};
    this->targetPool = {};
    this->poolIndex = 0;

    this->skills.push_back(new Skill(CreateMoveSkill()));
    this->skills.push_back(new Skill(CreateSampleSkill()));
    this->skills.push_back(new Skill(CreateMoveSkill()));
}

Unit::~Unit() {
    for (Skill* s : skills) {
        delete s;
    }
    skills.clear();

    unitMap[gridPos.y * BOARD_WIDTH + gridPos.x] = nullptr;
}

void Unit::select(){
    printf("%d | selecionado\n", this->id);
    SELECTED_UNIT = this;
}

void Unit::desselect(){
    if(this->state == UnitState::Casting || this->state == UnitState::Moving) return;
    if(SELECTED_UNIT == this){
        this->state = UnitState::Idle;
        for (SDL_Point p : this->reachMap){clearHighlight(p);}
        this->reachMap.clear();
        
        for (Skill* s :  this->skills)
        {
            for(SkillEffect& e : s->effects){
                for(std::vector<SDL_Point> pp : e.preview.actionLines){for(SDL_Point p : pp){clearHighlight(p);}}
                for(SDL_Point p : e.preview.affectedTiles){clearHighlight(p);}
                e.preview.affectedTiles.clear();
                e.preview.actionLines.clear();
            }
        }
        SELECTED_UNIT = nullptr;
    }
}

void Unit::hoverSkill(int skillId){
    if(this->state == UnitState::Casting || this->state == UnitState::Moving) return;
    Skill* skill = this->skills[skillId];
    
    int range = this->getSkillDependentValue(skill->rangeDep, skill->baseRange);

    this->calculateReachMap(range, skill->minRange);
    for (SDL_Point p : this->reachMap)
    {
        addHighlight(p, 1+skill->highlightPallete, skill->highlightPallete);
    }
    this->state = UnitState::Selecting;
        
}

void Unit::offHoverSkill(int skillId){
    if(this->state == UnitState::Selecting){
        for (SDL_Point p : this->reachMap){clearHighlight(p);}
        this->reachMap.clear();
        this->state = UnitState::Idle;
    }
        
}

void Unit::selectSkill(int skillId){
    if(this->state == UnitState::Casting || this->state == UnitState::Moving) return;
    Skill* skill = this->skills[skillId];

    int range = this->getSkillDependentValue(skill->rangeDep, skill->baseRange);    

    this->calculateReachMap(range, skill->minRange);
    for (SDL_Point p : this->reachMap)
    {
        addHighlight(p, 1+skill->highlightPallete, skill->highlightPallete);
    }
    this->state = UnitState::Casting;
    this->selectedSkill = skillId;
    
}

void Unit::setPath(std::vector<SDL_Point> path){
    if(path.size() <= 0) return;
    printf("%d | vai se mover %d casas\n",this->id, path.size());
    this->poolIndex = 0;
    this->targetPool = path;
    if(this->targetPool.size() > 0){
        this->targetPos = targetPool[0];
        this->direction = getDirection(this->gridPos, this->targetPos);
        this->state = UnitState::Moving;
    }
}

int Unit::getSkillDependentValue(SkillDependency dep, int base){
    switch (dep)
        {
        case SkillDependency::None :
            return base;
        case SkillDependency::Speed :
            return this->currentSpeed * base;
        default:
            return base;
        }
}

void Unit::calculatePreview(SDL_Point selectedTile){
    SDL_Point origin = selectedTile;

    if(
        this->state != UnitState::Casting
        ||
        selectedSkill < 0 || selectedSkill > (skills.size()-1)
    ) {return;}

    Skill* s = this->skills[selectedSkill];

    for(SkillEffect& e : s->effects){
        for(std::vector<SDL_Point> pp : e.preview.actionLines){for(SDL_Point p : pp){clearHighlight(p);}}
        for(SDL_Point p : e.preview.affectedTiles){clearHighlight(p);}
        e.preview.affectedTiles.clear();
        e.preview.actionLines.clear();
    }

    for (SDL_Point p : this->reachMap){addHighlight(p, 1+s->highlightPallete, s->highlightPallete);}

    if(s->origin == SkillOrigin::Self){
        origin = this->gridPos;
    }

    for (SkillEffect& e : s->effects)
    {

        EffectPreview currentEp;
        int range = this->getSkillDependentValue(e.radiusDep, e.baseRadius);
        currentEp.affectedTiles = getSkillEffectArea(e, origin, Direction::Left, range);
        
        currentEp.actionLines.clear();
        for (SDL_Point p : currentEp.affectedTiles)
        {   
            Unit* pTarget = unitMap[p.y * BOARD_WIDTH + p.x];
            int range2 = this->getSkillDependentValue(e.powerDep, e.basePower);
            std::vector<SDL_Point> actionLine;

            if (e.type == EffectType::Pathfind) {
                if (e.target == EffectTarget::Self) {
                    actionLine = getPath(this->gridPos, selectedTile, range2);
                } 
                else if (e.target == EffectTarget::Unit && pTarget != nullptr) {
                    actionLine = getPath(pTarget->gridPos, this->gridPos, range2);
                }
            } 
            else if (e.type == EffectType::Move && pTarget != nullptr) {
                if(origin.x == pTarget->gridPos.x && origin.y == pTarget->gridPos.y){
                    origin = this->gridPos;
                }
                Direction pushDir = getDirection(pTarget->gridPos, origin);
                actionLine = getLine(pTarget->gridPos, pushDir, range2, 0, true);
            }

            currentEp.actionLines.push_back(actionLine);

        }

        e.preview = currentEp;
        for(std::vector<SDL_Point> pp : e.preview.actionLines){for(SDL_Point p : pp){addHighlight(p, 2+s->highlightPallete, s->highlightPallete);}}
        for(SDL_Point p : e.preview.affectedTiles){addHighlight(p, 3+s->highlightPallete, s->highlightPallete);}
    }

    
}

void Unit::castSkill(int skillId){
    if(this->state == UnitState::Casting){
        if(selectedSkill < 0 || selectedSkill > (skills.size()-1)) return;
        Skill* s = this->skills[selectedSkill];

        printf("%d | usou a skill %s",this->id, s->name);

        for (SkillEffect& e : s->effects)
        {
            EffectPreview ep = e.preview;

            int j = 0;
            for (SDL_Point p : ep.affectedTiles)
            {
                Unit* pTarget = unitMap[p.y * BOARD_WIDTH + p.x];
                if(pTarget == nullptr)continue;
                printf(" em %d\n",pTarget->id);

                switch(e.type){
                    case EffectType::Move:
                    case EffectType::Pathfind:
                        pTarget->setPath(ep.actionLines[j]);
                        break;
                    case EffectType::Create:
                        break;
                    case EffectType::Damage:
                        break;
                    default:break;
                }
                j++;
            }

            for(std::vector<SDL_Point> pp : e.preview.actionLines){for(SDL_Point p : pp){clearHighlight(p);}}
            for(SDL_Point p : e.preview.affectedTiles){clearHighlight(p);}
            e.preview.affectedTiles.clear();
            e.preview.actionLines.clear();
            
        }

        for (SDL_Point p : this->reachMap){clearHighlight(p);}

        if(this->state != UnitState::Moving){
            this->state = UnitState::Idle;
        }
    }
}

void Unit::move(){
    SDL_FPoint target = tiles[this->targetPos.y*BOARD_WIDTH + this->targetPos.x].tile.surface[0];
    SDL_FPoint current = tiles[this->gridPos.y*BOARD_WIDTH + this->gridPos.x].tile.surface[0];
    SDL_FPoint targetOffset = {target.x - current.x, target.y - current.y};
    float distX = this->gridOffset.x - targetOffset.x;
    float distY = this->gridOffset.y - targetOffset.y;
    float dist = sqrtf(distX*distX + distY*distY);
    
    if(dist > 0.5f){ 
        this->gridOffset.x -= (distX / dist) * 40.0f * DELTA_TIME;
        this->gridOffset.y -= (distY / dist) * 40.0f * DELTA_TIME;
    } else {
        this->setTile(this->targetPos);
        this->poolIndex ++;
        if(this->poolIndex < this->targetPool.size()){
            this->targetPos = this->targetPool[this->poolIndex];
            this->direction = getDirection(this->gridPos, this->targetPos);
        }else{
            this->state = UnitState::Idle;
            this->targetPos = {-1,-1};
            this->poolIndex = 0;
            this->selectedSkill = -1;
            for (SDL_Point p : this->targetPool){clearHighlight(p);}
            this->targetPool.clear();
        }
    }
    dirtyUnits = true;
}

void Unit::calculateReachMap(int size, int minSize){
    this->reachMap.clear();
    this->reachMap = getDiamond(this->gridPos, size, minSize);
};

void Unit::setTile(SDL_Point target){
    SDL_Point oldPos = this->gridPos;
    unitMap[this->gridPos.y*BOARD_WIDTH + this->gridPos.x] = nullptr;
    unitMap[target.y*BOARD_WIDTH + target.x] = this;
    this->gridOffset = {0, 0};
    this->gridPos = target;
};

Unit* SELECTED_UNIT;
Unit* HOVERED_UNIT;

std::vector<Unit*> units;
Unit* unitMap[BOARD_WIDTH * BOARD_HEIGHT];

Geometry<Entity_Vertex> unitGeometry[BOARD_WIDTH + BOARD_HEIGHT - 1];

void sortUnits(SDL_GPUDevice* renderer){
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

    size_t maxVerts = units.size() * 4 * sizeof(Entity_Vertex);
    size_t maxInds = units.size() * 6 * sizeof(int);
    Uint32 totalSize = (Uint32)(maxVerts + maxInds);

    SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
    SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

    Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
    Uint32 indexCount = 0;
    Uint32 vertOffset = 0;
    Uint32 indexOffset = 0;
    Uint32 vertexAmt = 0;
    for (int i = 0; i < BOARD_HEIGHT + BOARD_WIDTH - 1; i++)
    {   
        unitLayer[i].index = indexCount;
        unitLayer[i].size = (Uint32)unitGeometry[i].indices.size();
        indexCount += unitLayer[i].size;

        size_t vertSize = unitGeometry[i].vertices.size() * sizeof(Entity_Vertex);
        size_t indexSize = unitGeometry[i].indices.size() * sizeof(int);

        unitLayer[i].vertex = (Sint32)vertexAmt;

        if(vertSize > 0){
            memcpy(mapPtr + vertOffset, unitGeometry[i].vertices.data(), vertSize);
            vertOffset += vertSize;
        }

        if(indexSize > 0){
            Uint32 tbufIndexStart = (Uint32)maxVerts + indexOffset;
            memcpy((Uint8*)mapPtr + tbufIndexStart, unitGeometry[i].indices.data(), indexSize);
            indexOffset+=indexSize;
        }
        vertexAmt += (Uint32)unitGeometry[i].vertices.size();

        
    }

    SDL_GPUTransferBufferLocation vertSrc = { tbuf, 0 };
    SDL_GPUBufferRegion vertDst = { unitVBuf,  0, vertOffset};
    SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);

    SDL_GPUTransferBufferLocation indexSrc = { tbuf, (Uint32)maxVerts };
    SDL_GPUBufferRegion indexDst = { unitIBuf,  0, indexOffset};
    SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDst, false);
    
    SDL_UnmapGPUTransferBuffer(renderer, tbuf);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
}

void calculateUnitPoints(SDL_GPUDevice* renderer){
    for(int i = 0; i < BOARD_WIDTH + BOARD_HEIGHT - 1; i++) {
        unitGeometry[i].vertices.clear();
        unitGeometry[i].indices.clear();
    }
    int vertexOffset[BOARD_WIDTH + BOARD_HEIGHT - 1] = {};

    for (int i = 0; i < units.size(); i++)
    { 
        Unit* u = units[i];
            
        SDL_FPoint tileOrigin = tiles[u->gridPos.y*BOARD_WIDTH + u->gridPos.x].tile.surface[0];
        SDL_FPoint tl = {tileOrigin.x + u->gridOffset.x, tileOrigin.y + u->gridOffset.y};
 
        SDL_FPoint ptr = {tl.x, tl.y + (TILE_SIZE - u->width)};
        SDL_FPoint points_p[4] = {
            {ptr.x - u->width, ptr.y - u->height},
            {ptr.x + u->width +1, ptr.y - u->height},
            {ptr.x - u->width, ptr.y},
            {ptr.x + u->width +1, ptr.y},
        };
   

        int index = u->gridPos.x + u->gridPos.y;
        int indexSum = 0;

        if(u->state == UnitState::Moving){
            SDL_FPoint normOffset = {fabs(u->gridOffset.x), fabs(u->gridOffset.y)};
            if(u->gridPos.x < u->targetPos.x){if(normOffset.x > (float)(TILE_SIZE/3.0f)){indexSum = 1;}}
            if(u->gridPos.x > u->targetPos.x){if(normOffset.x > (float)(TILE_SIZE/1.5f)){indexSum = -1;}}
            if(u->gridPos.y < u->targetPos.y){if(normOffset.y > (float)((TILE_SIZE)/2)/3.0f){indexSum = 1;}}
            if(u->gridPos.y > u->targetPos.y){if(normOffset.y > (float)((TILE_SIZE)/2)/1.5f){indexSum = -1;}}
            index += indexSum;
            if(index >= BOARD_WIDTH + BOARD_HEIGHT - 1) index = BOARD_WIDTH + BOARD_HEIGHT - 2;
            if(index < 0) index = 0;
        }

        
      
        unitGeometry[index].vertices.push_back({points_p[0], {0.0f, 0.0f}, 1});
        unitGeometry[index].vertices.push_back({points_p[1], {1.0f, 0.0f}, 1});
        unitGeometry[index].vertices.push_back({points_p[2], {0.0f, 1.0f}, 1});
        unitGeometry[index].vertices.push_back({points_p[3], {1.0f, 1.0f}, 1});

        unitGeometry[index].indices.push_back(vertexOffset[index]);
        unitGeometry[index].indices.push_back(vertexOffset[index] + 1);
        unitGeometry[index].indices.push_back(vertexOffset[index] + 2);
        unitGeometry[index].indices.push_back(vertexOffset[index] + 1);
        unitGeometry[index].indices.push_back(vertexOffset[index] + 2);
        unitGeometry[index].indices.push_back(vertexOffset[index] + 3);

        vertexOffset[index]+=4;
    }

    sortUnits(renderer);
    dirtyUnits = false;

}

