#ifndef ASTAR_H
#define ASTAR_H
#include "Utils.h"

#include <stdlib.h>

struct AStar_Node
{
    Vector2Int  position;
    uint16_t    gCost;
    uint16_t    hCost;
    uint32_t    fCost;
    AStar_Node* parent;
    bool        closed;
    bool        valid;
};

typedef bool (*HeuristicFuncPtr)(const Vector2Int, const Vector2Int, uint16_t& outCost);

Vector2Int8 AStar_GetMoveDirection(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea,
                                   HeuristicFuncPtr hFunc);

bool AStar_IsPathAvailable(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea, HeuristicFuncPtr hFunc);

uint16_t AStar_GetPath(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea, Vector2Int* outPathBuffer,
                       size_t outPathBufferSize, size_t& outPathLength, HeuristicFuncPtr hFunc);

#endif  // ASTAR_H
