#pragma once

#include "SDL_rect.h"
#include "SDL_gpu.h"

#include "geometry/geometry.h"
#include "render/render.h"
#include "colors.h"
#include "constants.h"

struct MapInfo {
    int type;
    int height;
};

extern MapInfo map[BOARD_WIDTH][BOARD_HEIGHT];

extern bool dirtyMap;

void sortTilePoints(SDL_GPUDevice* renderer);
