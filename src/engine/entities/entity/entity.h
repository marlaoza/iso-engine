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
        int height;
        int width;
        Direction direction;

        EntityState state;

        SDL_Point targetPos;

        virtual void update();

        void setPath(std::vector<SDL_Point> path);
        virtual void move();

        virtual Animation& getCurrentAnimation();

        void playClip(std::string clipName);
        Animation* currentClip;
        int getClipStartFrame();

        virtual ~Entity() = default;

    protected:
        std::map<std::string, Animation*> animations;
        std::vector<SDL_Point> reachMap;
        std::vector<SDL_Point> targetPool;
        SDL_Point lastPos;
        int poolIndex;

        int clipStartFrame;
        virtual void setTile(SDL_Point target);
};
