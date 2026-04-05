#include "highlight.h"

int highlightMap[BOARD_WIDTH * BOARD_HEIGHT];
bool dirtyHighlights = false;

void sortHighlight(SDL_GPUDevice* renderer){

    std::vector<Highlight_Vertex> vertices;
    std::vector<int> indices;

    int vertexOffset = 0;

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

    size_t maxVerts =  BOARD_WIDTH * BOARD_HEIGHT * 4 * sizeof(Highlight_Vertex);
    size_t maxInds =  BOARD_WIDTH * BOARD_HEIGHT * 6 * sizeof(int);
    Uint32 totalSize = (Uint32)(maxVerts + maxInds);

    SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
    SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

    Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

    highLightIndexSize = 0;
    for (int iM = 0; iM < BOARD_WIDTH * BOARD_HEIGHT; iM++)
    {   
        if(highlightMap[iM] == 0) continue;
        int y = iM / BOARD_WIDTH;
        int x = iM % BOARD_WIDTH;

        IsoObject body = tiles[y * BOARD_WIDTH + x].tile;
                        
        vertices.push_back({body.surface[0], {0.0f, 0.0f}, highlightMap[iM], x, y});
        vertices.push_back({body.surface[1], {1.0f, 0.0f}, highlightMap[iM], x, y});
        vertices.push_back({body.surface[2], {0.0f, 1.0f}, highlightMap[iM], x, y});
        vertices.push_back({body.surface[3], {1.0f, 1.0f}, highlightMap[iM], x, y});
        indices.push_back(vertexOffset + 0);
        indices.push_back(vertexOffset + 1);
        indices.push_back(vertexOffset + 2);
        indices.push_back(vertexOffset + 1);
        indices.push_back(vertexOffset + 2);
        indices.push_back(vertexOffset + 3);
        vertexOffset+=4;
        
    }

    if(SELECTED_TILE.x >= 0 && SELECTED_TILE.y >= 0){

        IsoObject body = tiles[SELECTED_TILE.y * BOARD_WIDTH + SELECTED_TILE.x].tile;
                        
        vertices.push_back({body.surface[0], {0.0f, 0.0f}, 1, SELECTED_TILE.x, SELECTED_TILE.y});
        vertices.push_back({body.surface[1], {1.0f, 0.0f}, 1, SELECTED_TILE.x, SELECTED_TILE.y});
        vertices.push_back({body.surface[2], {0.0f, 1.0f}, 1, SELECTED_TILE.x, SELECTED_TILE.y});
        vertices.push_back({body.surface[3], {1.0f, 1.0f}, 1, SELECTED_TILE.x, SELECTED_TILE.y});
        indices.push_back(vertexOffset + 0);
        indices.push_back(vertexOffset + 1);
        indices.push_back(vertexOffset + 2);
        indices.push_back(vertexOffset + 1);
        indices.push_back(vertexOffset + 2);
        indices.push_back(vertexOffset + 3);
        vertexOffset+=4;
    }


    highLightIndexSize = (Uint32)indices.size();

    size_t vertSize = vertices.size() * sizeof(Highlight_Vertex);
    size_t indexSize = indices.size() * sizeof(int);
    if(vertSize > 0){
        memcpy(mapPtr + 0, vertices.data(), vertSize);
    }
    if(indexSize > 0){
        memcpy((Uint8*)mapPtr + vertSize, indices.data(), indexSize);
    }

    SDL_GPUTransferBufferLocation vertSrc = { tbuf, 0 };
    SDL_GPUBufferRegion vertDst = { highlightVBuf,  0, (Uint32)vertSize};
    SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);

    SDL_GPUTransferBufferLocation indexSrc = { tbuf, (Uint32)vertSize };
    SDL_GPUBufferRegion indexDst = { highlightIBuf,  0, (Uint32)indexSize};
    SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDst, false);
    
    SDL_UnmapGPUTransferBuffer(renderer, tbuf);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
    dirtyHighlights = false;
}
void addHighlight(SDL_Point p, int type, int pallete){
    if(p.x >= 0 && p.x < BOARD_WIDTH && p.y >= 0 && p.y < BOARD_HEIGHT){
        highlightMap[p.y * BOARD_WIDTH + p.x] = type;
        dirtyHighlights = true;
    }

}
void clearHighlight(SDL_Point p){
    if(p.x >= 0 && p.x < BOARD_WIDTH && p.y >= 0 && p.y < BOARD_HEIGHT){
        highlightMap[p.y * BOARD_WIDTH + p.x] = 0;
        dirtyHighlights = true;
    }
}
