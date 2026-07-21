#include "renderLayer.h"


void RenderLayer::createBuffers(SDL_GPUDevice* renderer){
    size_t vertexSize = maxElements * 4 * vertexShape;
    size_t indexSize = maxElements * 6 * sizeof(int);
    size_t fragmentSize = maxFragmentSize * fragmentShape;

    SDL_GPUBufferCreateInfo vertexInfo;
    if(vertexSize > 0) vertexInfo = { .usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = (Uint32)vertexSize };

    SDL_GPUBufferCreateInfo indexInfo;
    if(indexSize > 0) indexInfo = { .usage = SDL_GPU_BUFFERUSAGE_INDEX, .size = (Uint32)indexSize };

    SDL_GPUBufferCreateInfo fragmentInfo;
    if(fragmentSize > 0) fragmentInfo = { .usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = (Uint32)fragmentSize };

    if(vertexSize > 0) vertexBuffer = SDL_CreateGPUBuffer(renderer, &vertexInfo);
    if(indexSize > 0) indexBuffer = SDL_CreateGPUBuffer(renderer, &indexInfo);
    if(fragmentSize > 0) fragmentBuffer = SDL_CreateGPUBuffer(renderer, &fragmentInfo);
}

void RenderLayer::draw(SDL_GPURenderPass* renderPass){
    int bindingSize = 0;
    SDL_GPUBufferBinding vertexBinding[2];
    if(vertexBuffer){ vertexBinding[0] = { .buffer = vertexBuffer, .offset = 0 }; bindingSize++; }
    if(fragmentBuffer){ vertexBinding[1] = { .buffer = fragmentBuffer, .offset = 0 }; bindingSize++; }
    SDL_GPUBufferBinding indexBinding;
    if(indexBuffer) indexBinding = { .buffer = indexBuffer, .offset = 0 };
    SDL_GPUTextureSamplerBinding textureBinding;
    if(texture && sampler) textureBinding = {.texture = texture, .sampler = sampler};

    SDL_BindGPUGraphicsPipeline(renderPass, pipeline);

    SDL_BindGPUVertexBuffers(renderPass, 0, vertexBinding, bindingSize);

    SDL_BindGPUIndexBuffer(renderPass, &indexBinding, indexElementSize);

    if(texture && sampler) SDL_BindGPUFragmentSamplers(renderPass, 0, &textureBinding, 1);
    
    SDL_DrawGPUIndexedPrimitives(renderPass, indexSize, dataSize, 0, 0, 0);
}
