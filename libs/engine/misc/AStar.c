#include "AStar.h"

#include "Logger.h"

#include <cstddef>
#include <stdint.h>

AStar_Node* AStar_CalucalatePath(AStar_Node* nodeArray, size_t nodeArraySize, const Vector2Int& startPos,
                                 const Vector2Int& targetPos, bool (*hFunc)(Vector2Int, uint16_t& outCost))
{
    LOG_INF("A* Pathfinding from (%d, %d) to (%d, %d)", startPos.x, startPos.y, targetPos.x, targetPos.y);
    uint16_t nodeListCount = 0;

    AStar_Node* currentNode = NULL;
    // Initialize start node
    AStar_Node startNode;
    startNode.position         = startPos;
    startNode.gCost            = 0;
    startNode.hCost            = Utils_ManhattanDistance(startPos, targetPos);
    startNode.fCost            = startNode.gCost + startNode.hCost;
    startNode.parent           = NULL;
    startNode.closed           = false;
    nodeArray[nodeListCount++] = startNode;
    while (nodeListCount < nodeArraySize)
    {
        // Find node with lowest fCost
        uint16_t lowerstFCost = UINT16_MAX;
        uint16_t lowestIndex  = 0;
        bool     anyOpen      = false;
        for (uint16_t i = 0; i < nodeListCount; i++)
        {
            if (nodeArray[i].closed == false)
            {
                anyOpen = true;
            }
            if (nodeArray[i].fCost < lowerstFCost && nodeArray[i].closed == false)
            {
                lowerstFCost = nodeArray[i].fCost;
                lowestIndex  = i;
            }
        }
        if (!anyOpen)
        {
            LOG_WRN("No path found to target (%d, %d)", targetPos.x, targetPos.y);
            return NULL;
        }
        // LOG_INF("Current Node (%d, %d) fCost: %d", openList[lowestIndex].position.x,
        //         openList[lowestIndex].position.y, openList[lowestIndex].fCost);
        currentNode = &nodeArray[lowestIndex];
        // Check if reached target
        if (currentNode->position.x == targetPos.x && currentNode->position.y == targetPos.y)
        {
            LOG_INF("Reached target at (%d, %d)", currentNode->position.x, currentNode->position.y);
            return currentNode;
        }
        if (nodeListCount >= 256)
        {
            LOG_ERR("Closed list overflow in A* pathfinding");
            return NULL;
        }
        // Move current node from open to closed list
        currentNode->closed    = true;
        uint16_t hCost         = 0;
        bool     validPosition = hFunc(currentNode->position, hCost);
        if (validPosition && currentNode->parent != NULL)
        {
            continue;
        }
        // Check neighbors
        Vector2Int neighbors[4] = {
            { currentNode->position.x + 1, currentNode->position.y },
            { currentNode->position.x - 1, currentNode->position.y },
            { currentNode->position.x, currentNode->position.y + 1 },
            { currentNode->position.x, currentNode->position.y - 1 },
        };
        for (uint8_t i = 0; i < 4; i++)
        {
            Vector2Int neighborPos  = neighbors[i];
            bool       skipNeighbor = false;
            for (uint16_t j = 0; j < nodeListCount; j++)
            {
                if (nodeArray[j].position.x == neighborPos.x && nodeArray[j].position.y == neighborPos.y)
                {
                    skipNeighbor = true;
                    break;
                }
            }
            if (skipNeighbor)
            {
                continue;
            }
            uint16_t hCost                    = 0;
            nodeArray[nodeListCount].position = neighborPos;
            nodeArray[nodeListCount].gCost    = currentNode->gCost + 1;
            hFunc(neighborPos, hCost);
            nodeArray[nodeListCount].hCost = hCost;
            // nodeArray[nodeListCount].hCost    = Utils_ManhattanDistance(neighborPos, targetPos);
            nodeArray[nodeListCount].fCost  = nodeArray[nodeListCount].gCost + nodeArray[nodeListCount].hCost;
            nodeArray[nodeListCount].parent = currentNode;
            nodeArray[nodeListCount].closed = false;
            nodeListCount++;
            if (nodeListCount >= 256)
            {
                LOG_ERR("Open list overflow in A* pathfinding when adding neighbor");
                return NULL;
            }
        }
    }
}

Vector2Int8 AStar_GetMoveDirection(const Vector2Int& startPos, const Vector2Int& targetPos, uint16_t maxSearchArea,
                                   bool (*hFunc)(Vector2Int, uint16_t& outCost))
{
    LOG_INF("A* Pathfinding from (%d, %d) to (%d, %d)", startPos.x, startPos.y, targetPos.x, targetPos.y);
    AStar_Node  nodeList[maxSearchArea];
    AStar_Node* lastNode = NULL;

    lastNode = AStar_CalucalatePath(nodeList, maxSearchArea, startPos, targetPos, hFunc);
    if (lastNode == NULL)
    {
        return { 0, 0 };
    }

    for (int i = 0; i < maxSearchArea; i++)
    {
        Vector3Int pos3D = { lastNode->position.x, lastNode->position.y, 0 };
        if (lastNode->parent == NULL
            || (lastNode->parent->position.x == startPos.x && lastNode->parent->position.y == startPos.y))
        {
            break;
        }
        lastNode = lastNode->parent;
    }
    Vector2Int8 direction;
    direction.x = int8_t(lastNode->position.x - startPos.x);
    direction.y = int8_t(lastNode->position.y - startPos.y);
    return direction;
}

bool AStar_IsPathAvailable(const Vector2Int& startPos, const Vector2Int& targetPos, uint16_t maxSearchArea,
                           bool (*hFunc)(Vector2Int, uint16_t& outCost))
{
    LOG_INF("A* Pathfinding from (%d, %d) to (%d, %d)", startPos.x, startPos.y, targetPos.x, targetPos.y);
    AStar_Node  nodeList[maxSearchArea];
    AStar_Node* currentNode = NULL;

    return AStar_CalucalatePath(nodeList, maxSearchArea, startPos, targetPos, hFunc) != NULL;
}

uint16_t AStar_GetPath(const Vector2Int& startPos, const Vector2Int& targetPos, uint16_t maxSearchArea,
                       Vector2Int* outPathBuffer, size_t outPathBufferSize,
                       bool (*hFunc)(Vector2Int, uint16_t& outCost))
{

    LOG_INF("A* Pathfinding from (%d, %d) to (%d, %d)", startPos.x, startPos.y, targetPos.x, targetPos.y);
    AStar_Node  nodeList[maxSearchArea];
    AStar_Node* lastNode = NULL;

    lastNode = AStar_CalucalatePath(nodeList, maxSearchArea, startPos, targetPos, hFunc);
    if (lastNode == NULL)
    {
        return 0;
    }
    // Reconstruct path
    uint16_t pathLength = 0;
    for (int i = 0; i < maxSearchArea; i++)
    {
        if (pathLength >= outPathBufferSize)
        {
            LOG_WRN("Output path buffer too small, truncating path");
            return 0;
        }
        outPathBuffer[pathLength] = lastNode->position;
        pathLength++;
        if (lastNode->parent == NULL
            || (lastNode->parent->position.x == startPos.x && lastNode->parent->position.y == startPos.y))
        {
            return pathLength;
        }
        lastNode = lastNode->parent;
    }
}
