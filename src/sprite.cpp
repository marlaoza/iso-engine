#include "sprite.h"

//UI SPRITES
Sprite container = {};

Sprite empty = {
    SpriteType::Default,
    {{0,0},1,1}
};

Sprite unitOverlay = {
    SpriteType::Default,
    {{64,0}, 194, 100}
};

Sprite unitOverlayTail = {
    SpriteType::Sliced,
    {{265, 48}, 5, 5},

    {{259, 37}, 5 , 10}, {{265, 37}, 5 , 10}, {{271, 37}, 8 , 10},
    {{259, 48}, 5 , 5 },                      {{271, 48}, 8 , 5 },
    {{259, 54}, 5 , 5 }, {{265, 54}, 5 , 5 }, {{271, 54}, 8 , 5 }

};

Sprite unitButtonLeft = {
    SpriteType::Default,
    {{0,101}, 53, 27}
};
Sprite unitButtonLeftHL = {
    SpriteType::Default,
    {{0,141}, 53, 27}
};

Sprite unitButtonRight = {
    SpriteType::Default,
    {{53,102}, 28, 39}
};
Sprite unitButtonRightHL = {
    SpriteType::Default,
    {{53,142}, 28, 39}
};

Sprite skillButton = {
    SpriteType::Default,
    {{82,101}, 45, 40}
};
Sprite skillButtonHL = {
    SpriteType::Default,
    {{82,142}, 45, 40}
};
