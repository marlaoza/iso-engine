#include "ui.h"
#include <vector>
#include "SDL_rect.h"
#include "render.h"
#include "colors.h"
#include "geometry.h"

UIElement::UIElement(SDL_FPoint position, int width, int height, bool interactable, Sprite sprite, Sprite hlSprite, int param, SDL_FColor tint) {
    this->id = 0;
    this->position = position;
    this->height = height;
    this->width = width;
    this->interactable = interactable;
    this->state = UIState::Default;
    this->param = param;
    this->onClick = nullptr;
    this->onHover = nullptr;
    this->offHover = nullptr;
    this->sprite = sprite;
    this->parent = nullptr;
    this->visible = true;
    if(hlSprite.c.pos.x == 0 && hlSprite.c.pos.y == 0 && hlSprite.c.height == 0 && hlSprite.c.width == 0){
        this->hlSprite = sprite;
    }else{
        this->hlSprite = hlSprite;
    }
    this->tint = tint;

    this->setText({});

};

UIElement::~UIElement(){
    if (this->text.texture) {
        SDL_ReleaseGPUTexture(renderer, this->text.texture); 
    }

    dirtyUi = true;
}

void UIElement::addChildren(UIElement* child){
    child->setParent(this);
    this->children.push_back(child);
    dirtyUi = true;

}
std::vector<UIElement*> UIElement::getChildren(){
    return this->children;
}
UIElement* UIElement::getChild(int id){
    return this->children[id];
}

void UIElement::setParent(UIElement* parent){
    this->id = parent->getChildren().size();
    this->parent = parent;
}
UIElement* UIElement::getParent(){
    return this->parent;
}

bool UIElement::inBounds(SDL_FPoint point){
    return point.x >= this->position.x && 
           point.x <= this->position.x + this->width &&
           point.y >= this->position.y && 
           point.y <= this->position.y + this->height;
}

UIElement* UIElement::findSelectedChildren(SDL_FPoint point){

    bool foundChild = false;
    for (int i = (int)this->children.size() - 1; i >= 0; i--) 
    {
        UIElement* c = children[i];
        if(c->isVisible() && c->getState() != UIState::Disabled && c->inBounds(point)){
            UIElement* hovered = c->findSelectedChildren(point);
            if (hovered != nullptr) return hovered;
        }
    }

    if(this->interactable == true){return this;}
    return nullptr;
}

void UIElement::setText(UIText text){
    int maxWidth = text.maxWidth;
    if(maxWidth <= 0) maxWidth = this->width;
    if (this->text.texture != nullptr) {
        SDL_ReleaseGPUTexture(renderer, this->text.texture);
    }
    text.texture = createTextTexture(text.content.data(), text.color, font_main, &text.width, &text.height, maxWidth);
    this->text = text;

    dirtyUi = true;
}

void UIElement::setTextContent(char* content){
    this->text.content = content;
    if (this->text.texture != nullptr) {
        SDL_ReleaseGPUTexture(renderer, this->text.texture);
    }
    int maxWidth = text.maxWidth;
    if(maxWidth <= 0) maxWidth = this->width;
    text.texture = createTextTexture(text.content.data(), text.color, font_main, &text.width, &text.height, maxWidth);

    dirtyUi = true;
}

UIText UIElement::getText(){return this->text;}

void UIElement::setState(UIState state){
    this->state = state;
    dirtyUi = true;
};
void UIElement::setPosition(SDL_FPoint position){
    this->position = position;
    dirtyUi = true;
};
void UIElement::setSize(int width , int height ){
    this->height = height;
    this->width = width;

    int maxWidth = this->text.maxWidth;
    if(maxWidth <= 0) maxWidth = this->width;
    this->text.texture = createTextTexture(this->text.content.data(), this->text.color, font_main, &this->text.width, &this->text.height, maxWidth);
    
    dirtyUi = true;
};
void UIElement::setSprite(Sprite sprite){
    this->sprite = sprite;
    dirtyUi = true;
};
void UIElement::setHLSprite(Sprite HlSprite){
    this->hlSprite = HlSprite;
    dirtyUi = true;
};
void UIElement::setTint(SDL_FColor tint){
    this->tint = tint;
    dirtyUi = true;
};

bool UIElement::isVisible(){return this->visible;}

void UIElement::setVisible(bool value){
    this->visible = value;
    dirtyUi = true;
}

Sprite UIElement::getSprite() {return this->sprite;}
Sprite UIElement::getHLSprite() {return this->hlSprite;}
UIState UIElement::getState(){return this->state;}
SDL_FPoint UIElement::getPosition() {return this->position;}
SDL_FColor UIElement::getColor() {return this->tint;}
int UIElement::getWidth(){return this->width;}
int UIElement::getHeight(){return this->height;}

bool dirtyUi = false;
std::vector<UIElement*> UIElements;


