#pragma once
#include <vector>
#include "SDL_rect.h"
#include "SDL_gpu.h"
#include "geometry.h"
#include "render.h"
#include "colors.h"
#include "sprite.h"

enum class UIState {
    Default, //0
    Hovered, //1
    Selected, //2
    Disabled, //3
    Hidden //4
};

struct UIText {
    int id;
    char* content;
    SDL_FPoint position;
    SDL_FColor color;
    SDL_GPUTexture* texture = nullptr;
    int width, height;
    char* font;        
};

class UIElement {       
    public:
        bool interactable;
        int param;
        void (*onClick)(int param);
        void (*onHover)(int param);
        void (*offHover)(int param);

        UIElement(SDL_FPoint position, int width, int height, bool interactable, Sprite texture = empty, Sprite hlTexture = empty, int param = -1,  SDL_FColor tint = WHITE);
        ~UIElement();


        void setState(UIState state);
        void setPosition(SDL_FPoint position);
        void setSize(int width , int height );

        void addText(UIText textToAdd);
        void updateText(int id, UIText newText);

        bool inBounds(SDL_FPoint point);

        UIState getState();

        void setSprite(Sprite sprite);
        void setHLSprite(Sprite HlSprite);
        void setTint(SDL_FColor tint);

        SDL_FPoint getPosition();
        SDL_FColor getColor();

        std::vector<UIText*> getText();

        Sprite getSprite();
        Sprite getHLSprite();

        int getWidth();
        int getHeight();
        int getId();


    private:
        std::vector<UIText*> text;
        int id;
        SDL_FPoint position;
        int height, width;
        UIState state;
        Sprite sprite;
        Sprite hlSprite;
        SDL_FColor tint;
        };

extern std::vector<UIElement*> UIElements;
extern bool dirtyUi;

void sortUiElements(SDL_GPUDevice* renderer);

