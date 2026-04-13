#pragma once
#include "SDL_rect.h"
#include "SDL_pixels.h"
#include <vector>
#include <constants.h>

struct SDL_PointHash{
    size_t operator()(const SDL_Point& a) const {
        return (size_t)(a.y * BOARD_WIDTH + a.x);
    }
};

struct SDL_PointEquality{
    bool operator()(const SDL_Point& a, const SDL_Point& b) const {
        return a.x == b.x && a.y == b.y;
    };
};

enum class Direction {
    Down,
    DownLeft,
    Left,
    UpLeft,
    Up,
    UpRight,
    Right,
    DownRight,

};

enum class Shape {
    Tile,
    Line,
    Diamond,
    Circle,
    Circumference,
    Cross,
    X,
};

struct IsoObject {
    SDL_FPoint surface[4];
    SDL_FPoint wall[6];
};

std::vector<SDL_Point> getPath(SDL_Point origin, SDL_Point target, int maxSize, int minSize = 0, bool checkWalkability = true);
std::vector<SDL_Point> getStraightPath(SDL_Point origin, SDL_Point target, int maxSize, int minSize = 0, bool checkWalkability = true);
std::vector<SDL_Point> getDiamond(SDL_Point origin, int maxSize, int minSize = 0, bool checkWalkability = false);
std::vector<SDL_Point> getLine(SDL_Point origin, Direction direction, int maxSize, int minSize = 0, bool checkWalkability = false);
std::vector<SDL_Point> getCross(SDL_Point origin, int maxSize, int minSize = 0, bool checkWalkability = false);
std::vector<SDL_Point> getX(SDL_Point origin, int maxSize, int minSize = 0, bool checkWalkability = false);
std::vector<SDL_Point> getDiagonalLine(SDL_Point origin, Direction direction, int maxSize, int minSize = 0, bool checkWalkability = false);
std::vector<SDL_Point> getCircle(SDL_Point origin, int maxSize, int minSize = 0, bool checkWalkability = false);
std::vector<SDL_Point> getCircumference(SDL_Point origin, int maxSize, int minSize = 0, bool checkWalkability = false);
bool pointInPolygon(SDL_FPoint point, int indices[], int n_triangules, const SDL_FPoint vertices[]);

Direction getDirection(SDL_Point a, SDL_Point b);
SDL_Point getDirectionVector(Direction d);