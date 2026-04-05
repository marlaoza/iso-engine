#include <SDL3/SDL.h>
#include <iostream>

#include "render/render.h"
#include "map/map.h"
#include "ui/ui.h"
#include "highlight/highlight.h"
#include "sprite/sprite.h"

#include "entities/unit/unit.h"
#include "entities/projectile/projectile.h"

#include "managers/inputmanager/inputManager.h"
#include "managers/dialogmanager/dialogManager.h"
#include "managers/cameramanager/cameramanager.h"
#include "managers/framemanager/framemanager.h"

#include "constants.h"

//TODO
//spells com projeteis
//sistema de particulas
//projeteis com particulas
//ui manager
//unit manager
//interaction manager ? ou deixar selected tile no input manager
//entity unificação com alguma classe pois muito do processo é igual 
//efeitos de tile
//salas obscurecidas (pode ser com os efeitos de tile (nevoa, etc))

//arquivos pra salvar os prefabs
//unitdata.h    
//animation.h
//projectilesdata.h
//skilldata.h
//ou algo do genero

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
        if(SELECTED_UNIT->state == EntityState::Casting) SELECTED_UNIT->calculatePreview(SELECTED_TILE);
        
        if(SELECTED_UNIT->state != EntityState::Moving && SELECTED_UNIT->state != EntityState::ForcedMoving){
            SELECTED_UNIT->direction = getDirection(SELECTED_UNIT->gridPos, SELECTED_TILE);
            dirtyUnits = true;
        }
    }



}

UIElement* SELECTED_ELEMENT;

UIElement* SelectedUnitUI;

void findSelectedUIElement(){
    UIElement* LAST_ELEMENT = SELECTED_ELEMENT;
    SELECTED_ELEMENT = nullptr;

    bool found = false;

    for(int i = UIElements.size()-1; i >= 0; i--){
        UIElement* e = UIElements[i];

        if(e->isVisible() && e->getState() != UIState::Disabled){
            if(e->inBounds(MOUSE_POS)){
                UIElement* hovered = e->findSelectedChildren(MOUSE_POS);
                if(hovered != nullptr){
                    SELECTED_ELEMENT = hovered;
                    found = true;
                    break;
                }
            }
        }
        
    }

    if(!found) SELECTED_ELEMENT = nullptr;

    if(SELECTED_ELEMENT != LAST_ELEMENT){
        if(LAST_ELEMENT != nullptr){
            LAST_ELEMENT->setState(UIState::Default);
            if(LAST_ELEMENT->offHover) LAST_ELEMENT->offHover(LAST_ELEMENT->param);
        }

        if(SELECTED_ELEMENT != nullptr){
            SELECTED_ELEMENT->setState(UIState::Hovered);
            if(SELECTED_ELEMENT->onHover) SELECTED_ELEMENT->onHover(SELECTED_ELEMENT->param);
        }
        
    }
}

void createUnitUI(){
    SelectedUnitUI = new UIElement({10, 250}, 250, 100, false);
    UIElement* selectedUiHead = new UIElement({10, 250}, 194, 100, false, unitOverlay);
    UIElement* selectedUiTail = new UIElement({200, 287}, 40, 37, false, unitOverlayTail);

    SelectedUnitUI->addChildren(selectedUiHead);
    SelectedUnitUI->addChildren(selectedUiTail);

    UIElement* btnMove = new UIElement({105, 295}, 53, 27, true, unitButtonLeft, unitButtonLeftHL, 0);
    UIElement* btnPass = new UIElement({132, 296}, 29, 39, true, unitButtonRight, unitButtonRightHL, 0);

    SelectedUnitUI->addChildren(btnMove);
    SelectedUnitUI->addChildren(btnPass);

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
            SELECTED_UNIT->offHoverSkill();
        }
    };

    float x = 170.0f;
    for (int i = 0; i < 6; i++)
    {
        UIElement* skillButtonElement = new UIElement({x, 270}, 45, 40, true, skillButton, skillButtonHL, i+1);
        skillButtonElement->onClick = [](int param){
            if(SELECTED_UNIT != nullptr){
                SELECTED_UNIT->selectSkill(param);
            }
        };
        skillButtonElement->onHover = [](int param){
            if(SELECTED_UNIT != nullptr){
                SELECTED_UNIT->hoverSkill(param);
            }
        };
        skillButtonElement->offHover = [](int param){
            if(SELECTED_UNIT != nullptr){
                SELECTED_UNIT->offHoverSkill();
            }
        };
        x+= 47.0f;
        SelectedUnitUI->addChildren(skillButtonElement);
    }

    registerElement(SelectedUnitUI);

    
};

