#include "render.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <SDL3/SDL.h>
#include "enginedata.h"
#include "camera.h"
#include "constants.h"
#include <iostream>

SDL_GPUTexture* swapchainTexture;
SDL_GPUGraphicsPipeline* highLightPipeline;
SDL_GPUGraphicsPipeline* pipeline;
SDL_GPUGraphicsPipeline* UIPipeline;


SDL_GPUBuffer* tileVBuf;
SDL_GPUBuffer* tileIBuf;
Layer_Info tileLayer[BOARD_WIDTH + BOARD_HEIGHT - 1];

SDL_GPUBuffer* highlightVBuf;
SDL_GPUBuffer* highlightIBuf;
Layer_Info highlightLayer[BOARD_WIDTH + BOARD_HEIGHT - 1];

SDL_GPUBuffer* unitVBuf;
SDL_GPUBuffer* unitIBuf;
Layer_Info unitLayer[BOARD_WIDTH + BOARD_HEIGHT - 1];

SDL_GPUBuffer* UIVBuf;
SDL_GPUBuffer* UIIBuf;
int UIIndexSize = 0;


SDL_GPUShader* loadShader(SDL_GPUDevice* device, const char* fileName, SDL_GPUShaderStage stage, Uint32 samplers) {
    size_t size;
    void* code = SDL_LoadFile(fileName, &size); 
    
    SDL_GPUShaderCreateInfo info = {
        .code_size = size,
        .code = (const Uint8*)code,
        .entrypoint = "main", 
        .format = SDL_GPU_SHADERFORMAT_SPIRV, 
        .stage = stage,
        .num_samplers = samplers,
        .num_storage_textures = 0,
        .num_storage_buffers = 0,
        .num_uniform_buffers = 1
    };
    
    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &info);
    SDL_free(code);
    return shader;
}
void createPipeline(SDL_GPUDevice* renderer, SDL_Window* window){
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {};

    SDL_GPUVertexAttribute attributes[5];

    attributes[0].location = 0;
    attributes[0].buffer_slot = 0;
    attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[0].offset = 0;

    attributes[1].location = 1;
    attributes[1].buffer_slot = 0;
    attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_INT2;
    attributes[1].offset = sizeof(float) * 2;

    attributes[2].location = 2;
    attributes[2].buffer_slot = 0;
    attributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    attributes[2].offset = (sizeof(float) * 2) + (sizeof(int)*2);

    attributes[3].location = 3;
    attributes[3].buffer_slot = 0;
    attributes[3].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT;
    attributes[3].offset = (sizeof(float) * 6) + (sizeof(int)*2);

    attributes[4].location = 4;
    attributes[4].buffer_slot = 0;
    attributes[4].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[4].offset = (sizeof(float) * 7) + (sizeof(int)*2);


    pipelineInfo.vertex_shader = loadShader(renderer, "vertShader",SDL_GPU_SHADERSTAGE_VERTEX, 0);
    pipelineInfo.fragment_shader = loadShader(renderer, "fragShader",SDL_GPU_SHADERSTAGE_FRAGMENT, 0);

    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
        .slot = 0,
        .pitch = sizeof(Tile_Vertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0,
    }
    };
    pipelineInfo.vertex_input_state.vertex_attributes = attributes;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 5;

    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
        .format = SDL_GetGPUSwapchainTextureFormat(renderer, window),
        .blend_state = { 
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .color_blend_op = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
            .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
            .enable_blend = true,
        }
    }};

    pipeline = SDL_CreateGPUGraphicsPipeline(renderer, &pipelineInfo);
    if (pipeline == NULL) {
        SDL_Log("Pipeline creation failed: %s", SDL_GetError());
    }

}
void createHighLightPipeline(SDL_GPUDevice* renderer, SDL_Window* window){
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {};

    SDL_GPUVertexAttribute attributes[3];

    attributes[0].location = 0;
    attributes[0].buffer_slot = 0;
    attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[0].offset = 0;

    attributes[1].location = 1;
    attributes[1].buffer_slot = 0;
    attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[1].offset = sizeof(float) * 2;

    attributes[2].location = 2;
    attributes[2].buffer_slot = 0;
    attributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_INT;
    attributes[2].offset = (sizeof(float) * 4);

    pipelineInfo.vertex_shader = loadShader(renderer, "HL_vertShader",SDL_GPU_SHADERSTAGE_VERTEX, 0);
    pipelineInfo.fragment_shader = loadShader(renderer, "HL_fragShader",SDL_GPU_SHADERSTAGE_FRAGMENT, 0);

    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
        .slot = 0,
        .pitch = sizeof(Highlight_Vertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0,
    }
    };
    pipelineInfo.vertex_input_state.vertex_attributes = attributes;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 3;

    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
        .format = SDL_GetGPUSwapchainTextureFormat(renderer, window),
        .blend_state = { 
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .color_blend_op = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
            .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
            .enable_blend = true,
        }
    }};

    highLightPipeline = SDL_CreateGPUGraphicsPipeline(renderer, &pipelineInfo);
    if (highLightPipeline == NULL) {
        SDL_Log("Pipeline creation failed: %s", SDL_GetError());
    }

}
void createUIPipeline(SDL_GPUDevice* renderer, SDL_Window* window){
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {};

    SDL_GPUVertexAttribute attributes[8];

    attributes[0].location = 0;
    attributes[0].buffer_slot = 0;
    attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[0].offset = 0;

    attributes[1].location = 1;
    attributes[1].buffer_slot = 0;
    attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    attributes[1].offset = sizeof(float) * 2;

    attributes[2].location = 2;
    attributes[2].buffer_slot = 0;
    attributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    attributes[2].offset = (sizeof(float) * 4);

    attributes[3].location = 3;
    attributes[3].buffer_slot = 0;
    attributes[3].format = SDL_GPU_VERTEXELEMENTFORMAT_INT2;
    attributes[3].offset = (sizeof(float) * 8);

    attributes[4].location = 4;
    attributes[4].buffer_slot = 0;
    attributes[4].format = SDL_GPU_VERTEXELEMENTFORMAT_INT2;
    attributes[4].offset = (sizeof(float) * 8) + (sizeof(int) * 2);

    attributes[5].location = 5;
    attributes[5].buffer_slot = 0;
    attributes[5].format = SDL_GPU_VERTEXELEMENTFORMAT_INT2;
    attributes[5].offset = (sizeof(float) * 8) + (sizeof(int) * 4);

    attributes[6].location = 6;
    attributes[6].buffer_slot = 0;
    attributes[6].format = SDL_GPU_VERTEXELEMENTFORMAT_INT2;
    attributes[6].offset = (sizeof(float) * 8) + (sizeof(int) * 6);

    attributes[7].location = 7;
    attributes[7].buffer_slot = 0;
    attributes[7].format = SDL_GPU_VERTEXELEMENTFORMAT_INT;
    attributes[7].offset = (sizeof(float) * 8) + (sizeof(int) * 8);



    pipelineInfo.vertex_shader = loadShader(renderer, "UI_vertShader",SDL_GPU_SHADERSTAGE_VERTEX, 0);
    pipelineInfo.fragment_shader = loadShader(renderer, "UI_fragShader",SDL_GPU_SHADERSTAGE_FRAGMENT, 1);

    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
        .slot = 0,
        .pitch = sizeof(UI_Vertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0,
    }};

    pipelineInfo.vertex_input_state.vertex_attributes = attributes;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 8;

    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
        .format = SDL_GetGPUSwapchainTextureFormat(renderer, window),
        .blend_state = {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .color_blend_op = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
            .enable_blend = true,
        }
    }};

    UIPipeline = SDL_CreateGPUGraphicsPipeline(renderer, &pipelineInfo);
    if (UIPipeline == NULL) {
        SDL_Log("Pipeline creation failed: %s", SDL_GetError());
    }
}

