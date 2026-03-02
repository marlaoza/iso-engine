#include "skill.h"
#include "geometry.h"

Skill CreateMoveSkill() {
    return {
        0, 
        SkillOrigin::Self,
        "Move", 
        "Se Mova", 
        SkillDependency::Speed, 
        2, 
        0,1, 
        0, 
        CostType::MovePoints,
        {{
            EffectTarget::Self,
            Shape::Tile,
            EffectType::Pathfind,
            SkillDependency::Speed,
            SkillDependency::Speed,
            1,
            1
        }} 
    };
}


Skill CreateSampleSkill() {
    return {
        1, 
        SkillOrigin::Tile,
        "Test", 
        "test", 
        SkillDependency::None, 
        2, 
        0,5, 
        0, 
        CostType::ActionPoints,
        {{
            EffectTarget::Unit,
            Shape::Tile,
            EffectType::Pathfind,
            SkillDependency::None,
            SkillDependency::None,
            4,
            4
        }} 
    };
}

Skill CreateSampleSkill2() {
    return {
        2, 
        SkillOrigin::Tile,
        "push", 
        "test", 
        SkillDependency::None, 
        2, 
        0,5, 
        0, 
        CostType::ActionPoints,
        {{
            EffectTarget::Unit,
            Shape::Tile,
            EffectType::Move,
            SkillDependency::None,
            SkillDependency::None,
            4,
            4
        }} 
    };
}



std::vector<SDL_Point> getSkillEffectArea(SkillEffect e, SDL_Point origin, Direction direction, int range){
    std::vector<SDL_Point> path;
    
    bool walkability = false;
    if(e.type == EffectType::Move){
        walkability = true;
    }

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


