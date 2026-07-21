#pragma once
#include "SDL_gpu.h"
#include "vector"
#include <SDL3/SDL.h>

class RenderLayer {
    private:
        SDL_GPUBuffer* vertexBuffer;
        int vertexShape;
        SDL_GPUBuffer* fragmentBuffer;
        int fragmentShape;

        int dataSize;

        SDL_GPUBuffer* indexBuffer;
        int indexSize;
        SDL_GPUIndexElementSize indexElementSize;

        SDL_GPUGraphicsPipeline* pipeline;
        SDL_GPUTexture* texture;
        SDL_GPUSampler* sampler;

        int maxElements;
        int maxFragmentSize;
    public:
        RenderLayer(int vertexShape, int fragmentShape, int maxElements, int maxFragmentSize){
            this->vertexShape = vertexShape;
            this->fragmentShape = fragmentShape;
            this->maxElements = maxElements;
            this->maxFragmentSize = maxFragmentSize;

            pipeline = nullptr;
            texture = nullptr;
            sampler = nullptr;

            vertexBuffer = nullptr;
            indexBuffer = nullptr;
            fragmentBuffer = nullptr;

            dataSize = 0;
            indexSize = 0;
            indexElementSize = SDL_GPU_INDEXELEMENTSIZE_32BIT;
        }

        void clearBuffers(SDL_GPUDevice* renderer){
            if(vertexBuffer) SDL_ReleaseGPUBuffer(renderer, vertexBuffer);
            if(indexBuffer) SDL_ReleaseGPUBuffer(renderer, indexBuffer);
            if(fragmentBuffer) SDL_ReleaseGPUBuffer(renderer, fragmentBuffer);
        }

        void bindPipeline(SDL_GPUGraphicsPipeline* pipeline) {this->pipeline = pipeline; };
        void bindTexture(SDL_GPUTexture* texture) {this->texture = texture; };
        void bindSampler(SDL_GPUSampler* sampler) {this->sampler = sampler; };

        template<typename T, typename D>
        void writeBuffers(SDL_GPUDevice* renderer, std::vector<int> indexes, std::vector<T> vertices, std::vector<D> fragments){
            SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

            size_t vertSize = vertices.size() * vertexShape;
            size_t indexSize = indexes.size() * sizeof(int);
            size_t fragmentSize = fragments.size() * fragmentShape;

            Uint32 totalSize = (Uint32)(vertSize + indexSize + fragmentSize);
            SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
            SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);
            Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);
            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

            this->dataSize = (Uint32)fragments.size();
            this->indexSize = (Uint32)indexes.size();
            memcpy(mapPtr + 0, vertices.data(), vertSize);
            memcpy((Uint8*)(mapPtr + vertSize), indexes.data(), indexSize);
            memcpy((Uint8*)(mapPtr + vertSize + indexSize), fragments.data(), fragmentSize);

            SDL_GPUTransferBufferLocation vertSrc = { tbuf, 0 };
            SDL_GPUBufferRegion vertDst = { vertexBuffer,  0, (Uint32)vertSize};
            SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);

            SDL_GPUTransferBufferLocation indexSrc = { tbuf, (Uint32)vertSize };
            SDL_GPUBufferRegion indexDst = { indexBuffer,  0, (Uint32)indexSize};
            SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDst, false);

            SDL_GPUTransferBufferLocation fragSrc = { tbuf, (Uint32)(vertSize + indexSize)};
            SDL_GPUBufferRegion fragDst = { fragmentBuffer,  0, (Uint32)fragmentSize};
            SDL_UploadToGPUBuffer(copyPass, &fragSrc, &fragDst, false);

            SDL_UnmapGPUTransferBuffer(renderer, tbuf);
            SDL_EndGPUCopyPass(copyPass);
            SDL_SubmitGPUCommandBuffer(cmd);
            SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
        }

        template<typename T>
        void writeBuffers(SDL_GPUDevice* renderer, std::vector<int> indexes, std::vector<T> vertices){
            SDL_Log("loading vertex and index buffers");
            SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

            size_t vertSize = vertices.size() * vertexShape;
            size_t indexSize = indexes.size() * sizeof(int);

            Uint32 totalSize = (Uint32)(vertSize + indexSize);
            SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
            SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);
            Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);
            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

            this->dataSize = 1;
            this->indexSize = (Uint32)indexes.size();
            memcpy(mapPtr + 0, vertices.data(), vertSize);
            memcpy((Uint8*)(mapPtr + vertSize), indexes.data(), indexSize);

            SDL_GPUTransferBufferLocation vertSrc = { tbuf, 0 };
            SDL_GPUBufferRegion vertDst = { vertexBuffer,  0, (Uint32)vertSize};
            SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);

            SDL_GPUTransferBufferLocation indexSrc = { tbuf, (Uint32)vertSize };
            SDL_GPUBufferRegion indexDst = { indexBuffer,  0, (Uint32)indexSize};
            SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDst, false);

            SDL_UnmapGPUTransferBuffer(renderer, tbuf);
            SDL_EndGPUCopyPass(copyPass);
            SDL_SubmitGPUCommandBuffer(cmd);
            SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
            SDL_Log("loaded vertex and index buffers");
        }

        template<typename T>
        void writeBuffers(SDL_GPUDevice* renderer, std::vector<T> fragments){
            SDL_Log("loading fragment buffer");
            SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

            size_t fragmentSize = fragments.size() * fragmentShape;

            Uint32 totalSize = (Uint32)(fragmentSize);
            SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
            SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);
            Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);
            SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

            this->dataSize = (Uint32)fragments.size();
            memcpy((Uint8*)(mapPtr), fragments.data(), fragmentSize);

            SDL_GPUTransferBufferLocation fragSrc = { tbuf, 0};
            SDL_GPUBufferRegion fragDst = { fragmentBuffer,  0, (Uint32)fragmentSize};
            SDL_UploadToGPUBuffer(copyPass, &fragSrc, &fragDst, false);

            SDL_UnmapGPUTransferBuffer(renderer, tbuf);
            SDL_EndGPUCopyPass(copyPass);
            SDL_SubmitGPUCommandBuffer(cmd);
            SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
            SDL_Log("loaded");
        }

        void draw(SDL_GPURenderPass* renderPass);

        void createBuffers(SDL_GPUDevice* renderer);
};