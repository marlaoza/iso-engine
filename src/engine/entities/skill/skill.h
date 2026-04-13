#pragma once

#include <vector>
#include <string>

#include "geometry/geometry.h"
#include "entities/projectile/projectile.h"

enum class EffectTarget {
    Caster,
    Unit,
    Tile
};

enum class EffectType {
    Create,
    Damage,
    Heal,
    Knockback,
    Pathfind,
    Stat,
};

enum class SkillDependency {
    None,
    Speed,
    Strength
};

struct EffectPreview {
    std::vector<SDL_Point> affectedTiles; 
    std::vector<std::vector<SDL_Point>> actionLines; 
};

struct SkillEffect {
    EffectTarget target;
    Shape shape;
    EffectType type;
    SkillDependency powerDep;
    SkillDependency radiusDep;
    int basePower;
    int baseRadius;
    EffectPreview preview;
};



enum class CostType {
    MovePoints,
    ActionPoints
};

enum class SkillOrigin {
    Caster,
    Unit,
    Tile
};


//se skill meele, meele preparation
//se skill weapon shoot, aiming
//se skill weapon meele, weapon preparation
//se skill power *, power aim
enum class CasterAnimation {
    None,
    Meele,
    WeaponMeele,
    WeaponShoot,
    Power,
    PowerShoot,
    PowerSummon,
};


struct Skill {
    int id;
    SkillOrigin origin;
    std::string name;
    std::string description;
    SkillDependency rangeDep;
    int minRange; int baseRange;
    int cost;
    bool reusable;
    bool used;
    CostType costType;
    CasterAnimation casterAnimation;
    ProjectileData* projectileData;
    std::vector<SkillEffect> effects;
    int castingType;
};

extern Skill* moveSkill;
extern Skill* grenadeSkill;

void loadSkillPrefabs();

std::vector<SDL_Point> getSkillEffectArea(SkillEffect e, SDL_Point origin, Direction direction, int range);