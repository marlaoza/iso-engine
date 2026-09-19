#include "lightSource.h"

std::vector<LightSource> lights;
SDL_FColor lightMap[BOARD_WIDTH * BOARD_HEIGHT];
bool dirtyLights = false;

void sortLightmap(SDL_GPUDevice* renderer){
    SDL_Log("sorting lights");
    dirtyLights = false;
    for (auto& c : lightMap) c = {0.08f, 0.08f, 0.1f, 1.0f};

    for (const LightSource& L : lights){
        for (Node n : floodLight(L.gridPos, L.radius)){
            SDL_Log("%d", n.cost);
            float falloff = 1.0f - (float)n.cost / (float)L.radius;
            int idx = n.pos.y * BOARD_WIDTH + n.pos.x;
            float contribution = falloff * L.intensity;

            lightMap[idx].r += L.color.r * contribution;
            lightMap[idx].g += L.color.g * contribution;
            lightMap[idx].b += L.color.b * contribution;
        }
    }
    SDL_Log("sort ok");
    SDL_Log("uploading lights");
    uploadLightmap(renderer);
}

void uploadLightmap(SDL_GPUDevice* renderer){
    SDL_Log("lightmapTexture ptr: %p", (void*)lightmapTexture);
    size_t dataSize = BOARD_WIDTH * BOARD_HEIGHT * sizeof(SDL_FColor);

    SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)dataSize };
    SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);
    void* ptr = SDL_MapGPUTransferBuffer(renderer, tbuf, false);
    memcpy(ptr, lightMap, dataSize);
    SDL_UnmapGPUTransferBuffer(renderer, tbuf);

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);
    SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);

    SDL_GPUTextureTransferInfo src = {};
    src.transfer_buffer = tbuf;
    src.pixels_per_row = BOARD_WIDTH;
    src.rows_per_layer = BOARD_HEIGHT;

    SDL_GPUTextureRegion dst = {};
    dst.texture = lightmapTexture->texture;
    dst.w = BOARD_WIDTH; dst.h = BOARD_HEIGHT; dst.d = 1;

    SDL_UploadToGPUTexture(copy, &src, &dst, false);
    SDL_EndGPUCopyPass(copy);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
     SDL_Log("upload end");
}

