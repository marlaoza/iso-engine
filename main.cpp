#include <SDL3/SDL.h>
#include "enginedata.h"
#include "constants.h"
#include <iostream>
#include "render.h"
#include "map.h"
#include "camera.h"
#include "unit.h"
#include "ui.h"
#include "highlight.h"


bool watcher(void *userdata, SDL_Event* event){
    if(event->type == SDL_EVENT_WINDOW_RESIZED){
        return true;
    }
   
    return false;
}

void findSelectedTile(){
    SDL_Point LAST_TILE = SELECTED_TILE;
    SELECTED_TILE = {-1, -1};    

    float worldMouseX = ( MOUSE_POS.x / CAM_ZOOM ) + CAM_POS.x;
    float worldMouseY = ( MOUSE_POS.y / CAM_ZOOM) + CAM_POS.y;

    int indices[6] = { 0, 1, 2, 1, 2, 3 };
    int indices_w[12] = { 0, 1, 3,  1, 3, 4, 1, 2, 4,  2, 4, 5, };

    for (int i = BOARD_WIDTH*BOARD_HEIGHT - 1; i >= 0; i -- ){
        const IsoObject& t = tiles[i].tile;
        
        if(pointInPolygon({worldMouseX,worldMouseY},indices, 2, t.surface)
        ||
        pointInPolygon({worldMouseX,worldMouseY}, indices_w, 4, t.wall)
        ){
            SELECTED_TILE = {i % BOARD_WIDTH, i / BOARD_WIDTH};
            if(SELECTED_TILE.x != LAST_TILE.x || SELECTED_TILE.y != LAST_TILE.y){
                dirtyHighlights = true;
            }

            break;
        }
            
    }
    if( SELECTED_TILE.x > 0 
        && SELECTED_TILE.y > 0
        && SELECTED_UNIT != nullptr
        && SELECTED_UNIT->state == UnitState::Casting
        && (SELECTED_TILE.x != LAST_TILE.x || SELECTED_TILE.y != LAST_TILE.y)) {
        SELECTED_UNIT->calculatePreview(SELECTED_TILE);
    }



}

UIElement* SELECTED_ELEMENT;


UIElement* FTSelectedUi = new UIElement({10, 250} , 192, 97, false, -1, 7);
UIElement* MTSelectedUi = new UIElement({202, 288}, 100, 32, false, -1, 5);
UIElement* LTSelectedUi=  new UIElement({302, 288}, 14, 32,  false, -1, 6);
UIElement* btnMove = new UIElement({105, 295}, 53, 27, true, 0, 0, 3);

UIElement* btnPass = new UIElement({132, 296}, 29, 39, true, 0, 1, 4);
UIElement* skillButtons[6];

void findSelectedUIElement(){
    UIElement* LAST_ELEMENT = SELECTED_ELEMENT;
    SELECTED_ELEMENT = nullptr;

    bool found = false;

    for(int i = UIElements.size()-1; i >= 0; i--){
        UIElement* e = UIElements[i];

        if(e->getState() != UIState::Hidden && e->getState() != UIState::Disabled && e->interactable){
            if(e->inBounds(MOUSE_POS)){
                SELECTED_ELEMENT = e;
                found = true;
                break;;
            }
        }
        
    }

    if(!found) SELECTED_ELEMENT = nullptr;

    if(SELECTED_ELEMENT != LAST_ELEMENT){
        if(SELECTED_ELEMENT != nullptr){
            SELECTED_ELEMENT->setState(UIState::Hovered);
            if(SELECTED_ELEMENT->onHover) SELECTED_ELEMENT->onHover(SELECTED_ELEMENT->param);
        }
        if(LAST_ELEMENT != nullptr){
            LAST_ELEMENT->setState(UIState::Default);
            if(LAST_ELEMENT->offHover) LAST_ELEMENT->offHover(LAST_ELEMENT->param);
        }
    }
}

void createUnitUI(){
    btnMove->onClick = [](int param){
        if(SELECTED_UNIT != nullptr){
            SELECTED_UNIT->selectSkill(param);
        }
    };
    btnMove->onHover = [](int param){
        if(SELECTED_UNIT != nullptr){
            SELECTED_UNIT->hoverSkill(param);
        }
    };
    btnMove->offHover = [](int param){
        if(SELECTED_UNIT != nullptr){
            SELECTED_UNIT->offHoverSkill(param);
        }
    };
    float x = 170.0f;
    for (int i = 0; i < 6; i++)
    {
        skillButtons[i] = new UIElement({x, 270}, 45, 40, true, i+1, 2);
        skillButtons[i]->onClick = [](int param){
            if(SELECTED_UNIT != nullptr){
                SELECTED_UNIT->selectSkill(param);
            }
        };
        skillButtons[i]->onHover = [](int param){
            if(SELECTED_UNIT != nullptr){
                SELECTED_UNIT->hoverSkill(param);
            }
        };
        skillButtons[i]->offHover = [](int param){
            if(SELECTED_UNIT != nullptr){
                SELECTED_UNIT->offHoverSkill(param);
            }
        };
        x+= 47.0f;
    }
    
};

