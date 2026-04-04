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
    Selecting
};

class Entity{
    public:
        int id;
        SDL_FPoint gridOffset;
        SDL_Point gridPos;
        float height;
        float width;

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

        Entity();
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
