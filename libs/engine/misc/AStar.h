#ifndef ASTAR_H
#define ASTAR_H
#include "Utils.h"
#include <stdlib.h>

struct AStar_Node
{
    Vector2Int  position;
    uint32_t    gCost;
    uint32_t    hCost;
    uint32_t    fCost;
    AStar_Node* parent;
    bool        closed;
};

Vector2Int8 AStar_GetMoveDirection(const Vector2Int& startPos, const Vector2Int& targetPos, uint16_t maxSearchArea,
                                   bool (*hFunc)(Vector2Int, uint16_t& outCost));

bool AStar_IsPathAvailable(const Vector2Int& startPos, const Vector2Int& targetPos, uint16_t maxSearchArea,
                           bool (*hFunc)(Vector2Int, uint16_t& outCost));

bool AStar_GetPath(const Vector2Int& startPos, const Vector2Int& targetPos, uint16_t maxSearchArea,
                   Vector2Int* outPathBuffer, size_t outPathBufferSize, size_t& outPathLength,
                   bool (*hFunc)(Vector2Int, uint16_t& outCost));

#endif  // ASTAR_H
