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
    int vertexBindings = 0;
    SDL_GPUBufferBinding vertexBinding[2];
    if(vertexBuffer){ vertexBinding[0] = { .buffer = vertexBuffer, .offset = 0 }; vertexBindings++;}
    if(fragmentBuffer){ vertexBinding[1] = { .buffer = fragmentBuffer, .offset = 0 }; vertexBindings++;}
    SDL_GPUBufferBinding indexBinding;
    if(indexBuffer) indexBinding = { .buffer = indexBuffer, .offset = 0 };

    int bindingSize = this->textures.size();
    SDL_GPUTextureSamplerBinding bindings[bindingSize];
    for (int i = 0; i<bindingSize; i++)
    {
        bindings[i] = {.texture = this->textures[i]->texture, .sampler = this->textures[i]->sampler};
    }
    
    SDL_BindGPUGraphicsPipeline(renderPass, pipeline);

    SDL_BindGPUVertexBuffers(renderPass, 0, vertexBinding, vertexBindings);

    SDL_BindGPUIndexBuffer(renderPass, &indexBinding, indexElementSize);

    if(bindingSize > 0) SDL_BindGPUFragmentSamplers(renderPass, 0, bindings, bindingSize);
    
    SDL_DrawGPUIndexedPrimitives(renderPass, indexSize, dataSize, 0, 0, 0);
}
