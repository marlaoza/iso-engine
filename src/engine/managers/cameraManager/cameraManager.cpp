#include "cameraManager.h"

#include "managers/frameManager/frameManager.h"
#include "managers/inputManager/inputManager.h"

#include "constants.h"
#include <cmath>

SDL_FPoint CAM_POS = {0, 0};
float CAM_SPEED = CAM_MIN_SPEED;

int CAM_ZOOM = 0;

bool dirtyCanvas = false;

void moveCamera(){

    if(INPUT_AXIS.x != 0 || INPUT_AXIS.y != 0){
        if(CAM_SPEED < CAM_MAX_SPEED) CAM_SPEED += CAM_SPEED_INCREMENT * DELTA_TIME;

        CAM_POS.x += INPUT_AXIS.x * CAM_SPEED * DELTA_TIME;
        CAM_POS.y += INPUT_AXIS.y * CAM_SPEED * DELTA_TIME;

    }else{
        CAM_SPEED =  CAM_MIN_SPEED;
    }
    
    CAM_POS = { floorf(CAM_POS.x), floorf(CAM_POS.y) };
    
}

void zoomCamera(float amount){
    if(amount > 0)CAM_ZOOM += 1;
    else CAM_ZOOM -= 1;

    if(CAM_ZOOM > CAM_MAX_ZOOM) CAM_ZOOM = CAM_MAX_ZOOM;
    if(CAM_ZOOM < 0) CAM_ZOOM = 0;
    dirtyCanvas = true;
}