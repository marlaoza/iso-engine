#pragma once
#include "SDL_rect.h"
extern SDL_FPoint CAM_POS;
extern float CAM_SPEED;
extern int CAM_ZOOM;
extern bool dirtyCanvas;

void moveCamera();
void zoomCamera(float amount);