#include "geometry.h"
#include <vector>
#include "constants.h"
#include <queue>
#include <cmath>
#include "managers/frameManager/frameManager.h"
#include <unordered_map>
#include <cstdio>

class Unit;
extern Unit* unitMap[BOARD_WIDTH * BOARD_HEIGHT];


std::vector<SDL_Point> getPath(SDL_Point origin, SDL_Point target, int maxSize, int minSize, bool checkWalkability){
    std::vector<SDL_Point> path;

    SDL_Point pathArr[maxSize+1];
    for (int i = 0; i < maxSize+1; i++)
    {
        pathArr[i] = {-1, -1};
    }
    pathArr[0] = origin;

    std::unordered_map<int, bool> checked;
    
    SDL_Point prevPoint = {-1, -1};


    int lastStep = 0;
    while(lastStep < maxSize){
        SDL_Point cur = pathArr[lastStep];
        int cId = cur.y * BOARD_WIDTH + cur.x;
        if(cur.x == target.x && cur.y == target.y) break;
        int curDist = 999;
        int curId = cur.y * BOARD_WIDTH + cur.x;
        SDL_Point smallest = cur;
        SDL_Point neighbors[4] = {
            {cur.x + 1, cur.y}, {cur.x - 1, cur.y},
            {cur.x, cur.y + 1}, {cur.x, cur.y - 1}
        };
        for(SDL_Point n : neighbors){
            if (n.x < 0 || n.x >= BOARD_WIDTH || n.y < 0 || n.y >= BOARD_HEIGHT) continue;
            int nId = n.y * BOARD_WIDTH + n.x;
            if(checked[nId]) continue;
            if(unitMap[nId] != nullptr && checkWalkability) continue;
            if((tiles[nId].height - tiles[curId].height > 1) && checkWalkability) continue;
            int dx = target.x - n.x;
            int dy = target.y - n.y;
            int nDist = abs(dx) + abs(dy);
            if(nDist < curDist){
                smallest = n;
                curDist = nDist;
            }     
        }
        if(curDist != 999){
            int nId = smallest.y * BOARD_WIDTH + smallest.x;
            checked[nId] = true;
            lastStep+=1;
            pathArr[lastStep] = smallest;
        }
        else{
            if(lastStep > 0){
                lastStep -=1;
                cur = pathArr[lastStep];
            }else{
                break;
            }
            
        }
    }


    for (int i = 0; i <= lastStep; i++)
    {
        if((pathArr[i].x == -1 || pathArr[i].y == -1))
        break;

        path.push_back(pathArr[i]);

        if((pathArr[i].x == target.x && pathArr[i].y == target.y))
        break;
    }
    
    return path;

}
std::vector<SDL_Point> getStraightPath(SDL_Point origin, SDL_Point target, int maxSize, int minSize, bool checkWalkability)
{
    std::vector<SDL_Point> path;

    path.push_back(origin);
    SDL_Point o = origin;
    while(o.x != target.x || o.y != target.y){
        
        if(o.x < target.x){o.x += 1;}
        else if(o.x > target.x){ o.x -= 1;}

        if(o.y < target.y){o.y += 1;}
        else if(o.y > target.y){ o.y -= 1;}

        path.push_back(o);
    }

    return path;
}
std::vector<SDL_Point> getDiamond(SDL_Point origin, int maxSize, int minSize, bool checkWalkability){
    std::vector<SDL_Point> reachMap;
    reachMap.push_back(origin);
    int uId = origin.y*BOARD_WIDTH + origin.x;
            
    std::vector<int> visited(BOARD_WIDTH * BOARD_HEIGHT, 999);
    std::queue<SDL_Point> pointQueue;
    pointQueue.push(origin);
    visited[uId] = 0;

    while(!pointQueue.empty()){
        SDL_Point cur = pointQueue.front(); pointQueue.pop();
        int cId = cur.y * BOARD_WIDTH + cur.x;
        int cDist = visited[cId];
        int cheight = tiles[cId].height;
        if(cDist > minSize) reachMap.push_back(cur);
        if(cDist >= maxSize) continue;
        SDL_Point neighbors[4] = {
            {cur.x + 1, cur.y}, {cur.x - 1, cur.y},
            {cur.x, cur.y + 1}, {cur.x, cur.y - 1}
        };
        for(SDL_Point n : neighbors){
            if (n.x < 0 || n.x >= BOARD_WIDTH || n.y < 0 || n.y >= BOARD_HEIGHT) continue;
            int nId = n.y * BOARD_WIDTH + n.x;
            if (visited[nId] != 999) continue;
            if (tiles[nId].height - cheight > 1) continue;
            if(checkWalkability && unitMap[nId] != nullptr) continue;
            visited[nId] = cDist + 1;
            pointQueue.push(n);
        }
    }
    return reachMap;
}
std::vector<SDL_Point> getLine(SDL_Point origin, Direction direction, int maxSize, int minSize, bool checkWalkability){
    SDL_Point vector = getDirectionVector(direction);
    std::vector<SDL_Point> path;
    
    SDL_Point o = origin;
    for (int i = minSize; i < maxSize; i++)
    {
        int oId = o.y * BOARD_WIDTH + o.x;
        int oHeight = tiles[oId].height;

        SDL_Point p = {o.x + vector.x, o.y + vector.y};
        o = p;
        if (p.x < 0 || p.x >= BOARD_WIDTH || p.y < 0 || p.y >= BOARD_HEIGHT) continue;
        int pId = p.y * BOARD_WIDTH + p.x;
        if (tiles[pId].height - oHeight > 1) continue;
        if(checkWalkability && unitMap[pId] != nullptr) break;
        path.push_back(p);
        
    }
    return path;    
}
std::vector<SDL_Point> getCross(SDL_Point origin, int maxSize, int minSize, bool checkWalkability){
    std::vector<SDL_Point> path;

    int oId = origin.y * BOARD_WIDTH + origin.x;
    int oHeight = tiles[oId].height;

    for (int i = minSize; i < maxSize; i++)
    {
         SDL_Point neighbors[4] = {
            {origin.x + i, origin.y}, {origin.x - i, origin.y},
            {origin.x, origin.y + i}, {origin.x, origin.y - i}
        };
        for(SDL_Point p : neighbors){   
            if (p.x < 0 || p.x >= BOARD_WIDTH || p.y < 0 || p.y >= BOARD_HEIGHT) continue;
            int pId = p.y * BOARD_WIDTH + p.x;
            if (tiles[pId].height - oHeight > 1) continue;
            if(checkWalkability && unitMap[pId] != nullptr) continue;
            path.push_back(p);
        }
    }

    
    return path;
}
std::vector<SDL_Point> getX(SDL_Point origin, int maxSize, int minSize, bool checkWalkability){
    std::vector<SDL_Point> path;

    int oId = origin.y * BOARD_WIDTH + origin.x;
    int oHeight = tiles[oId].height;

    for (int i = minSize; i < maxSize; i++)
    {
         SDL_Point neighbors[4] = {
            {origin.x + i, origin.y + i}, {origin.x + i, origin.y + -i},
            {origin.x + -i, origin.y + i}, {origin.x + -i, origin.y + -i}
        };
        
        for(SDL_Point p : neighbors){   
            if (p.x < 0 || p.x >= BOARD_WIDTH || p.y < 0 || p.y >= BOARD_HEIGHT) continue;
            int pId = p.y * BOARD_WIDTH + p.x;
            if (tiles[pId].height - oHeight > 1) continue;
            if(checkWalkability && unitMap[pId] != nullptr) continue;
            path.push_back(p);
        }
    }
    return path;
}
std::vector<SDL_Point> getCircle(SDL_Point origin, int maxSize, int minSize, bool checkWalkability){
    std::vector<SDL_Point> path;

    int oId = origin.y * BOARD_WIDTH + origin.x;
    int oHeight = tiles[oId].height;

    for (int dx = -maxSize; dx <= maxSize; dx++)
    {
        for (int dy = -maxSize; dy <= maxSize; dy++)
        {
            float dist = sqrt(dx*dx + dy*dy);
            if (dist > maxSize) continue; 
        
            SDL_Point p = {origin.x + dx, origin.y + dy};
            if (p.x < 0 || p.x >= BOARD_WIDTH || p.y < 0 || p.y >= BOARD_HEIGHT) continue;
            int pId = p.y * BOARD_WIDTH + p.x;
            if (tiles[pId].height - oHeight > 1) continue;
            if(checkWalkability && unitMap[pId] != nullptr) continue;
            path.push_back(p);
            
        }
    }
    return path;
}
std::vector<SDL_Point> getCircumference(SDL_Point origin, int maxSize, int minSize, bool checkWalkability){
    std::vector<SDL_Point> path;

    int oId = origin.y * BOARD_WIDTH + origin.x;
    int oHeight = tiles[oId].height;

    for (int dx = -maxSize; dx <= maxSize; dx++)
    {
        for (int dy = -maxSize; dy <= maxSize; dy++)
        {
            float dist = sqrt(dx*dx + dy*dy);
            if (dist > maxSize) continue; 
            
            SDL_Point p = {origin.x + dx, origin.y + dy};
            if (p.x < 0 || p.x >= BOARD_WIDTH || p.y < 0 || p.y >= BOARD_HEIGHT) continue;
            int pId = p.y * BOARD_WIDTH + p.x;
            if (tiles[pId].height - oHeight > 1) continue;
            if(checkWalkability && unitMap[pId] != nullptr) continue;
            if (dist > maxSize || dist < maxSize - 1) continue;
            path.push_back(p);            
        }
    }
    return path;
}


