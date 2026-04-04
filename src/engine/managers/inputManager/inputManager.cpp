#include "inputManager.h"

SDL_FPoint INPUT_AXIS = {0, 0};
SDL_FPoint MOUSE_POS = {0,0};

void onKeyDown(SDL_Keycode key){
    switch(key){
        case SDLK_W:case SDLK_UP:
            INPUT_AXIS.y = -1; break;
        case SDLK_S:case SDLK_DOWN:
            INPUT_AXIS.y =  1; break;
        case SDLK_D:case SDLK_RIGHT:
            INPUT_AXIS.x =  1; break;
        case SDLK_A:case SDLK_LEFT:
            INPUT_AXIS.x = -1; break;
        default: break; 
    }
}
void onKeyUp(SDL_Keycode key){
    switch(key){
        case SDLK_W:case SDLK_UP:
            if(INPUT_AXIS.y == -1) INPUT_AXIS.y = 0; break;
        case SDLK_S:case SDLK_DOWN:
            if(INPUT_AXIS.y == 1)  INPUT_AXIS.y = 0; break;
        case SDLK_D:case SDLK_RIGHT:
            if(INPUT_AXIS.x == 1)  INPUT_AXIS.x = 0; break;
        case SDLK_A:case SDLK_LEFT:
            if(INPUT_AXIS.x == -1) INPUT_AXIS.x = 0; break;
        default: break; 
    }
}

void onMouseMotion(SDL_MouseMotionEvent motion){
    MOUSE_POS.x = motion.x;
    MOUSE_POS.y = motion.y;
}
