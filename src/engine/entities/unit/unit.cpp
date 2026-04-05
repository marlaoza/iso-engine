#include "unit.h"

#include "SDL_rect.h"
#include <vector>
#include <cmath>
#include <cstdio>

#include "managers/frameManager/frameManager.h"

#include "entities/projectile/projectile.h"

#include "highlight/highlight.h"
#include "map/map.h"


bool dirtyUnits = false;

 Unit::Unit(const std::string& name, SDL_Point gridPos, const UnitData& uData) : Entity() {
    this->id = units.size();
    this->name = name;
    this->gridOffset = {0, 0};
    this->gridPos = gridPos;
    this->height = uData.baseHeight;
    this->width = uData.baseWidth;
    this->maxHp = uData.baseHP;
    this->currentHp = this->maxHp;
    this->maxSpeed = uData.baseSpeed;
    this->currentSpeed = this->maxSpeed;
    this->shield = uData.baseShield;
    this->animations = uData.animations;
    this->expressionSheet = uData.expressionSheet;
    this->selectedSkill = -1;

    this->baseAnimSpeed = 2;

    this->skills.push_back(moveSkill);
    this->skills.push_back(grenadeSkill);

    units.insert(this);
    unitMap[gridPos.y*BOARD_WIDTH + gridPos.x] = this;
    dirtyUnits = true;
}

Unit::~Unit() {
    skills.clear();
    unitMap[gridPos.y * BOARD_WIDTH + gridPos.x] = nullptr;
    units.erase(this);
    dirtyUnits = true;
}

void Unit::select(){
    printf("%d | selecionado\n", this->id);
    SELECTED_UNIT = this;
}

