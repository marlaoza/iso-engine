#pragma once
#include "SDL_rect.h"
extern SDL_FPoint CAM_POS;
extern float CAM_SPEED;
extern float CAM_ZOOM;

void moveCamera();
void zoomCamera(float amount);