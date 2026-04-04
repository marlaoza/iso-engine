#include "sprite.h"

//UI SPRITES
Sprite container = {
    SpriteType::Sliced,
    {{6, 6},3 ,3 },

    {{0 ,0 },5 ,5 }, {{6 ,0 },3 ,5 }, {{10,0 },9 ,5 },
    {{0 ,6 },5 ,3 },                  {{10,6 },9 ,3 },
    {{0 ,10},5 ,8 }, {{6 ,10},3 ,8 }, {{10,10},9 ,8 }
};

Sprite empty = {
    SpriteType::Default,
    {{0,0},0,0}
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


Sprite dialogBackground = {
    SpriteType::Sliced,
    {{20,26},4 ,3 },

    {{0 ,19},19,6 }, {{20,19},4 ,6 }, {{25,19},10,6 },
    {{0 ,26},19,3 },                  {{25,26},10,3 },
    {{0 ,30},19,23}, {{20,30},4 ,23}, {{25,30},10,23}
};

Sprite dialogOptionButton = {
    SpriteType::Sliced,
    {{24, 7},3 ,3 },

    {{20 ,0 },3 ,6 }, {{24,0 },3 ,6 }, {{28,0 },9 ,6 },
    {{20 ,7 },3 ,3 },                  {{28,7 },9 ,3 },
    {{20 ,11},3 ,5 }, {{24,11},3 ,5 }, {{28,11},9 ,5 }
};

Sprite dialogOptionButtonHL = {
    SpriteType::Sliced,
    {{42, 7},3 ,3 },

    {{38 ,0 },3 ,6 }, {{42,0 },3 ,6 }, {{46,0 },9 ,6 },
    {{38 ,7 },3 ,3 },                  {{46,7 },9 ,3 },
    {{38 ,11},3 ,5 }, {{42,11},3 ,5 }, {{46,11},9 ,5 }
};

Sprite dialogAdvanceButton = {
    SpriteType::Default,
    {{0, 54},23,23},
};

Sprite dialogAdvanceButtonHL = {
    SpriteType::Default,
    {{24, 54},23,23},

};