void Unit::desselect(){
    if(this->state == EntityState::Casting || this->state == EntityState::Moving) return;
    if(SELECTED_UNIT == this){
        this->state = EntityState::Idle;
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
    this->offHoverSkill();
        
    if(this->state == EntityState::Casting || this->state == EntityState::Moving || this->state == EntityState::ForcedMoving || this->skills.size() <= 0 || this->skills.size() <= skillId) return;
    Skill* skill = this->skills[skillId];
    
    int range = this->getSkillDependentValue(skill->rangeDep, skill->baseRange);

    this->calculateReachMap(range, skill->minRange);
    for (SDL_Point p : this->reachMap)
    {
        addHighlight(p, 1+skill->highlightPallete, skill->highlightPallete);
    }
    this->state = EntityState::Selecting;
        
}

void Unit::offHoverSkill(){
    if(this->state == EntityState::Selecting){
        for (SDL_Point p : this->reachMap){clearHighlight(p);}
        this->reachMap.clear();
        this->state = EntityState::Idle;

    }
        
}

void Unit::selectSkill(int skillId){
    if(this->state == EntityState::Casting || this->state == EntityState::Moving || this->state == EntityState::ForcedMoving || this->skills.size() <= 0) return;
    Skill* skill = this->skills[skillId];

    int range = this->getSkillDependentValue(skill->rangeDep, skill->baseRange);    

    this->calculateReachMap(range, skill->minRange);
    for (SDL_Point p : this->reachMap)
    {
        addHighlight(p, 1+skill->highlightPallete, skill->highlightPallete);
    }
    this->state = EntityState::Casting;
    this->selectedSkill = skillId;
    
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
        this->state != EntityState::Casting
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

    bool found = false;
    for (SDL_Point p : this->reachMap){
        if(p.x == selectedTile.x && p.y == selectedTile.y){found = true;}
        addHighlight(p, 1+s->highlightPallete, s->highlightPallete);
    }
    if(!found){return;}

    Unit* targetOrigin = unitMap[selectedTile.y * BOARD_WIDTH + selectedTile.x];

    if(s->origin == SkillOrigin::Unit && targetOrigin == nullptr){ return;}
    if(s->origin == SkillOrigin::Caster){origin = this->gridPos;}

    for (SkillEffect& e : s->effects)
    {
        SDL_Point effectTarget = selectedTile;

        EffectPreview currentEp;
        int range = this->getSkillDependentValue(e.radiusDep, e.baseRadius);
        currentEp.affectedTiles = getSkillEffectArea(e, origin, Direction::Left, range);
        
        currentEp.actionLines.clear();
        for (SDL_Point p : currentEp.affectedTiles)
        {   
            Unit* pTarget = unitMap[p.y * BOARD_WIDTH + p.x];
            if(e.target == EffectTarget::Unit && pTarget == nullptr){continue;}
            int range2 = this->getSkillDependentValue(e.powerDep, e.basePower);
            std::vector<SDL_Point> actionLine;

            SDL_Point start = p;
            SDL_Point end = selectedTile;

            switch (e.type)
            {
                case EffectType::Pathfind:
                    if(e.target == EffectTarget::Unit){end = this->gridPos;}
                    else if(e.target == EffectTarget::Caster){start = this->gridPos;}
                    actionLine = getPath(start, end, range2);
                break;

                case EffectType::Move:
                {
                    Direction pushDir = getDirection(p, origin);
                    actionLine = getLine(p, pushDir, range2, 0, true);
                }
                break;
                
                default:
                    continue;
                break;
            }

            currentEp.actionLines.push_back(actionLine);

        }

        e.preview = currentEp;
        for(SDL_Point p : e.preview.affectedTiles){addHighlight(p, 3+s->highlightPallete, s->highlightPallete);}
        for(std::vector<SDL_Point> pp : e.preview.actionLines){for(SDL_Point p : pp){addHighlight(p, 2+s->highlightPallete, s->highlightPallete);}}
    }

    
}

void applySkillEffects(Unit* caster, Skill* s){
    for (SkillEffect& e : s->effects)
    {
        EffectPreview ep = e.preview;
        int j = 0;
        for (SDL_Point p : ep.affectedTiles)
        {
            Unit* pTarget = unitMap[p.y * BOARD_WIDTH + p.x];
            if(e.target == EffectTarget::Caster){pTarget = caster;}
            if(e.target != EffectTarget::Tile && pTarget == nullptr) continue;
            printf(" em %d\n",pTarget->id);
            switch(e.type){
                case EffectType::Move:
                case EffectType::Pathfind:
                    pTarget->setPath(ep.actionLines[j]);
                    if(e.target == EffectTarget::Unit) pTarget->state = EntityState::ForcedMoving;
                    break;
                case EffectType::Create:
                    break;
                case EffectType::Damage:
                    break;
                default:break;
            }
            j++;
        }
        
        e.preview.affectedTiles.clear();
        e.preview.actionLines.clear();
    }
}

void Unit::castSkill(int skillId, SDL_Point selectedTile){
    if(this->state == EntityState::Casting){
        if(selectedSkill < 0 || selectedSkill > (skills.size()-1)) return;
        Skill* s = this->skills[selectedSkill];

        bool found = false;

        for (SDL_Point p : this->reachMap){
            if(p.x == selectedTile.x && p.y == selectedTile.y){found = true;}
            clearHighlight(p);
        }

        for (SkillEffect& e : s->effects){
            for(std::vector<SDL_Point> pp : e.preview.actionLines){for(SDL_Point p : pp){clearHighlight(p);}}
            for(SDL_Point p : e.preview.affectedTiles){clearHighlight(p);}
        }

        if(found){
            printf("%d | usou a skill %s",this->id, s->name.c_str());

            if(s->projectileData != nullptr){
                Projectile* p = new Projectile(*s->projectileData, this->gridPos, {0, 0}, selectedTile, {0,0});
                p->onLand = [this, s](){
                    applySkillEffects(this, s);
                };
                
            }
            else{
                applySkillEffects(this, s);
            }
        }

        if(this->state != EntityState::Moving && this->state != EntityState::ForcedMoving){
            this->state = EntityState::Idle;
        }
        this->selectedSkill = -1;
    }
}

void Unit::move() {
    Entity::move();
    dirtyUnits = true;
}

void Unit::calculateReachMap(int size, int minSize){
    this->reachMap.clear();
    this->reachMap = getDiamond(this->gridPos, size, minSize);
};

void Unit::setTile(SDL_Point target){
    Entity::setTile(target);
    unitMap[this->lastPos.y*BOARD_WIDTH + this->lastPos.x] = nullptr;
    unitMap[target.y*BOARD_WIDTH + target.x] = this;
};


Unit* SELECTED_UNIT;
Unit* HOVERED_UNIT;

std::unordered_set<Unit*> units;
Unit* unitMap[BOARD_WIDTH * BOARD_HEIGHT];

void sortUnits(SDL_GPUDevice* renderer){
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

    size_t maxVerts = units.size() * 4 * sizeof(Entity_Vertex);
    size_t maxInds = units.size() * 6 * sizeof(int);
    Uint32 totalSize = (Uint32)(maxVerts + maxInds);

    SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
    SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

    Uint8* buffPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

    int vertexOffset = 0;
    std::vector<Entity_Vertex> vertices;
    std::vector<int> indices;
    
    unitIndexSize = 0;

    for (Unit* u : units)
    { 
        SDL_FPoint tileOrigin = tiles[u->gridPos.y*BOARD_WIDTH + u->gridPos.x].tile.surface[0];
        SDL_FPoint tl = {tileOrigin.x + u->gridOffset.x, tileOrigin.y + u->gridOffset.y};
 
        SDL_FPoint ptr = {tl.x, tl.y + (TILE_SIZE/2)};
        SDL_FPoint points_p[4] = {
            {ptr.x - u->width, ptr.y - u->height},
            {ptr.x + u->width, ptr.y - u->height},
            {ptr.x - u->width, ptr.y},
            {ptr.x + u->width, ptr.y},
        };

        int indexSum = 0;

        if(u->state == EntityState::Moving || u->state == EntityState::ForcedMoving){
            SDL_FPoint normOffset = {fabs(u->gridOffset.x), fabs(u->gridOffset.y)};
            if(u->gridPos.x < u->targetPos.x){if(normOffset.x > (float)(TILE_SIZE/3.0f)){indexSum = 1;}}
            else if(u->gridPos.x > u->targetPos.x){if(normOffset.x > (float)(TILE_SIZE/1.5f)){indexSum = -1;}}
            else if(u->gridPos.y < u->targetPos.y){if(normOffset.y > (float)((TILE_SIZE)/2)/3.0f){indexSum = 1;}}
            else if(u->gridPos.y > u->targetPos.y){if(normOffset.y > (float)((TILE_SIZE)/2)/1.5f){indexSum = -1;}}
        }

        const Animation& anim = u->getCurrentAnimation();
        SDL_FPoint UVS[4] = {
            {0.0f, 0.0f},{1.0f, 0.0f},
            {0.0f, 1.0f},{1.0f, 1.0f}
        };

        for (int i = 0; i < 4; i++){vertices.push_back({points_p[i], UVS[i], anim.sheet->id, anim.frameWidth, anim.frameHeight, anim.frames, (int)u->direction, u->baseAnimSpeed * anim.speed, u->gridPos.x + u->gridPos.y, indexSum});}
        indices.push_back(vertexOffset);
        indices.push_back(vertexOffset + 1);
        indices.push_back(vertexOffset + 2);
        indices.push_back(vertexOffset + 1);
        indices.push_back(vertexOffset + 2);
        indices.push_back(vertexOffset + 3);

        vertexOffset+=4;
    }


    unitIndexSize = (Uint32)indices.size();


    size_t vertSize = vertices.size() * sizeof(Entity_Vertex);
    size_t indexSize = indices.size() * sizeof(int);
    if(vertSize > 0){
        memcpy(buffPtr + 0, vertices.data(), vertSize);
    }
    if(indexSize > 0){
        memcpy((Uint8*)buffPtr + vertSize, indices.data(), indexSize);
    }

    SDL_GPUTransferBufferLocation vertSrc = { tbuf, 0 };
    SDL_GPUBufferRegion vertDst = { unitVBuf,  0, (Uint32)vertSize};
    SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);

    SDL_GPUTransferBufferLocation indexSrc = { tbuf, (Uint32)vertSize };
    SDL_GPUBufferRegion indexDst = { unitIBuf,  0, (Uint32)indexSize};
    SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDst, false);
    
    SDL_UnmapGPUTransferBuffer(renderer, tbuf);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
    dirtyUnits = false;
}