void toggleUnitUI(SDL_GPUDevice* renderer){
    if(SELECTED_UNIT == nullptr){
        btnMove->setState(UIState::Hidden);
        btnPass->setState(UIState::Hidden);
        FTSelectedUi->setState(UIState::Hidden);
        MTSelectedUi->setState(UIState::Hidden);
        LTSelectedUi->setState(UIState::Hidden);
        for (int i = 0; i < 6; i++)
        {
            skillButtons[i]->setState(UIState::Hidden);
        } 
    }else{
        btnMove->setState(UIState::Default);
        btnPass->setState(UIState::Default);
        FTSelectedUi->setState(UIState::Default);
        MTSelectedUi->setState(UIState::Default);
        LTSelectedUi->setState(UIState::Default);       
        for (int i = 0; i < SELECTED_UNIT->skills.size()-1; i++)
        {
            skillButtons[i]->setState(UIState::Default);
        } 
        

    }
}


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

int main(int argc, char *argv[]){
    SDL_Window *window = NULL;
    SDL_GPUDevice *renderer = NULL;

    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
    SDL_AddEventWatch(watcher,NULL);

    window = SDL_CreateWindow("test", WIDTH, HEIGHT, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE);
    if(window == NULL){
        std::cout << "Não foi possivel inicializar janela: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SetWindowMinimumSize(window, MIN_WIDTH, MIN_HEIGHT);

    renderer = createRenderer(window);
    if(renderer == NULL){
        return 1;
    }
    
    createUnitUI();
    toggleUnitUI(renderer);

    calculateTilePoints(renderer);

    SDL_Event windowEvent;
    while(true){
        calculateDeltaTime();

        if(SDL_PollEvent(&windowEvent)){
            if(windowEvent.type == SDL_EVENT_QUIT){break;}
            if(windowEvent.type == SDL_EVENT_KEY_DOWN){onKeyDown(windowEvent.key.key);}
            if(windowEvent.type == SDL_EVENT_KEY_UP){onKeyUp(windowEvent.key.key);}
            if(windowEvent.type == SDL_EVENT_MOUSE_MOTION){
                MOUSE_POS.x = windowEvent.motion.x;
                MOUSE_POS.y = windowEvent.motion.y;
            }
            if(windowEvent.type == SDL_EVENT_MOUSE_WHEEL){
                zoomCamera(windowEvent.wheel.y);
            }
            if(windowEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                if(windowEvent.button.button == SDL_BUTTON_LEFT){
                    if(SELECTED_ELEMENT && SELECTED_ELEMENT->onClick)SELECTED_ELEMENT->onClick(SELECTED_ELEMENT->param);

                    if(SELECTED_TILE.x >= 0 && SELECTED_TILE.y >= 0){
                        if(unitMap[SELECTED_TILE.y * BOARD_WIDTH + SELECTED_TILE.x] != nullptr && unitMap[SELECTED_TILE.y * BOARD_WIDTH + SELECTED_TILE.x] != SELECTED_UNIT){
                            
                            if(SELECTED_UNIT == nullptr || (SELECTED_UNIT && SELECTED_UNIT->state == UnitState::Idle)) {
                                if(SELECTED_UNIT)SELECTED_UNIT->desselect();
                                unitMap[SELECTED_TILE.y * BOARD_WIDTH + SELECTED_TILE.x]->select();
                                toggleUnitUI(renderer);
                            }

                           
                        }
                        
                        if(SELECTED_UNIT && SELECTED_UNIT->selectedSkill != -1){
                            SELECTED_UNIT->castSkill(SELECTED_UNIT->selectedSkill);
                        }
                        
                    }
                    
                }
                if(windowEvent.button.button == SDL_BUTTON_RIGHT){
                    if(SELECTED_UNIT){
                        if(SELECTED_UNIT->state == UnitState::Casting || SELECTED_UNIT->state == UnitState::Selecting){
                            SELECTED_UNIT->offHoverSkill(SELECTED_UNIT->selectedSkill);
                            SELECTED_UNIT->selectedSkill = -1;
                            SELECTED_UNIT->state = UnitState::Idle;
                        }
                        else SELECTED_UNIT->desselect();toggleUnitUI(renderer);
                    }
                    else if(!unitMap[SELECTED_TILE.y * BOARD_WIDTH + SELECTED_TILE.x]){
                        new Unit("carinha", SELECTED_TILE, 10, 3, 10, 3);
                    }
                    
                }
            }
        }

        moveCamera();
        findSelectedUIElement();
        if(SELECTED_ELEMENT){
            SELECTED_TILE = {-1, -1};
            dirtyHighlights = true;
        } 
        else {findSelectedTile();}
        for (Unit* u : units)
        {
            if(u->state == UnitState::Moving) u->move();
        }
        if(dirtyMap)calculateTilePoints(renderer);
        if(dirtyHighlights)sortHighlight(renderer);
        if(dirtyUnits)calculateUnitPoints(renderer);
        if(dirtyUi)updateElementBuffer(renderer);
        render(renderer, window);
    }

    destroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

