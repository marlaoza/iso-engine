#pragma once
#include "render.h"
#include "constants.h"
#include "geometry.h"
#include "enginedata.h"

extern Geometry<Highlight_Vertex> highlightGeometry[BOARD_WIDTH + BOARD_HEIGHT - 1];
extern int highlightMap[BOARD_WIDTH * BOARD_HEIGHT];
extern bool dirtyHighlights;

void sortHighlight(SDL_GPUDevice* renderer);
void addHighlight(SDL_Point p, int type, int pallete);
void clearHighlight(SDL_Point p);
void addHighlightGeometry(SDL_Point p, int vertexOffset[], int type);
