#pragma once

#include <SDL_gpu.h>
#include <cmath>
#include <algorithm>
#include <unordered_set>

#include "managers/frameManager/frameManager.h"

#include "entities/entity/entity.h"

#include "render/render.h"
#include "map/map.h"

extern bool dirtyProjectiles;

enum class ProjectileTrajectory {
    Arc,
    Line,
    Path
};

struct ProjectileData {
    float width, height; 
    float speed;
    ProjectileTrajectory trajectory;
    Animation* animation;
};

class Projectile : public Entity {
    public:
        Projectile(const ProjectileData& pData, SDL_Point origin, SDL_FPoint originOffset, SDL_Point end, SDL_FPoint endOffset);
        ~Projectile();
        
        ProjectileTrajectory trajectory;

        float z;

        Animation& getCurrentAnimation() override;
    private:
        void move() override;
        void setTile(SDL_Point target) override;
        SDL_FPoint startPos;
        SDL_FPoint startOffset;
        SDL_FPoint endPos;
        float trajectoryDistance;
};

extern std::unordered_set<Projectile*> projectiles;
extern std::vector<Projectile*> projectileDeleteList;
void sortProjectiles(SDL_GPUDevice* renderer);

extern ProjectileData* testProjectile;
void loadProjectilePrefabs();
