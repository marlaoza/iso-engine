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
#include "sprite.h"
#include "managers/inputManager.h"
#include "managers/dialogManager.h"


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
    if( SELECTED_TILE.x >= 0 
        && SELECTED_TILE.y >= 0
        && SELECTED_UNIT != nullptr
        && (SELECTED_TILE.x != LAST_TILE.x || SELECTED_TILE.y != LAST_TILE.y)) {
        if(SELECTED_UNIT->state == UnitState::Casting) SELECTED_UNIT->calculatePreview(SELECTED_TILE);
        
        if(SELECTED_UNIT->state != UnitState::Moving){
            SELECTED_UNIT->direction = getDirection(SELECTED_UNIT->gridPos, SELECTED_TILE);
            dirtyUnits = true;
        }
    }



}

UIElement* SELECTED_ELEMENT;

UIElement* SelectedUi;
UIElement* SelectedUiTail;
UIElement* btnMove;
UIElement* btnPass;
UIElement* skillButtons[6];

void findSelectedUIElement(){
    UIElement* LAST_ELEMENT = SELECTED_ELEMENT;
    SELECTED_ELEMENT = nullptr;

    bool found = false;

    for(int i = UIElements.size()-1; i >= 0; i--){
        UIElement* e = UIElements[i];

        if(e->isVisible() && e->getState() != UIState::Disabled && e->interactable){
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
    SelectedUi = new UIElement({10, 250}, 194, 100, false, unitOverlay);
    SelectedUiTail = new UIElement({200, 287}, 40, 37, false, unitOverlayTail);

    btnMove = new UIElement({105, 295}, 53, 27, true, unitButtonLeft, unitButtonLeftHL, 0);
    btnPass = new UIElement({132, 296}, 29, 39, true, unitButtonRight, unitButtonRightHL, 0);
    // UIText t = {
    //     0,
    //     "mover",
    //     {0, 0},
    //     WHITE,
    // };
    // btnMove->addText(t);

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
        skillButtons[i] = new UIElement({x, 270}, 45, 40, true, skillButton, skillButtonHL, i+1);
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
        btnMove->setVisible(false);
        btnPass->setVisible(false);
        SelectedUi->setVisible(false);
        SelectedUiTail->setVisible(false);
        for (int i = 0; i < 6; i++)
        {
            skillButtons[i]->setVisible(false);
        } 
    }else{
        btnMove->setVisible(true);
        btnPass->setVisible(true);
        SelectedUi->setVisible(true);
        SelectedUiTail->setVisible(true);
        for (int i = 0; i < SELECTED_UNIT->skills.size()-1; i++)
        {
            skillButtons[i]->setVisible(true);
        } 
    }
}

int main(int argc, char *argv[]){
    SDL_Window *window = NULL;
    SDL_GPUDevice *renderer = NULL;

    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
    SDL_AddEventWatch(watcher,NULL);

    window = SDL_CreateWindow("test", WIDTH, HEIGHT, SDL_WINDOW_HIGH_PIXEL_DENSITY );
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
    sortTilePoints(renderer);

    dialogManager = new DialogManager();
    
    std::vector<DialogLine> lines;
    lines.push_back({"f","ola, dialogo 1"});
    lines.push_back({"f","segunda frase"});
    lines.push_back({"f","ultima frase"});

    std::vector<DialogOption> options;
    options.push_back({"ir ao 2","dialogo 2",1});
    

    DialogAct act = {
        lines,
        options
    };

    std::vector<DialogLine> lines2;
    lines2.push_back({"f","ola, dialogo 2"});
    lines2.push_back({"f","segunda frase"});
    lines2.push_back({"f","ultima frase"});

    std::vector<DialogOption> options2;
    options2.push_back({"ir ao 1","dialogo 1",0});
    options2.push_back({"sair","tchau",-1});
    

    DialogAct act2 = {
        lines2,
        options2
    };

    std::vector<DialogAct> play;
    play.push_back(act);
    play.push_back(act2);

    dialogManager->set(play);

    dialogManager->show();

    SDL_Event windowEvent;
    while(true){
        calculateDeltaTime();

        if(SDL_PollEvent(&windowEvent)){
            if(windowEvent.type == SDL_EVENT_QUIT){break;}
            if(windowEvent.type == SDL_EVENT_KEY_DOWN){onKeyDown(windowEvent.key.key);}
            if(windowEvent.type == SDL_EVENT_KEY_UP){onKeyUp(windowEvent.key.key);}
            if(windowEvent.type == SDL_EVENT_MOUSE_MOTION){onMouseMotion(windowEvent.motion);}
            if(windowEvent.type == SDL_EVENT_MOUSE_WHEEL){zoomCamera(windowEvent.wheel.y);}
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
                        else {SELECTED_UNIT->desselect();toggleUnitUI(renderer);}
                    }
                    else if(SELECTED_TILE.x >= 0 && SELECTED_TILE.y >= 0 && !unitMap[SELECTED_TILE.y * BOARD_WIDTH + SELECTED_TILE.x]){
                        new Unit("carinha", SELECTED_TILE, 28, 5, 10, 3);
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
        for (Unit* u : units){if(u->state == UnitState::Moving) u->move();}
        if(dirtyMap)sortTilePoints(renderer);
        if(dirtyHighlights)sortHighlight(renderer);
        if(dirtyUnits)sortUnits(renderer);
        if(dirtyUi)sortUiElements(renderer);
        render(renderer, window);
    }

    destroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

