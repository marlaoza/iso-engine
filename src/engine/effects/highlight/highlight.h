#pragma once

#include "managers/frameManager/frameManager.h"

#include "render/render.h"
#include "geometry/geometry.h"

#include "constants.h"
#include <unordered_set>


#define HL_SELECTED_TILE 1

#define HL_REACH_MAP 3
#define HL_EFFECT_AREA 4
#define HL_ACTION_LINE 5

extern std::unordered_set<SDL_Point, SDL_PointHash, SDL_PointEquality> highlightRegionMap[MAX_HL_LAYERS];

extern bool dirtyHighlights;

void loadHighlightQuad(SDL_GPUDevice* renderer);
void sortHighlights(SDL_GPUDevice* renderer);

void addHighlightPoint(SDL_Point p, int type, bool updateBuffer = true);
void clearHighlightPoint(SDL_Point p, int type,bool updateBuffer = true);

void addHighlightRegion(std::vector<SDL_Point> p, int type);
void clearHighlightRegion(int type);

