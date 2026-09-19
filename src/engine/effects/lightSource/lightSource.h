#pragma once
#include <constants.h>
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <vector>
#include "geometry/geometry.h"
#include <SDL_gpu.h>
#include "render/render.h"

extern SDL_FColor lightMap[BOARD_WIDTH * BOARD_HEIGHT];
extern bool dirtyLights;

struct LightSource { SDL_Point gridPos; int height; int radius; float intensity; SDL_FColor color; };
extern std::vector<LightSource> lights;

void sortLightmap(SDL_GPUDevice* renderer);
void uploadLightmap(SDL_GPUDevice* renderer);

