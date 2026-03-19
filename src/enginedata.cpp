#include "enginedata.h"
#include "SDL_timer.h"
#include "constants.h"
#include "camera.h"

float DELTA_TIME = 0;
Uint64 LAST_TIME = 0;
float FRAME_TIME = 0;

float SCALE = 1;
int WIDTH = MIN_WIDTH, HEIGHT = MIN_HEIGHT;
float CANVAS_WIDTH = MIN_WIDTH, CANVAS_HEIGHT = MIN_HEIGHT;

SDL_Point SELECTED_TILE = {-1, -1};

void calculateDeltaTime() {
    Uint64 currentTime = SDL_GetTicksNS();
    DELTA_TIME = (float)(currentTime - LAST_TIME) / 1000000000.0f;
    LAST_TIME = currentTime;
    if(DELTA_TIME > 0.1f) DELTA_TIME = 0.1f;

    FRAME_TIME+=DELTA_TIME*2;
    if(FRAME_TIME >= 50.0){
        FRAME_TIME = 0.0;
    }
   
}

MapCell tiles[BOARD_WIDTH*BOARD_HEIGHT] = {};


