#pragma once
#include "SDL_rect.h"
#include <functional>
#include <string>
#include <map>

#include "sprite/sprite.h"
#include "geometry/geometry.h"


enum class EntityState {
    Idle,
    Moving,
    Casting,
    Selecting,
    ForcedMoving,
};

class Entity{
    public:
        int id;
        SDL_FPoint gridOffset;
        SDL_Point gridPos;
        float quadHeight;
        float quadWidth;

        int gridWidth;
        int gridHeight;

        int baseAnimSpeed;
        
        Direction direction;

        EntityState state;

        SDL_Point targetPos;

        virtual void update();

        void setPath(std::vector<SDL_Point> path);

        virtual Animation& getCurrentAnimation();

        void playClip(std::string clipName);
        Animation* currentClip;
        int getClipStartFrame();

        Entity(SDL_Point gridPos, int gridWidth = 1, int gridHeight = 1);
        virtual ~Entity() = default;

    protected:
        std::map<std::string, Animation*> animations;
        std::vector<SDL_Point> reachMap;
        std::vector<SDL_Point> targetPool;
        SDL_Point lastPos;
        int poolIndex;
        float moveSpeed;

        int clipStartFrame;
        virtual void setTile(SDL_Point target);

        virtual void move();
};
