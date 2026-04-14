#pragma once

#include <unordered_set>
#include <map>
#include "SDL_rect.h"
#include <string>

#include "entities/entity/entity.h"
#include "entities/skill/skill.h"

#include "sprite/sprite.h"
#include "render/render.h"
#include "geometry/geometry.h"

#include "constants.h"

struct UnitData {
    std::map<std::string, Animation*> animations;
    SpriteSheet* expressionSheet;
    float baseWidth, baseHeight;
    int baseHP, baseSpeed, baseShield;
};

class Unit : public Entity{       
    public:
        std::string name;
        int maxHp;
        int currentHp;
        int maxSpeed;
        int currentSpeed;
        int shield;
        int selectedSkill;
        std::vector<Skill*> skills;

        Unit(const std::string& name, SDL_Point gridPos, const UnitData& uData);
        ~Unit();

        void select();

        void desselect();
    
        void hoverSkill(int skillId);
        void offHoverSkill();
        void selectSkill(int skillId);

        void castSkill(int skillId, SDL_Point selectedTile);

        void calculatePreview(SDL_Point target);


    private:
        SpriteSheet* expressionSheet;

        void calculateReachMap(int skillId, int size, int minSize = 0);
        int getSkillDependentValue(SkillDependency dep, int base);

        void setTile(SDL_Point target) override;

        void move() override;
};

extern bool dirtyUnits;

extern Unit* SELECTED_UNIT;
extern Unit* HOVERED_UNIT;

extern std::unordered_set<Unit*> units;
extern Unit* unitMap[BOARD_WIDTH * BOARD_HEIGHT];

void sortUnits(SDL_GPUDevice* renderer);

void applySkillEffects(Unit* caster, Skill* s);