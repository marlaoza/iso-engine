#pragma once
#include "constants.h"
#include <vector>
#include "geometry.h"
#include "SDL_rect.h"
#include "render.h"
#include "skill.h"

enum class UnitState {
    Idle,
    Moving,
    Casting,
    Selecting
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
        IsoObject body;
        SDL_Point targetPos;
        int selectedSkill;
        std::vector<Skill> skills;

        Unit(char name[32], SDL_Point gridPos, int height, int width, int maxHp, int maxSpeed);

        void select();

        void desselect();
    
        void hoverSkill(int skillId);
        void offHoverSkill(int skillId);
        void selectSkill(int skillId);

        void castSkill(int skillId);

        void setPath(std::vector<SDL_Point> path);
        void move();
        int getId();
        void calculatePreview(SDL_Point target);

    private:
        std::vector<SDL_Point> reachMap;
        std::vector<SDL_Point> targetPool;
        std::vector<EffectPreview> effectPreview;
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

extern Geometry<Tile_Vertex> unitGeometry[BOARD_WIDTH + BOARD_HEIGHT - 1];

void sortUnits(SDL_GPUDevice* renderer);

void calculateUnitPoints(SDL_GPUDevice* renderer);