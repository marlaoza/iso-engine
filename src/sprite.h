#pragma once
#include "SDL_rect.h"


struct SpriteSheet {
    int id;
    const char* path;
};
struct Animation {
    SpriteSheet* sheet;
    int frames;
    int frameWidth, frameHeight;
};


struct Texture {
    SDL_Point pos;
    int width, height;
};

enum class SpriteType{
    Default,
    Sliced
};

struct Sprite {
    SpriteType type;

    Texture c;

    Texture 
    tl, tc, tr, 
    cl,     cr, 
    bl, bc, br;
};


extern Sprite empty;

extern Sprite container;

//UI Sprites
extern Sprite unitOverlay;
extern Sprite unitOverlayTail;

extern Sprite unitButtonLeft;
extern Sprite unitButtonLeftHL;

extern Sprite unitButtonRight;
extern Sprite unitButtonRightHL;

extern Sprite skillButton;
extern Sprite skillButtonHL;

extern Sprite dialogBackground;

extern Sprite dialogOptionButton;

extern Sprite dialogOptionButtonHL;

extern Sprite dialogAdvanceButton;

extern Sprite dialogAdvanceButtonHL;