void calculateElementPoints(
    UIElement* u, 
    std::vector<UI_Vertex>& vertices, 
    std::vector<int>& indices, 
    int* vertexOffset,
    std::vector<Text_Vertex>& textVertices,
    std::vector<int>& textIndices,
    int* textVertexOffset,
    int* totalTextElements
){
    if(!u->isVisible()) {return;}
            
    Sprite sprite = u->getSprite(); 
    Sprite HLSprite = u->getHLSprite();

    SDL_FPoint position = u->getPosition();
    int width = u->getWidth();
    int height = u->getHeight();

    SDL_FColor tint =  u->getColor();
    UIState state = u->getState();
    if(sprite.type == SpriteType::Default){

        vertices.push_back({
            position, 
            {0.0f, 0.0f}, 
            tint, 
            sprite.c.pos.x, sprite.c.pos.y, sprite.c.width, sprite.c.height, 
            HLSprite.c.pos.x, HLSprite.c.pos.y, HLSprite.c.width, HLSprite.c.height, 
            (int32_t)state
        });

        vertices.push_back({
            {position.x + width, position.y}, 
            {1.0f, 0.0f}, 
            tint,
            sprite.c.pos.x, sprite.c.pos.y, sprite.c.width, sprite.c.height, 
            HLSprite.c.pos.x, HLSprite.c.pos.y, HLSprite.c.width, HLSprite.c.height, 
            (int32_t)state
        });

        vertices.push_back({
            {position.x, position.y + height}, 
            {0.0f, 1.0f}, 
            tint,
            sprite.c.pos.x, sprite.c.pos.y, sprite.c.width, sprite.c.height, 
            HLSprite.c.pos.x, HLSprite.c.pos.y, HLSprite.c.width, HLSprite.c.height, 
            (int32_t)state
        });

        vertices.push_back({
            {position.x + width, position.y + height}, 
            {1.0f, 1.0f}, 
            tint,
            sprite.c.pos.x, sprite.c.pos.y, sprite.c.width, sprite.c.height, 
            HLSprite.c.pos.x, HLSprite.c.pos.y, HLSprite.c.width, HLSprite.c.height, 
            (int32_t)state
        });

        indices.push_back((*vertexOffset) + 0);
        indices.push_back((*vertexOffset) + 1);
        indices.push_back((*vertexOffset) + 2);
        indices.push_back((*vertexOffset) + 1);
        indices.push_back((*vertexOffset) + 2);
        indices.push_back((*vertexOffset) + 3);
        (*vertexOffset)+=4;

    }else{
            
        int stretchWidth = width - sprite.cl.width - sprite.cr.width;
        if(stretchWidth <= 0) stretchWidth = sprite.c.width;
        int stretchHeight = height - sprite.tr.height - sprite.br.height;
        if(stretchHeight <= 0) stretchHeight = sprite.c.height;
        Texture spriteSlices[3][3] 
        = 
        {
            {sprite.tl, sprite.tc, sprite.tr},
            {sprite.cl, sprite.c, sprite.cr},
            {sprite.bl, sprite.bc, sprite.br},
        };
        Texture HLspriteSlices[3][3] 
        = 
        {
            {HLSprite.tl, HLSprite.tc, HLSprite.tr},
            {HLSprite.cl, HLSprite.c, HLSprite.cr},
            {HLSprite.bl, HLSprite.bc, HLSprite.br},
        };
        int Twidth = 0;
        int Theight = 0;
        SDL_FPoint lastPos = position;
        for (int y = 0; y < 3; y++)
        {
            for(int x = 0; x < 3; x++){
                Texture t = spriteSlices[y][x];
                Texture hl = HLspriteSlices[y][x];
                Twidth = t.width;
                Theight = t.height;
                if(x == 1){Twidth = stretchWidth;}
                if(y == 1){Theight = stretchHeight;}
                vertices.push_back({
                    lastPos, 
                    {0.0f, 0.0f}, 
                    tint, 
                    t.pos.x, t.pos.y, t.width, t.height, 
                    hl.pos.x, hl.pos.y, hl.width, hl.height, 
                    (int32_t)state
                });
                vertices.push_back({
                    {lastPos.x + Twidth, lastPos.y}, 
                    {1.0f, 0.0f}, 
                    tint,
                    t.pos.x, t.pos.y, t.width, t.height, 
                    hl.pos.x, hl.pos.y, hl.width, hl.height, 
                    (int32_t)state
                });
                vertices.push_back({
                    {lastPos.x, lastPos.y + Theight}, 
                    {0.0f, 1.0f}, 
                    tint,
                    t.pos.x, t.pos.y, t.width, t.height, 
                    hl.pos.x, hl.pos.y, hl.width, hl.height, 
                    (int32_t)state
                });
                vertices.push_back({
                    {lastPos.x + Twidth, lastPos.y + Theight}, 
                    {1.0f, 1.0f}, 
                    tint,
                    t.pos.x, t.pos.y, t.width, t.height, 
                    hl.pos.x, hl.pos.y, hl.width, hl.height, 
                    (int32_t)state
                });
                indices.push_back((*vertexOffset) + 0);
                indices.push_back((*vertexOffset) + 1);
                indices.push_back((*vertexOffset) + 2);
                indices.push_back((*vertexOffset) + 1);
                indices.push_back((*vertexOffset) + 2);
                indices.push_back((*vertexOffset) + 3);
                (*vertexOffset)+=4;
                lastPos = {lastPos.x + Twidth, lastPos.y};
            }
            lastPos = {position.x, lastPos.y + Theight};
        }
            
    }

    UIText t = u->getText();
    if(t.content.size() > 0 && t.texture != nullptr){
        SDL_FPoint posOffset = {position.x + t.position.x, position.y + t.position.y};
        textVertices.push_back({
            posOffset,
            {0.0f,0.0f},
            t.color,
            (int32_t)state
        });
        textVertices.push_back({
            {posOffset.x + t.width, posOffset.y},
            {1.0f,0.0f},
            t.color,
            (int32_t)state
        });
        textVertices.push_back({
            {posOffset.x, posOffset.y + t.height},
            {0.0f,1.0f},
            t.color,
            (int32_t)state
        });
        textVertices.push_back({
            {posOffset.x + t.width, posOffset.y + t.height},
            {1.0f,1.0f},
            t.color,
            (int32_t)state
        });
        textIndices.push_back((*textVertexOffset) + 0);
        textIndices.push_back((*textVertexOffset) + 1);
        textIndices.push_back((*textVertexOffset) + 2);
        textIndices.push_back((*textVertexOffset) + 1);
        textIndices.push_back((*textVertexOffset) + 2);
        textIndices.push_back((*textVertexOffset) + 3);
        (*totalTextElements)++;
        (*textVertexOffset)+=4;
        textDrawCalls.push_back(t.texture);
    }

    std::vector<UIElement*> childrens = u->getChildren();
    for (UIElement* c : childrens)
    {
        calculateElementPoints(c, vertices, indices, vertexOffset, textVertices, textIndices, textVertexOffset, totalTextElements);
    }
    

}

