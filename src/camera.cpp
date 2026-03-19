#include "camera.h"
#include "constants.h"
#include "enginedata.h"
#include "managers/inputManager.h"

SDL_FPoint CAM_POS = {0, 0};
float CAM_SPEED = CAM_MIN_SPEED;
float CAM_ZOOM = 1.0f;

void moveCamera(){

    if(INPUT_AXIS.x != 0 || INPUT_AXIS.y != 0){
        if(CAM_SPEED < CAM_MAX_SPEED) CAM_SPEED += CAM_SPEED_INCREMENT * DELTA_TIME;

        CAM_POS.x += INPUT_AXIS.x * CAM_SPEED * DELTA_TIME;
        CAM_POS.y += INPUT_AXIS.y * CAM_SPEED * DELTA_TIME;

    }else{
        CAM_SPEED =  CAM_MIN_SPEED;
    }
    
}

void zoomCamera(float amount){
    CAM_ZOOM += (amount * 0.05f);
    if(CAM_ZOOM > CAM_MAX_ZOOM) CAM_ZOOM = CAM_MAX_ZOOM;
    if(CAM_ZOOM < CAM_MIN_ZOOM) CAM_ZOOM = CAM_MIN_ZOOM;
}