#include "ui.h"
#include <vector>
#include "SDL_rect.h"
#include "render.h"
#include "colors.h"
#include "geometry.h"

UIElement::UIElement(SDL_FPoint position, int width, int height, bool interactable, int param, int texture, int hlTexture, SDL_FColor tint) {
    this->id = UIElements.size();
    this->position = position;
    this->height = height;
    this->width = width;
    this->interactable = interactable;
    this->state = UIState::Default;
    this->param = param;
    this->onClick = nullptr;
    this->onHover = nullptr;
    this->offHover = nullptr;
    this->texture = texture;
    this->hlTexture = hlTexture;
    this->tint = tint;
    UIElements.push_back(this);

    int* textureInfo = UITextureMap[texture];
    int* HLTextureInfo = hlTexture == -1 ? UITextureMap[texture] : UITextureMap[hlTexture];

    UIGeometryMap.vertices.push_back({
        position, 
        {0.0f, 0.0f}, 
        tint, 
        textureInfo[0], textureInfo[1], textureInfo[2], textureInfo[3], 
        HLTextureInfo[0], HLTextureInfo[1], HLTextureInfo[2], HLTextureInfo[3], 
        (int32_t)this->state
    });

    UIGeometryMap.vertices.push_back({
        {position.x + width, position.y}, 
        {1.0f, 0.0f}, 
        tint,
        textureInfo[0], textureInfo[1], textureInfo[2], textureInfo[3], 
        HLTextureInfo[0], HLTextureInfo[1], HLTextureInfo[2], HLTextureInfo[3], 
        (int32_t)this->state
    });

    UIGeometryMap.vertices.push_back({
        {position.x, position.y + height}, 
        {0.0f, 1.0f}, 
        tint,
        textureInfo[0], textureInfo[1], textureInfo[2], textureInfo[3], 
        HLTextureInfo[0], HLTextureInfo[1], HLTextureInfo[2], HLTextureInfo[3], 
        (int32_t)this->state
    });
    UIGeometryMap.vertices.push_back({
        {position.x + width, position.y + height}, 
        {1.0f, 1.0f}, 
        tint,
        textureInfo[0], textureInfo[1], textureInfo[2], textureInfo[3], 
        HLTextureInfo[0], HLTextureInfo[1], HLTextureInfo[2], HLTextureInfo[3], 
        (int32_t)this->state
    });

    UIGeometryMap.indices.push_back(UIIndexSize*4 + 0);
    UIGeometryMap.indices.push_back(UIIndexSize*4 + 1);
    UIGeometryMap.indices.push_back(UIIndexSize*4 + 2);
    UIGeometryMap.indices.push_back(UIIndexSize*4 + 1);
    UIGeometryMap.indices.push_back(UIIndexSize*4 + 2);
    UIGeometryMap.indices.push_back(UIIndexSize*4 + 3);

    UIIndexSize = UIElements.size();
    dirtyUi = true;

};

void UIElement::setState(UIState state){
    this->state = state;
    this->updateGeometry();
};
UIState UIElement::getState(){
    return this->state;
}

bool UIElement::inBounds(SDL_FPoint point){
    return point.x >= this->position.x && 
           point.x <= this->position.x + this->width &&
           point.y >= this->position.y && 
           point.y <= this->position.y + this->height;
}

void UIElement::setPosition(SDL_FPoint position){
    this->position = position;
    this->updateGeometry();
};
void UIElement::setSize(int width , int height ){
    this->height = height;
    this->width = width;
    this->updateGeometry();
};

void UIElement::setTexture(int texture){
    this->texture = texture;
    this->updateGeometry();
};
void UIElement::setHLTexture(int hlTexture){
    this->hlTexture = hlTexture;
    this->updateGeometry();
};
void UIElement::setTint(SDL_FColor tint){
    this->tint = tint;
    this->updateGeometry();
};

int UIElement::getWidth(){
    return this->width;
};
int UIElement::getHeight(){
    return this->height;
};
int UIElement::getId(){
    return this->id;
};

void UIElement::updateGeometry(){
    int base = this->id * 4;

    int* textureInfo = UITextureMap[this->texture];
    int* HLTextureInfo = this->hlTexture == -1 ? UITextureMap[this->texture] : UITextureMap[this->hlTexture];

    UIGeometryMap.vertices[base + 0] = {
        this->position,
        {0.0f, 0.0f},
        this->tint,  
        textureInfo[0], textureInfo[1], textureInfo[2], textureInfo[3], 
        HLTextureInfo[0], HLTextureInfo[1], HLTextureInfo[2], HLTextureInfo[3], 
        (int32_t)this->state
    };

    UIGeometryMap.vertices[base + 1] = {
        {this->position.x + this->width, this->position.y},
        {1.0f, 0.0f},
        this->tint,  
        textureInfo[0], textureInfo[1], textureInfo[2], textureInfo[3], 
        HLTextureInfo[0], HLTextureInfo[1], HLTextureInfo[2], HLTextureInfo[3], 
        (int32_t)this->state
    };

    UIGeometryMap.vertices[base + 2] = {
        {this->position.x, this->position.y + this->height},
        {0.0f, 1.0f},
        this->tint,  
        textureInfo[0], textureInfo[1], textureInfo[2], textureInfo[3], 
        HLTextureInfo[0], HLTextureInfo[1], HLTextureInfo[2], HLTextureInfo[3], 
        (int32_t)this->state
    };

    UIGeometryMap.vertices[base + 3] = {
        {this->position.x + this->width, this->position.y + this->height},
        {1.0f, 1.0f},
        this->tint,  
        textureInfo[0], textureInfo[1], textureInfo[2], textureInfo[3], 
        HLTextureInfo[0], HLTextureInfo[1], HLTextureInfo[2], HLTextureInfo[3], 
        (int32_t)this->state
    };


    dirtyUi = true;
}

Geometry<UI_Vertex> UIGeometryMap;
bool dirtyUi = false;
std::vector<UIElement*> UIElements;

void updateElementBuffer(SDL_GPUDevice* renderer){
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

    size_t maxVerts = UIGeometryMap.vertices.size() * sizeof(UI_Vertex);
    size_t maxInds  = UIGeometryMap.indices.size()  * sizeof(int);

    Uint32 totalSize = (Uint32)(maxVerts + maxInds);

    SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
    SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

    Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
    

    memcpy(mapPtr, UIGeometryMap.vertices.data(), maxVerts);
    memcpy(mapPtr + maxVerts, UIGeometryMap.indices.data(), maxInds);

    SDL_GPUTransferBufferLocation vertSrc = { tbuf, 0 };
    SDL_GPUBufferRegion vertDst = { UIVBuf,  0,(Uint32)maxVerts};
    SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);

    SDL_GPUTransferBufferLocation indexSrc = { tbuf, (Uint32)maxVerts };
    SDL_GPUBufferRegion indexDst = { UIIBuf,  0, (Uint32)maxInds};
    SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDst, false);
    
    SDL_UnmapGPUTransferBuffer(renderer, tbuf);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(renderer, tbuf);

    dirtyUi = false;
}


