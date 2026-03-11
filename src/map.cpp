#include "map.h"
#include "enginedata.h"

MapInfo map[BOARD_WIDTH][BOARD_HEIGHT] = {
    {{0,3},{0,2},{0,1},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}},
    {{0,3},{0,2},{0,1},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}},
    {{0,3},{0,2},{0,1},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}},
    {{0,3},{0,2},{0,1},{0,0},{0,0},{0,0},{0,0},{0,5},{0,0},{0,0}},
};

bool dirtyMap = false;


void sortTilePoints(SDL_GPUDevice* renderer){
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer);

    size_t maxVerts = BOARD_WIDTH * BOARD_HEIGHT * 12 * sizeof(Tile_Vertex);
    size_t maxInds = BOARD_WIDTH * BOARD_HEIGHT * 18 * sizeof(int);
    Uint32 totalSize = (Uint32)(maxVerts + maxInds);

    SDL_GPUTransferBufferCreateInfo tbufInfo = { .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = (Uint32)totalSize };
    SDL_GPUTransferBuffer* tbuf = SDL_CreateGPUTransferBuffer(renderer, &tbufInfo);

    Uint8* mapPtr = (Uint8*)SDL_MapGPUTransferBuffer(renderer, tbuf, false);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);


    SDL_FPoint lastPoint = {0,0};
    SDL_FPoint rightVec = { (float)TILE_SIZE + 1,  (float)TILE_SIZE/2 };
    SDL_FPoint downVec  = { (float)TILE_SIZE,  (float)TILE_SIZE/2 };

    int vertexOffset = 0;
    std::vector<Tile_Vertex> vertices;
    std::vector<int> indices;

    tileIndexSize = 0;
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        lastPoint = {0  + (x*rightVec.x), 0 + (x*rightVec.y)};

        for (int y = 0; y < BOARD_HEIGHT; y++)
        {
            SDL_FPoint tl = lastPoint;
            SDL_FPoint tr = {tl.x + rightVec.x, tl.y + rightVec.y};
            SDL_FPoint bl = {tl.x - TILE_SIZE, tl.y + TILE_SIZE/2};
            SDL_FPoint br = {tl.x + 1,  tl.y + TILE_SIZE};

            int height = (map[x][y].height + 1) * (TILE_SIZE/2);

            lastPoint = bl;

            //paredes
            SDL_FPoint points_w[6] = {
                {tr.x, tr.y - height},
                {br.x, br.y - height},
                {bl.x, bl.y - height},
                {tr.x, tr.y},
                {br.x, br.y},
                {bl.x, bl.y}
            };

            SDL_FColor color = (x + y) % 2 == 0 ? WHITE : GRAY_75;

            vertices.push_back({points_w[0], x,y, color, 1.0f, {0.0f, 0.0f}});
            vertices.push_back({points_w[1], x,y, color, 1.0f, {1.0f, 0.0f}});
            vertices.push_back({points_w[3], x,y, color, 1.0f, {0.0f, 1.0f}});
            vertices.push_back({points_w[4], x,y, color, 1.0f, {1.0f, 1.0f}});

            indices.push_back(vertexOffset + 0);
            indices.push_back(vertexOffset + 1);
            indices.push_back(vertexOffset + 2);
            indices.push_back(vertexOffset + 1);
            indices.push_back(vertexOffset + 2);
            indices.push_back(vertexOffset + 3);
            vertexOffset += 4;
            
            vertices.push_back({points_w[1], x,y, color, 2.0f, {0.0f, 0.0f}});
            vertices.push_back({points_w[2], x,y, color, 2.0f, {1.0f, 0.0f}});
            vertices.push_back({points_w[4], x,y, color, 2.0f, {0.0f, 1.0f}});
            vertices.push_back({points_w[5], x,y, color, 2.0f, {1.0f, 1.0f}});

            indices.push_back(vertexOffset + 0);
            indices.push_back(vertexOffset + 1);
            indices.push_back(vertexOffset + 2);
            indices.push_back(vertexOffset + 1);
            indices.push_back(vertexOffset + 2);
            indices.push_back(vertexOffset + 3);
            vertexOffset += 4;

            SDL_FPoint points[4] = {
                {tl.x, tl.y - height},
                {tr.x, tr.y - height},
                {bl.x, bl.y - height},
                {br.x, br.y - height}
            };
            vertices.push_back({points[0], x,y, color, 0.0f, {0.0f, 0.0f}});
            vertices.push_back({points[1], x,y, color, 0.0f, {1.0f, 0.0f}});
            vertices.push_back({points[2], x,y, color, 0.0f, {0.0f, 1.0f}});
            vertices.push_back({points[3], x,y, color, 0.0f, {1.0f, 1.0f}});

            indices.push_back(vertexOffset + 0);
            indices.push_back(vertexOffset + 1);
            indices.push_back(vertexOffset + 2);
            indices.push_back(vertexOffset + 1);
            indices.push_back(vertexOffset + 2);
            indices.push_back(vertexOffset + 3);

            memcpy(tiles[y*BOARD_WIDTH + x].tile.surface, points, sizeof(points));
            memcpy(tiles[y*BOARD_WIDTH + x].tile.wall, points_w, sizeof(points_w));
            tiles[y*BOARD_WIDTH + x].gridPos = {x, y};
            tiles[y*BOARD_WIDTH + x].height = map[x][y].height;
            vertexOffset+=4;
        }   
    }

    tileIndexSize = (Uint32)indices.size();

    size_t vertSize = vertices.size() * sizeof(Tile_Vertex);
    size_t indexSize = indices.size() * sizeof(int);
    if(vertSize > 0){
        memcpy(mapPtr + 0, vertices.data(), vertSize);
    }
    if(indexSize > 0){
        memcpy((Uint8*)mapPtr + vertSize, indices.data(), indexSize);
    }

    SDL_GPUTransferBufferLocation vertSrc = { tbuf, 0 };
    SDL_GPUBufferRegion vertDst = { tileVBuf,  0, (Uint32)vertSize};
    SDL_UploadToGPUBuffer(copyPass, &vertSrc, &vertDst, false);

    SDL_GPUTransferBufferLocation indexSrc = { tbuf, (Uint32)vertSize };
    SDL_GPUBufferRegion indexDst = { tileIBuf,  0, (Uint32)indexSize};
    SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDst, false);
    
    SDL_UnmapGPUTransferBuffer(renderer, tbuf);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(renderer, tbuf);
    dirtyMap = false;
}
