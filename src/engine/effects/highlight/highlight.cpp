#include "highlight.h"

std::unordered_set<SDL_Point, SDL_PointHash, SDL_PointEquality> highlightRegionMap[MAX_HL_LAYERS];

bool dirtyHighlights = false;

void loadHighlightQuad(SDL_GPUDevice* renderer){
    std::vector<Base_Vertex> vertices;
    std::vector<int> indices;

    highlightIndexSize = 0;

    IsoObject body = tiles[1 * BOARD_WIDTH + 1].tile;

    SDL_FPoint tl = {0, 0};
    SDL_FPoint tr = {tl.x + TILE_SIZE + 1, tl.y + TILE_SIZE/2};
    SDL_FPoint bl = {tl.x - TILE_SIZE, tl.y + TILE_SIZE/2};
    SDL_FPoint br = {tl.x + 1,  tl.y + TILE_SIZE};
                        
    vertices.push_back({{tl.x, tl.y}, {0.0f, 0.0f}});
    vertices.push_back({{tr.x, tr.y}, {1.0f, 0.0f}});
    vertices.push_back({{bl.x, bl.y}, {0.0f, 1.0f}});
    vertices.push_back({{br.x, br.y}, {1.0f, 1.0f}});
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

    size_t vertSize = vertices.size() * sizeof(Base_Vertex);
    size_t indexSize = indices.size() * sizeof(int);
    Uint32 totalSize = (Uint32)(vertSize + indexSize);

    if(totalSize > 0){

        SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
        SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

        Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);


        highlightIndexSize = (Uint32)indices.size();

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
    }
};

void sortHighlights(SDL_GPUDevice* renderer){

    std::vector<Highlight_Data> fragments;

    highlightDataSize = 0;


    SDL_Point neighbors[8] = {
        {-1,-1},{ 0,-1},{ 1,-1},
        {-1, 0},        { 1, 0},
        {-1, 1},{ 0, 1},{ 1, 1}
    };

    for (int l = 0; l <MAX_HL_LAYERS; l++)
    {
        for (SDL_Point p : highlightRegionMap[l]){
            IsoObject body = tiles[p.y * BOARD_WIDTH + p.x].tile;
            
            uint8_t edges = 0;
            
            int c = 0;
            for(SDL_Point n : neighbors){
                SDL_Point nCheck = {p.x + n.x, p.y + n.y};
                if(highlightRegionMap[l].find(nCheck) == highlightRegionMap[l].end()){
                    edges |= (1 << c);
                }
                c++;
            }

                            
            fragments.push_back({body.surface[0], p.x, p.y, l, edges});
        }
    }
    

    if(SELECTED_TILE.x >= 0 && SELECTED_TILE.y >= 0){

        IsoObject body = tiles[SELECTED_TILE.y * BOARD_WIDTH + SELECTED_TILE.x].tile;
                        
        fragments.push_back({body.surface[0], SELECTED_TILE.x, SELECTED_TILE.y, 1, 0});
    }


    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

    size_t fragSize = fragments.size() * sizeof(Highlight_Data);
    Uint32 totalSize = (Uint32)(fragSize);

    if(totalSize > 0){

        SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
        SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

        Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);


        highlightDataSize = (Uint32)fragments.size();

        if(fragSize > 0){
            memcpy(mapPtr + 0, fragments.data(), fragSize);
        }

        SDL_GPUTransferBufferLocation fragSrc = { tbuf, 0};
        SDL_GPUBufferRegion fragDst = { highlightFBuf,  0, (Uint32)fragSize};
        SDL_UploadToGPUBuffer(copyPass, &fragSrc, &fragDst, false);
        
        SDL_UnmapGPUTransferBuffer(renderer, tbuf);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
    }
    dirtyHighlights = false;
}


void addHighlightPoint(SDL_Point p, int type, bool updateBuffer){
    if(p.x >= 0 && p.x < BOARD_WIDTH && p.y >= 0 && p.y < BOARD_HEIGHT){
        highlightRegionMap[type].insert(p);
        if(updateBuffer)dirtyHighlights = true;
    }

}

void clearHighlightPoint(SDL_Point p, int type, bool updateBuffer){
    if(p.x >= 0 && p.x < BOARD_WIDTH && p.y >= 0 && p.y < BOARD_HEIGHT){
        highlightRegionMap[type].erase(p);
        if(updateBuffer)dirtyHighlights = true;
    }
}

void addHighlightRegion(std::vector<SDL_Point> p, int type){
    for (SDL_Point pp : p){addHighlightPoint(pp, type, false);}
    dirtyHighlights = true;
}

void clearHighlightRegion(int type){
    highlightRegionMap[type].clear();
    dirtyHighlights = true;
}