SDL_GPUTexture* UISpriteSheet;
SDL_GPUSampler* uiSampler;

void loadUISpriteSheet(SDL_GPUDevice* renderer, const char* path){
    int w, h, channels;
    unsigned char* pixels = stbi_load(path, &w, &h, &channels, 4);
    if(pixels == NULL){
        SDL_Log("Failed to load UI spritesheet: %s", stbi_failure_reason());
        return;
    }

    SDL_GPUTextureCreateInfo texInfo = {};
    texInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    texInfo.type = SDL_GPU_TEXTURETYPE_2D;
    texInfo.width = (Uint32)w;
    texInfo.height = (Uint32)h;
    texInfo.layer_count_or_depth = 1;
    texInfo.num_levels = 1;
    texInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    texInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    UISpriteSheet = SDL_CreateGPUTexture(renderer, &texInfo);

    size_t pixelSize = w * h * 4;
    SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)pixelSize };
    SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);
    void* mapPtr = SDL_MapGPUTransferBuffer(renderer, tbuf, false);
    memcpy(mapPtr, pixels, pixelSize);
    SDL_UnmapGPUTransferBuffer(renderer, tbuf);
    stbi_image_free(pixels);

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

    SDL_GPUTextureTransferInfo src = {};
    src.transfer_buffer = tbuf;
    src.offset = 0;
    src.pixels_per_row = (Uint32)w;
    src.rows_per_layer = (Uint32)h;

    SDL_GPUTextureRegion dst = {};
    dst.texture = UISpriteSheet;
    dst.w = (Uint32)w;
    dst.h = (Uint32)h;
    dst.d = 1;

    SDL_UploadToGPUTexture(copyPass, &src, &dst, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(renderer, tbuf);

    SDL_GPUSamplerCreateInfo samplerInfo = {};
    samplerInfo.min_filter = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    uiSampler = SDL_CreateGPUSampler(renderer, &samplerInfo);
}