void toggleUnitUI(SDL_GPUDevice* renderer){
    if(SELECTED_UNIT == nullptr){SelectedUnitUI->setVisible(false);}
    else{SelectedUnitUI->setVisible(true);}
}

UnitData* test;


void createPrefabs(){
    SpriteSheet* testIdleSheet = new SpriteSheet{
        0,
        "src/assets/unit_sprites/base_idle.png",
    };

    testIdleSheet->id = loadUnitSheet(testIdleSheet->path);

    Animation* testIdle = new Animation{
        testIdleSheet,
        3,
        18,
        48,
        1
    };
    std::map<std::string, Animation*> testAnimations;
    testAnimations["idle"] = testIdle;

    test = new UnitData{
        testAnimations,
        nullptr,
        9, 48, 
        10, 3, 0
    };

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
    createPrefabs();

    loadSpritePrefabs();
    loadProjectilePrefabs();
    loadSkillPrefabs();

    createUnitUI();

    toggleUnitUI(renderer);
    sortTilePoints(renderer);

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
                            
                            if(SELECTED_UNIT == nullptr || (SELECTED_UNIT && SELECTED_UNIT->state == EntityState::Idle)) {
                                if(SELECTED_UNIT)SELECTED_UNIT->desselect();
                                unitMap[SELECTED_TILE.y * BOARD_WIDTH + SELECTED_TILE.x]->select();
                                toggleUnitUI(renderer);
                            }

                           
                        }
                        
                        if(SELECTED_UNIT && SELECTED_UNIT->selectedSkill != -1){
                            SELECTED_UNIT->castSkill(SELECTED_UNIT->selectedSkill, SELECTED_TILE);
                        }
                        
                    }
                    
                }
                if(windowEvent.button.button == SDL_BUTTON_RIGHT){
                    if(SELECTED_UNIT){
                        if(SELECTED_UNIT->state == EntityState::Casting || SELECTED_UNIT->state == EntityState::Selecting){
                            SELECTED_UNIT->offHoverSkill();
                            SELECTED_UNIT->selectedSkill = -1;
                            SELECTED_UNIT->state = EntityState::Idle;
                        }
                        else {SELECTED_UNIT->desselect();toggleUnitUI(renderer);}
                    }
                    else if(SELECTED_TILE.x >= 0 && SELECTED_TILE.y >= 0 && !unitMap[SELECTED_TILE.y * BOARD_WIDTH + SELECTED_TILE.x]){
                        new Unit("carinha", SELECTED_TILE, *test);
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

        for (Unit* u : units){u->update();}
        for (Projectile* p : projectiles){p->update();}

        if(projectileDeleteList.size() > 0){
            for (Projectile* p : projectileDeleteList){delete p;}
            projectileDeleteList.clear();
        }

        if(dirtyMap)sortTilePoints(renderer);
        if(dirtyHighlights)sortHighlight(renderer);
        if(dirtyUnits)sortUnits(renderer);
        if(dirtyProjectiles)sortProjectiles(renderer);
        if(dirtyUi)sortUiElements(renderer);
        render(renderer, window);
    }

    destroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

