#include "highlight.h"

std::unordered_set<SDL_Point, SDL_PointHash, SDL_PointEquality> highlightRegionMap[MAX_HL_LAYERS];

bool dirtyHighlights = false;

void loadHighlightQuad(SDL_GPUDevice* renderer){
    std::vector<Base_Vertex> vertices;
    std::vector<int> indices;

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

    renderLayers[HIGHLIGHT_RENDER_LAYER]->writeBuffers(renderer, indices, vertices);
    
};

void sortHighlights(SDL_GPUDevice* renderer){

    std::vector<Highlight_Data> fragments;

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

    renderLayers[HIGHLIGHT_RENDER_LAYER]->writeBuffers(renderer, fragments);
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
