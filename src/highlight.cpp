#include "highlight.h"
#include "render.h"
#include "constants.h"
#include "geometry.h"
#include "enginedata.h"

Geometry<Highlight_Vertex> highlightGeometry[BOARD_WIDTH + BOARD_HEIGHT - 1];
int highlightMap[BOARD_WIDTH * BOARD_HEIGHT];
bool dirtyHighlights = false;

void sortHighlight(SDL_GPUDevice* renderer){

    for(int i = 0; i < BOARD_WIDTH + BOARD_HEIGHT - 1; i++) {
        highlightGeometry[i].vertices.clear();
        highlightGeometry[i].indices.clear();
    }
    int vertexOffset[BOARD_WIDTH + BOARD_HEIGHT - 1] = {};

    for (int iM = 0; iM < BOARD_WIDTH * BOARD_HEIGHT; iM++)
    {   
        if(highlightMap[iM] == 0) continue;
        int y = iM / BOARD_WIDTH;
        int x = iM % BOARD_WIDTH;
        addHighlightGeometry({x, y}, vertexOffset, highlightMap[iM]);
        
    }

    if(SELECTED_TILE.x >= 0 && SELECTED_TILE.y >= 0){
        addHighlightGeometry({SELECTED_TILE.x, SELECTED_TILE.y}, vertexOffset, 1);
    }

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

    size_t maxVerts =  BOARD_WIDTH * BOARD_HEIGHT * 4 * sizeof(Highlight_Vertex);
    size_t maxInds =  BOARD_WIDTH * BOARD_HEIGHT * 6 * sizeof(int);
    Uint32 totalSize = (Uint32)(maxVerts + maxInds);

    SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
    SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

    Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);
    Uint32 indexCount = 0;
    Uint32 vertOffset = 0;
    Uint32 indexOffset = 0;
    Uint32 vertexAmt = 0;

    for (int i = 0; i < BOARD_HEIGHT + BOARD_WIDTH - 1; i++)
    { 
        highlightLayer[i].index = indexCount;
        highlightLayer[i].size = (Uint32)highlightGeometry[i].indices.size();
        indexCount += highlightLayer[i].size;

        size_t vertSize = highlightGeometry[i].vertices.size() * sizeof(Highlight_Vertex);
        size_t indexSize = highlightGeometry[i].indices.size() * sizeof(int);

        highlightLayer[i].vertex = (Sint32)vertexAmt;

        if(vertSize > 0){
            memcpy(mapPtr + vertOffset, highlightGeometry[i].vertices.data(), vertSize);
            vertOffset += vertSize;
        }

        if(indexSize > 0){
            Uint32 tbufIndexStart = (Uint32)maxVerts + indexOffset;
            memcpy((Uint8*)mapPtr + tbufIndexStart, highlightGeometry[i].indices.data(), indexSize);
            indexOffset+=indexSize;
        }
        vertexAmt += (Uint32)highlightGeometry[i].vertices.size();

        
    }

    SDL_GPUTransferBufferLocation vertSrc = { tbuf, 0 };
    SDL_GPUBufferRegion vertDst = { highlightVBuf,  0, vertOffset};
    SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);

    SDL_GPUTransferBufferLocation indexSrc = { tbuf, (Uint32)maxVerts };
    SDL_GPUBufferRegion indexDst = { highlightIBuf,  0, indexOffset};
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
void addHighlightGeometry(SDL_Point p, int vertexOffset[], int type){
    if(p.x >= 0 && p.x < BOARD_WIDTH && p.y >= 0 && p.y < BOARD_HEIGHT){
        IsoObject body = tiles[p.y * BOARD_WIDTH + p.x].tile;
                        
        highlightGeometry[p.x+p.y].vertices.push_back({body.surface[0], {0.0f, 0.0f}, type});
        highlightGeometry[p.x+p.y].vertices.push_back({body.surface[1], {1.0f, 0.0f}, type});
        highlightGeometry[p.x+p.y].vertices.push_back({body.surface[2], {0.0f, 1.0f}, type});
        highlightGeometry[p.x+p.y].vertices.push_back({body.surface[3], {1.0f, 1.0f}, type});
        highlightGeometry[p.x+p.y].indices.push_back(vertexOffset[p.x+p.y] + 0);
        highlightGeometry[p.x+p.y].indices.push_back(vertexOffset[p.x+p.y] + 1);
        highlightGeometry[p.x+p.y].indices.push_back(vertexOffset[p.x+p.y] + 2);
        highlightGeometry[p.x+p.y].indices.push_back(vertexOffset[p.x+p.y] + 1);
        highlightGeometry[p.x+p.y].indices.push_back(vertexOffset[p.x+p.y] + 2);
        highlightGeometry[p.x+p.y].indices.push_back(vertexOffset[p.x+p.y] + 3);
        vertexOffset[p.x+p.y]+=4;
    }
}

