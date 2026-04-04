#pragma once
#include <vector>
#include <string>
#include "SDL_rect.h"
#include "SDL_gpu.h"

#include "geometry/geometry.h"
#include "render/render.h"
#include "sprite/sprite.h"

#include "colors.h"


enum class UIState {
    Default, //0
    Hovered, //1
    Selected, //2
    Disabled, //3
};

struct UIText {
    int id;
    std::string content;
    SDL_FPoint position;
    SDL_FColor color;
    int maxWidth;
    SDL_GPUTexture* texture = nullptr;
    int width, height;
    char* font;        
};

enum class UIStylePositionType {Absolute,Relative,Anchored};
//abs use abs position, 
//relative use flex + position as offset,
//anchored fixed, calc anchorPoint

enum class AnchorPoint {
    TopLeft,TopCenter,TopRight,
    CenterLeft,Center,CenterRight,
    BottomLeft, BottomCenter, BottomRight
};
enum class FlexDirection {Row,Column};

struct UIStyle {
    UIStylePositionType position;
    AnchorPoint anchorPoint;
    FlexDirection flexDirection;
    float margin[4]; //t,r,b,l
    float padding[4];
    float gap;
};

class UIElement {       
    public:
        int id;
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

        void setText(UIText text);
        void setTextContent(char* content);

        bool inBounds(SDL_FPoint point);

        UIState getState();

        void setSprite(Sprite sprite);
        void setHLSprite(Sprite HlSprite);
        void setTint(SDL_FColor tint);

        SDL_FPoint getPosition();
        SDL_FColor getColor();

        UIText getText();

        Sprite getSprite();
        Sprite getHLSprite();

        int getWidth();
        int getHeight();

        bool isVisible();
        void setVisible(bool value);

        UIElement* findSelectedChildren(SDL_FPoint point);

        void setParent(UIElement* parent);
        UIElement* getParent();
        void addChildren(UIElement* child);
        std::vector<UIElement*> getChildren();
        UIElement* getChild(int id);


    private:
        UIElement* parent;
        std::vector<UIElement*> children;
        UIText text;
        UIStyle style;
        SDL_FPoint position;
        int height, width;
        UIState state;
        Sprite sprite;
        Sprite hlSprite;
        SDL_FColor tint;
        bool visible;
};

extern std::vector<UIElement*> UIElements;
extern bool dirtyUi;

void sortUiElements(SDL_GPUDevice* renderer);

void registerElement(UIElement* element);