void sortUiElements(SDL_GPUDevice* renderer){
    textDrawCalls.clear();
    int vertexOffset = 0;
    std::vector<UI_Vertex> vertices;
    std::vector<int> indices;
    
    UIIndexSize = 0;

    int textVertexOffset = 0;
    std::vector<Text_Vertex> textVertices;
    std::vector<int> textIndices;
    
    TextIndexSize = 0;

    int totalTextElements = 0;

    for (UIElement* u : UIElements){ calculateElementPoints(u, vertices, indices, &vertexOffset, textVertices, textIndices, &textVertexOffset, &totalTextElements); }

    UIIndexSize = (Uint32)indices.size();
    TextIndexSize = (Uint32)textIndices.size();

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

    size_t maxVerts = vertices.size() * sizeof(UI_Vertex);
    size_t maxInds  = indices.size() * sizeof(int);
    Uint32 totalSize = (Uint32)(maxVerts + maxInds);

    if(totalSize == 0){
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);
        dirtyUi = false;
        return;
    }

    SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
    SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

    Uint8* buffPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);
    size_t vertSize = vertices.size() * sizeof(UI_Vertex);
    size_t indexSize = indices.size() * sizeof(int);
    if(vertSize > 0){memcpy(buffPtr + 0, vertices.data(), vertSize);}
    if(indexSize > 0){memcpy((Uint8*)buffPtr + vertSize, indices.data(), indexSize);}
    SDL_GPUTransferBufferLocation vertSrc = { tbuf, 0 };
    SDL_GPUTransferBufferLocation indexSrc = { tbuf, (Uint32)vertSize };
    SDL_GPUBufferRegion vertDst = { UIVBuf,  0, (Uint32)vertSize};
    SDL_GPUBufferRegion indexDst = { UIIBuf,  0, (Uint32)indexSize};

    SDL_UnmapGPUTransferBuffer(renderer, tbuf);
    SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);
    SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDst, false);
    SDL_ReleaseGPUTransferBuffer(renderer, tbuf);

    if(totalTextElements > 0){
        maxVerts = totalTextElements * 4 * sizeof(Text_Vertex);
        maxInds  = totalTextElements * 6  * sizeof(int);
        totalSize = (Uint32)(maxVerts + maxInds);

        tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
        tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

        buffPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

        vertSize = textVertices.size() * sizeof(Text_Vertex);
        indexSize = textIndices.size() * sizeof(int);
        if(vertSize > 0){memcpy(buffPtr + 0, textVertices.data(), vertSize);}
        if(indexSize > 0){memcpy((Uint8*)buffPtr + vertSize, textIndices.data(), indexSize);}
        vertSrc = { tbuf, 0 };
        vertDst = { textVBuf,  0, (Uint32)vertSize};
        indexSrc = { tbuf, (Uint32)vertSize };
        indexDst = { textIBuf,  0, (Uint32)indexSize};

        SDL_UnmapGPUTransferBuffer(renderer, tbuf);
        SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);
        SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDst, false);
        SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
    }

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);
    dirtyUi = false;
}

void registerElement(UIElement* element){
    element->id = UIElements.size();
    UIElements.push_back(element);
    dirtyUi = true;
}