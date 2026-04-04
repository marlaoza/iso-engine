#pragma once

#include <vector>

#include "geometry/geometry.h"

enum class EffectTarget {
    Caster,
    Unit,
    Tile
};

enum class EffectType {
    Create,
    Damage,
    Heal,
    Move,
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

struct Skill {
    int id;
    SkillOrigin origin;
    char name[32];
    char description[256];
    SkillDependency rangeDep;
    int highlightPallete;
    int minRange; int baseRange;
    int cost;
    bool reusable;
    bool used;
    CostType costType;
    std::vector<SkillEffect> effects;
};


Skill CreateMoveSkill();
Skill CreateSampleSkill();
Skill CreateSampleSkill2();

std::vector<SDL_Point> getSkillEffectArea(SkillEffect e, SDL_Point origin, Direction direction, int range);