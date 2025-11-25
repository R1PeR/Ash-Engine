#include "AStar.h"

#include "Logger.h"
// #include "raylib.h"
// #include "utils/Structs.h"

AStar_Node* AStar_CalucalatePath(AStar_Node* nodeArray, size_t nodeArraySize, const Vector2Int startPos, const Vector2Int targetPos,
                                 HeuristicFuncPtr hFunc)
{
    LOG_INF("A* Pathfinding from (%d, %d) to (%d, %d)", startPos.x, startPos.y, targetPos.x, targetPos.y);
    uint16_t nodeListCount = 0;

    AStar_Node* currentNode = NULL;
    // Initialize start node
    AStar_Node startNode;
    startNode.position = startPos;
    startNode.gCost    = 0;
    hFunc(startPos, targetPos, startNode.hCost);
    // startNode.hCost            =  ? UINT16_MAX : startNode.hCost;
    startNode.fCost            = startNode.gCost + startNode.hCost;
    startNode.parent           = NULL;
    startNode.closed           = false;
    startNode.valid            = true;
    nodeArray[nodeListCount++] = startNode;
    while (nodeListCount < nodeArraySize)
    {
        // Find node with lowest fCost
        uint16_t lowerstFCost = UINT16_MAX;
        uint16_t lowestIndex  = 0;
        bool     anyOpen      = false;
        for (uint16_t i = 0; i < nodeListCount; i++)
        {
            if (nodeArray[i].closed == true)
            {
                continue;
            }
            anyOpen = true;
            if (nodeArray[i].fCost < lowerstFCost)
            {
                lowerstFCost = nodeArray[i].fCost;
                lowestIndex  = i;
            }
        }
        if (!anyOpen)
        {
// #if 1
//             for (uint16_t i = 0; i < nodeListCount; i++)
//             {
//                 Vector3Int pos3D    = { nodeArray[i].position.x, nodeArray[i].position.y, 0 };
//                 Vector2    worldPos = Utils_GridToWorld(pos3D, TEXTURE_SIZE * TEXTURE_SCALE);
//                 DrawRectangleLines(int(worldPos.x), int(worldPos.y), int(TEXTURE_SIZE * TEXTURE_SCALE),
//                                    int(TEXTURE_SIZE * TEXTURE_SCALE), BLUE);
//             }
// #endif
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
        currentNode->closed = true;
        if (currentNode->valid == false)
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
            nodeArray[nodeListCount].closed   = false;
            nodeArray[nodeListCount].valid    = hFunc(neighborPos, targetPos, nodeArray[nodeListCount].hCost);
            nodeArray[nodeListCount].position = neighborPos;
            nodeArray[nodeListCount].gCost    = currentNode->gCost + 1;
            nodeArray[nodeListCount].fCost    = nodeArray[nodeListCount].gCost + nodeArray[nodeListCount].hCost;
            nodeArray[nodeListCount].parent   = currentNode;
            nodeArray[nodeListCount].closed   = false;
            nodeListCount++;
            if (nodeListCount >= 256)
            {
                LOG_ERR("Open list overflow in A* pathfinding when adding neighbor");
                return NULL;
            }
        }
    }
    return NULL;
}

Vector2Int8 AStar_GetMoveDirection(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea,
                                   HeuristicFuncPtr hFunc)
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
// #if 1
//         Vector3Int pos3D    = { lastNode->position.x, lastNode->position.y, 0 };
//         Vector2    worldPos = Utils_GridToWorld(pos3D, TEXTURE_SIZE * TEXTURE_SCALE);
//         DrawRectangleLines(int(worldPos.x), int(worldPos.y), int(TEXTURE_SIZE * TEXTURE_SCALE),
//                            int(TEXTURE_SIZE * TEXTURE_SCALE), GREEN);
// #endif
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

bool AStar_IsPathAvailable(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea, HeuristicFuncPtr hFunc)
{
    LOG_INF("A* Pathfinding from (%d, %d) to (%d, %d)", startPos.x, startPos.y, targetPos.x, targetPos.y);
    AStar_Node  nodeList[maxSearchArea];
    AStar_Node* currentNode = NULL;

    return AStar_CalucalatePath(nodeList, maxSearchArea, startPos, targetPos, hFunc) != NULL;
}

uint16_t AStar_GetPath(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea, Vector2Int* outPathBuffer,
                       size_t outPathBufferSize, HeuristicFuncPtr hFunc)
{

    LOG_INF("A* Pathfinding from (%d, %d) to (%d, %d)", startPos.x, startPos.y, targetPos.x, targetPos.y);
    AStar_Node  nodeList[maxSearchArea];
    AStar_Node* lastNode = NULL;

    lastNode = AStar_CalucalatePath(nodeList, maxSearchArea, startPos, targetPos, hFunc);
    if (lastNode == NULL)
    {
        return 0;
    }
    // Reruct path
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
    return 0;
}