int UITextureMap[9][4] = {
    {0, 0, 53, 27}, // botao esq on
    {65, 0, 29, 39}, //botao dir on
    {129, 0, 45, 40}, //skill box
    {0, 64, 53, 27}, //botao esq off
    {65, 64, 29, 39}, //botao dir off
    {129, 64, 64, 32}, //unit bar segment
    {193, 64, 14, 32}, //unit bar end
    {0, 129, 192, 97} //selectedUnit
};

void render(SDL_GPUDevice* renderer, SDL_Window* window){
    if(renderer == NULL){return;}
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);
    
    Uint32 width = (Uint32)WIDTH;
    Uint32 heigth = (Uint32)HEIGHT;

    if(SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchainTexture, &width, &heigth)){
        SceneUniforms myData = { (float)WIDTH, (float)HEIGHT, MOUSE_POS.x, MOUSE_POS.y, CAM_POS.x, CAM_POS.y, CAM_ZOOM, 0, SELECTED_TILE.x, SELECTED_TILE.y, BOARD_HEIGHT + BOARD_WIDTH, 0 };

        SDL_GPUColorTargetInfo colorTarget{};
        colorTarget.texture = swapchainTexture;
        colorTarget.clear_color = {0.0f, 0.0f, 0.0f, 0.0f};
        colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;


        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmd, &colorTarget, 1, NULL);
        
        SDL_GPUBufferBinding tileVBinding = { .buffer = tileVBuf, .offset = 0 };
        SDL_GPUBufferBinding tileIBinding = { .buffer = tileIBuf, .offset = 0 };

        SDL_GPUBufferBinding unitVBinding = { .buffer = unitVBuf, .offset = 0 };
        SDL_GPUBufferBinding unitIBinding = { .buffer = unitIBuf, .offset = 0 };

        
        SDL_GPUBufferBinding highlightVBinding = { .buffer = highlightVBuf, .offset = 0 };
        SDL_GPUBufferBinding highlightIBinding = { .buffer = highlightIBuf, .offset = 0 };

        SDL_GPUBufferBinding UIVBinding = { .buffer = UIVBuf, .offset = 0 };
        SDL_GPUBufferBinding UIIBinding = { .buffer = UIIBuf, .offset = 0 };
        
        SDL_BindGPUGraphicsPipeline(renderPass, pipeline); 
        SDL_PushGPUVertexUniformData(cmd, 0, &myData, sizeof(myData));

        for (int i = 0; i < BOARD_HEIGHT + BOARD_WIDTH - 1; i++)
        {
            
            SDL_BindGPUVertexBuffers(renderPass, 0, &tileVBinding, 1);
            SDL_BindGPUIndexBuffer(renderPass, &tileIBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
            SDL_DrawGPUIndexedPrimitives(renderPass, tileLayer[i].size, 1, tileLayer[i].index, tileLayer[i].vertex, 0);

            if(highlightLayer[i].size > 0){
                SDL_BindGPUGraphicsPipeline(renderPass, highLightPipeline); 
                SDL_PushGPUVertexUniformData(cmd, 0, &myData, sizeof(myData));
                SDL_BindGPUVertexBuffers(renderPass, 0, &highlightVBinding, 1);
                SDL_BindGPUIndexBuffer(renderPass, &highlightIBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
                SDL_DrawGPUIndexedPrimitives(renderPass, highlightLayer[i].size, 1, highlightLayer[i].index, highlightLayer[i].vertex, 0);

                SDL_BindGPUGraphicsPipeline(renderPass, pipeline); 
                SDL_PushGPUVertexUniformData(cmd, 0, &myData, sizeof(myData));
            }
            SDL_BindGPUVertexBuffers(renderPass, 0, &unitVBinding, 1);
            SDL_BindGPUIndexBuffer(renderPass, &unitIBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
            SDL_DrawGPUIndexedPrimitives(renderPass, unitLayer[i].size, 1, unitLayer[i].index, unitLayer[i].vertex, 0);
        } 
        
        SDL_EndGPURenderPass(renderPass);

        if(UISpriteSheet != NULL && uiSampler != NULL){
            colorTarget.load_op = SDL_GPU_LOADOP_LOAD;
            SDL_GPURenderPass* uiPass = SDL_BeginGPURenderPass(cmd, &colorTarget, 1, NULL);
            SDL_BindGPUGraphicsPipeline(uiPass, UIPipeline);
            SDL_PushGPUVertexUniformData(cmd, 0, &myData, sizeof(myData));
            SDL_GPUTextureSamplerBinding binding = { .texture = UISpriteSheet, .sampler = uiSampler };
            SDL_BindGPUFragmentSamplers(uiPass, 0, &binding, 1);
            SDL_BindGPUVertexBuffers(uiPass, 0, &UIVBinding, 1);
            SDL_BindGPUIndexBuffer(uiPass, &UIIBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
            SDL_DrawGPUIndexedPrimitives(uiPass, UIIndexSize * 6, 1, 0, 0, 0);
            SDL_EndGPURenderPass(uiPass);

        }

    }
    
    SDL_SubmitGPUCommandBuffer(cmd);
    
}

SDL_GPUDevice* createRenderer(SDL_Window* window){
    SDL_GPUDevice* renderer = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, NULL);
    if(renderer == NULL){
        std::cout << "Não foi possivel inicializar renderizador GPU: " << SDL_GetError() << std::endl;
        return NULL;
    }
    SDL_ClaimWindowForGPUDevice(renderer, window);
    SDL_SetGPUSwapchainParameters(renderer, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX);

    createPipeline(renderer, window);
    createHighLightPipeline(renderer,window);
    createUIPipeline(renderer, window);

    size_t vertSize = BOARD_HEIGHT*BOARD_WIDTH * 12 * sizeof(Tile_Vertex);
    size_t indexSize = BOARD_HEIGHT*BOARD_WIDTH * 18 * sizeof(int);
    SDL_GPUBufferCreateInfo vInfo = { .usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = (Uint32)vertSize };
    SDL_GPUBufferCreateInfo iInfo = { .usage = SDL_GPU_BUFFERUSAGE_INDEX, .size = (Uint32)indexSize };
    tileVBuf = SDL_CreateGPUBuffer(renderer, &vInfo);
    tileIBuf = SDL_CreateGPUBuffer(renderer, &iInfo);

    size_t unitVertSize = MAX_UNITS * 12 * sizeof(Tile_Vertex);
    size_t unitIndexSize = MAX_UNITS * 18 * sizeof(int);
    SDL_GPUBufferCreateInfo unitVInfo = { .usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = (Uint32)unitVertSize };
    SDL_GPUBufferCreateInfo unitIInfo = { .usage = SDL_GPU_BUFFERUSAGE_INDEX, .size = (Uint32)unitIndexSize };
    unitVBuf = SDL_CreateGPUBuffer(renderer, &unitVInfo);
    unitIBuf = SDL_CreateGPUBuffer(renderer, &unitIInfo);

    size_t highlightVertSize = BOARD_HEIGHT*BOARD_WIDTH * 4 * sizeof(Highlight_Vertex);
    size_t highlightIndexSize = BOARD_HEIGHT*BOARD_WIDTH * 6 * sizeof(int);
    SDL_GPUBufferCreateInfo highlightVInfo = { .usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = (Uint32)highlightVertSize };
    SDL_GPUBufferCreateInfo highlightIInfo = { .usage = SDL_GPU_BUFFERUSAGE_INDEX, .size = (Uint32)highlightIndexSize };
    highlightVBuf = SDL_CreateGPUBuffer(renderer, &highlightVInfo);
    highlightIBuf = SDL_CreateGPUBuffer(renderer, &highlightIInfo);

    size_t UIVertSize = MAX_UI_ELEMENTS * 4 * sizeof(UI_Vertex);
    size_t UIIndexSize = MAX_UI_ELEMENTS * 6 * sizeof(int);
    SDL_GPUBufferCreateInfo UIVInfo = { .usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = (Uint32)UIVertSize };
    SDL_GPUBufferCreateInfo UIIInfo = { .usage = SDL_GPU_BUFFERUSAGE_INDEX, .size = (Uint32)UIIndexSize };
    UIVBuf = SDL_CreateGPUBuffer(renderer, &UIVInfo);
    UIIBuf = SDL_CreateGPUBuffer(renderer, &UIIInfo);

    loadUISpriteSheet(renderer, "src/assets/ui.png");
    

    return renderer;
}

void destroyRenderer(SDL_GPUDevice* renderer){
    SDL_ReleaseGPUBuffer(renderer,tileVBuf);
    SDL_ReleaseGPUBuffer(renderer,tileIBuf);
    SDL_ReleaseGPUBuffer(renderer,unitVBuf);
    SDL_ReleaseGPUBuffer(renderer,unitIBuf);
    SDL_ReleaseGPUBuffer(renderer,highlightVBuf);
    SDL_ReleaseGPUBuffer(renderer,highlightIBuf);
    
    SDL_DestroyGPUDevice(renderer);
}