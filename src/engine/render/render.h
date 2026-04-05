#pragma once
#include "SDL_gpu.h"
#include <vector>
#include "constants.h"
#include <SDL3_ttf/SDL_ttf.h>

extern SDL_GPUDevice* renderer;

extern std::vector<SDL_GPUTexture*> textDrawCalls;


struct Tile_Vertex {
    SDL_FPoint pos;
    int32_t gridX, gridY;
    SDL_FColor col;
    float faceID;  // 0 = Surface, 1 = Wall Left, 2 = Wall Right
    SDL_FPoint uv;
};

struct Entity_Vertex {
    SDL_FPoint pos;
    SDL_FPoint uv;
    int sheetIndex;
    int frameWidth, frameHeight;
    int frameCount;
    int direction;
    int speed;
    int32_t gridX, gridY;
    int indexOffset;
};

struct Highlight_Vertex {
    SDL_FPoint pos;
    SDL_FPoint uv;
    int32_t type;
    int32_t gridX, gridY;
};

struct Particle_Vertex{
    SDL_FPoint pos;
    SDL_FPoint uv;
};

struct Particle_Data {
    SDL_FPoint particlePos;
    int32_t gridX, gridY;
    float velX, velY;
    float instanceTime;
    float lifeTime;
    float size;
};

struct UI_Vertex {
    SDL_FPoint pos;
    SDL_FPoint uv;
    SDL_FColor tint;
    int32_t textureX, textureY;
    int32_t textureWidth, textureHeight;
    int32_t highlightTextureX, highlightTextureY;
    int32_t highlightTextureWidth, highlightTextureHeight;
    int32_t state;
};

struct Text_Vertex {
    SDL_FPoint pos;
    SDL_FPoint uv;
    SDL_FColor color;
    int32_t state;
};

template<typename T>
struct Geometry {
    std::vector<T> vertices;
    std::vector<int> indices;
};

struct Layer_Info {
    Uint32 size;
    Uint32 index;
    Sint32 vertex;
};

extern SDL_GPUTexture* swapchainTexture;

extern SDL_GPUGraphicsPipeline* highLightPipeline;
extern SDL_GPUGraphicsPipeline* pipeline;
extern SDL_GPUGraphicsPipeline* UIPipeline;

SDL_GPUShader* loadShader(SDL_GPUDevice* device, const char* fileName, SDL_GPUShaderStage stage, Uint32 samplers = 0);

extern TTF_Font* font_main;

SDL_GPUTexture* createTextTexture(const char* text, SDL_FColor color, TTF_Font* font, int* width, int* height, int maxWidth = 0);

extern SDL_GPUTexture* UISpriteSheet;
extern SDL_GPUSampler* uiSampler;

void loadUISpriteSheet(SDL_GPUDevice* renderer, const char* path);

extern SDL_GPUBuffer* tileVBuf;
extern SDL_GPUBuffer* tileIBuf;
extern int tileIndexSize;

extern SDL_GPUBuffer* highlightVBuf;
extern SDL_GPUBuffer* highlightIBuf;
extern int highLightIndexSize;

extern SDL_GPUBuffer* particleVBuf;
extern SDL_GPUBuffer* particleIBuf;
extern SDL_GPUBuffer* particleFBuf;
extern int particleIndexSize;
extern int particleDataSize;

extern SDL_GPUBuffer* unitVBuf;
extern SDL_GPUBuffer* unitIBuf;
extern int unitIndexSize;

extern SDL_GPUBuffer* projectileVBuf;
extern SDL_GPUBuffer* projectileIBuf;
extern int projectileIndexSize;

extern SDL_GPUBuffer* UIVBuf;
extern SDL_GPUBuffer* UIIBuf;
extern int UIIndexSize;

extern SDL_GPUBuffer* textVBuf;
extern SDL_GPUBuffer* textIBuf;
extern int TextIndexSize;

void render(SDL_GPUDevice* renderer, SDL_Window* window);
SDL_GPUDevice* createRenderer(SDL_Window* window);
void destroyRenderer(SDL_GPUDevice* renderer);


extern int unitSheetCount;

int loadUnitSheet(const char* path);