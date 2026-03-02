#pragma once
#include "constants.h"
#include "SDL_rect.h"
#include "geometry.h"
#include "SDL_gpu.h"
#include "render.h"
#include "colors.h"

struct MapInfo {
    int type;
    int height;
};

extern MapInfo map[BOARD_WIDTH][BOARD_HEIGHT];

extern bool dirtyMap;

extern Geometry<Tile_Vertex> layeredGeometry[BOARD_WIDTH + BOARD_HEIGHT - 1];

void sortTilePoints(SDL_GPUDevice* renderer);

void calculateTilePoints(SDL_GPUDevice* renderer);