bool pointInPolygon(SDL_FPoint point, int indices[], int n_triangules, const SDL_FPoint vertices[]){

    for(int i = 0; i < n_triangules; i++){
        SDL_FPoint a = vertices[indices[0 + (i*3)]];
        SDL_FPoint b = vertices[indices[1 + (i*3)]];
        SDL_FPoint c = vertices[indices[2 + (i*3)]];
        
        float area = 0.5f *(-b.y*c.x + a.y*(-b.x + c.x) + a.x*(b.y - c.y) + b.x*c.y);

        float s = 1.0f/(2.0f*area)*(a.y*c.x - a.x*c.y + (c.y - a.y)*point.x + (a.x - c.x)*point.y);
        float t = 1.0f/(2.0f*area)*(a.x*b.y - a.y*b.x + (a.y - b.y)*point.x + (b.x - a.x)*point.y);

        if(s >= 0 && t >=0 && (s + t) <= 1){return true;}
    }
    
    return false;
}

Direction getDirection(SDL_Point a, SDL_Point b){
    Direction direction = Direction::Left;
    SDL_Point directionPoint = {a.x - b.x, a.y - b.y};

    if(directionPoint.y >= 1 && directionPoint.x == 0)direction = Direction::Right;
    else if(directionPoint.y < 0 && directionPoint.x == 0)direction = Direction::Left;
    else if(directionPoint.y >= 1 && directionPoint.x >= 1)direction = Direction::UpRight;
    else if(directionPoint.y < 0 && directionPoint.x >= 1)direction = Direction::UpLeft;
    else if(directionPoint.y >= 1 && directionPoint.x < 0)direction = Direction::DownRight;
    else if(directionPoint.y < 0 && directionPoint.x < 0)direction = Direction::DownLeft;
    else if(directionPoint.y == 0 && directionPoint.x >= 1)direction = Direction::Up;
    else if(directionPoint.y == 0 && directionPoint.x < 0)direction = Direction::Down;
    
    return direction;
}
SDL_Point getDirectionVector(Direction d){
    switch (d){
        case Direction::Up:
            return {1, 0};
            break;
        case Direction::UpLeft:
            return {1, -1};
            break;
        case Direction::UpRight:
            return {1, 1};
            break;
        case Direction::Down:
            return {-1, 0};
            break;
        case Direction::DownLeft:
            return {-1, -1};
            break;
        case Direction::DownRight:
            return {-1, 1};
            break;
        case Direction::Left:
            return {0, -1};
            break;
        case Direction::Right:
            return {0, 1};
            break;
        default:
            break;
    }
}