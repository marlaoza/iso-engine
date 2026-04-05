#include "skill.h"

std::vector<SDL_Point> getSkillEffectArea(SkillEffect e, SDL_Point origin, Direction direction, int range){
    std::vector<SDL_Point> path;
    
    bool walkability = false;

    switch (e.shape)
    {
        case Shape::Tile:
            path.push_back(origin);
            break;
        case Shape::Line:
            path = getLine(origin, direction, range, walkability);
            break;
        case Shape::Diamond:
            path = getDiamond(origin, range, walkability);
            break;
        case Shape::Circle:
            path = getCircle(origin, range, walkability);
            break;
        case Shape::Circumference:
            path = getCircumference(origin, range, walkability);
            break;
        case Shape::Cross:
            path = getCross(origin, range, walkability);
            break;
        case Shape::X:
            path = getX(origin, range, walkability);
            break;
        
        default:
            break;
    }

    return path;
}

Skill* moveSkill;
Skill * grenadeSkill;

void loadSkillPrefabs(){
    moveSkill = new Skill
    {
        0, 
        SkillOrigin::Tile,
        "Move", 
        "Se Mova", 
        SkillDependency::Speed, 
        2, 
        0,1, 
        0, 
        false,
        false,
        CostType::MovePoints,
        CasterAnimation::None,
        nullptr,
        {{
            EffectTarget::Caster,
            Shape::Tile,
            EffectType::Pathfind,
            SkillDependency::Speed,
            SkillDependency::Speed,
            1,
            1,
            {}
        }} 
    };

    grenadeSkill = new Skill
    {
        0, 
        SkillOrigin::Tile,
        "Granada", 
        "Explode em área", 
        SkillDependency::None, 
        2, 
        3,8, 
        0, 
        false,
        false,
        CostType::ActionPoints,
        CasterAnimation::None,
        testProjectile,
        {{
            EffectTarget::Unit,
            Shape::Diamond,
            EffectType::Move,
            SkillDependency::None,
            SkillDependency::None,
            5,
            3,
            {}
        }} 
    };
}

