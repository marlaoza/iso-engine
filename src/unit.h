#pragma once
#include "constants.h"
#include <vector>
#include <map>
#include "geometry.h"
#include "SDL_rect.h"
#include "render.h"
#include "skill.h"
#include "sprite.h"
#include <string>

enum class UnitState {
    Idle,
    Moving,
    Casting,
    Selecting
};

struct UnitData {
    std::map<std::string, Animation*> animations;
    SpriteSheet* expressionSheet;
    int baseHeight, baseWidth, baseHP, baseSpeed, baseShield;
};

class Unit {       
    public:
        int id;
        char name[32];
        SDL_FPoint gridOffset;
        SDL_Point gridPos;
        int height;
        int width;
        int maxHp;
        int currentHp;
        int maxSpeed;
        int currentSpeed;
        int shield;
        UnitState state;
        Direction direction;
        SDL_Point targetPos;
        int selectedSkill;
        std::vector<Skill*> skills;

        Unit(char name[32], SDL_Point gridPos, UnitData* uData);
        ~Unit();

        void select();

        void desselect();
    
        void hoverSkill(int skillId);
        void offHoverSkill(int skillId);
        void selectSkill(int skillId);

        void castSkill(int skillId);

        void setPath(std::vector<SDL_Point> path);
        void move();
        void calculatePreview(SDL_Point target);

        Animation* getCurrentAnimation();

    private:
        std::map<std::string, Animation*> animations;
        SpriteSheet* expressionSheet;
        std::vector<SDL_Point> reachMap;
        std::vector<SDL_Point> targetPool;
        int poolIndex;

        void calculateReachMap(int size, int minSize = 0);

        void setTile(SDL_Point target);
        int getSkillDependentValue(SkillDependency dep, int base);
};

extern bool dirtyUnits;

extern Unit* SELECTED_UNIT;
extern Unit* HOVERED_UNIT;

extern std::vector<Unit*> units;
extern Unit* unitMap[BOARD_WIDTH * BOARD_HEIGHT];

void sortUnits(SDL_GPUDevice* renderer);