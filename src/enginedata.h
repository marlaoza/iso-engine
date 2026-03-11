#pragma once
#include "SDL_stdinc.h"
#include "SDL_rect.h"
#include "geometry.h"
#include "constants.h"

extern float DELTA_TIME;
extern Uint64 LAST_TIME;
extern float FRAME_TIME;

extern float SCALE;
extern int WIDTH;
extern int HEIGHT;
extern float CANVAS_WIDTH, CANVAS_HEIGHT;

extern SDL_FPoint INPUT_AXIS;
extern SDL_FPoint MOUSE_POS;

extern SDL_Point SELECTED_TILE;

void calculateDeltaTime();

struct SceneUniforms {
    float windowWidth;
    float windowHeight;
    float mouseX;
    float mouseY;
    float camX;
    float camY;
    float camZoom;
    float frameTime;
    int selectedX;
    int selectedY;
    int mapSize;
    int padding0;
};

struct MapCell {
    SDL_Point gridPos;
    IsoObject tile;
    int height;
};

extern MapCell tiles[BOARD_WIDTH*BOARD_HEIGHT];