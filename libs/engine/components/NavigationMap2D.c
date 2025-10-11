#include "NavigationMap2D.h"

#include "engine/misc/Logger.h"

#include <math.h>

void NavigationMap2D_Initialize(NavigationMap2D* map, float squareSize)
{
    for (uint8_t y = 0; y < NAVIGATIONMAP2D_MAX_SIZE; y++)
    {
        for (uint8_t x = 0; x < NAVIGATIONMAP2D_MAX_SIZE; x++)
        {
            NavigationMap2D_SetPosition(map, x, y, UINT32_MAX);
        }
    }
    map->sNavigation2DSquareSize = squareSize;
}

uint32_t NavigationMap2D_GetPosition(NavigationMap2D* map, uint8_t x, uint8_t y)
{
    if (x < NAVIGATIONMAP2D_MAX_SIZE && y < NAVIGATIONMAP2D_MAX_SIZE)
    {
        return map->square[x][y];
    }
    LOG_WRN("NavigationMap2D: GetPosition(), position {%d, %d} is bigger than max map size", x, y);
    return 0;
}

void NavigationMap2D_SetPosition(NavigationMap2D* map, uint8_t x, uint8_t y, uint32_t state)
{
    if (x < NAVIGATIONMAP2D_MAX_SIZE && y < NAVIGATIONMAP2D_MAX_SIZE)
    {
        map->square[x][y] = state;
    }
    else
    {
        LOG_WRN("NavigationMap2D: SetPosition(), position {%d, %d} is bigger than max map size", x, y);
    }
}

void NavigationMap2D_FillSurroundingWeights(NavigationMap2D* map,
                                            uint8_t weightMap[NAVIGATIONMAP2D_MAX_SIZE][NAVIGATIONMAP2D_MAX_SIZE],
                                            uint8_t currentX, uint8_t currentY, uint8_t stopX, uint8_t stopY)
{
    for (uint8_t y = 0; y < NAVIGATIONMAP2D_MAX_SIZE; y++)
    {
        for (uint8_t x = 0; x < NAVIGATIONMAP2D_MAX_SIZE; x++)
        {
            if (map->square[x][y] != UINT32_MAX)
            {
                weightMap[x][y] = UINT8_MAX;
            }
            else
            {
                weightMap[x][y] = (abs(x - stopX) + abs(y - stopY));
            }
        }
    }
    // for(uint8_t y = 0; y < 3; y++)
    // {
    //     for(uint8_t x = 0; x < 3; x++)
    //     {
    //         if(x != 1 && y != 1)
    //         {
    //             if(map->square[currentX + x - 1][currentY + y - 1] == UINT32_MAX && ((currentX + x - 1) >= 0 &&
    //             (currentY + y - 1) >= 0))
    //             {
    //                 weightMap[currentX + x - 1][currentY + y - 1] = (abs(currentX - stopX) + abs(currentY - stopY));
    //             }
    //             else
    //             {
    //                 weightMap[currentX + x - 1][currentY + y - 1] = 0;
    //             }
    //         }
    //     }
    // }
}

void NavigationMap2D_GetNextMove(NavigationMap2D* map,
                                 uint8_t          weightMap[NAVIGATIONMAP2D_MAX_SIZE][NAVIGATIONMAP2D_MAX_SIZE],
                                 uint8_t* currentX, uint8_t* currentY)
{
    uint8_t bestMoveX   = UINT8_MAX;
    uint8_t bestMoveY   = UINT8_MAX;
    uint8_t currentBest = UINT8_MAX;
    for (uint8_t y = 0; y < 3; y++)
    {
        for (uint8_t x = 0; x < 3; x++)
        {
            int8_t xPos = *currentX + x - 1;
            int8_t yPos = *currentY + y - 1;
            if (x == 1 && y == 1)
                continue;
            if (xPos < 0 || yPos < 0 || xPos > NAVIGATIONMAP2D_MAX_SIZE || yPos > NAVIGATIONMAP2D_MAX_SIZE)
                continue;
            if (weightMap[xPos][yPos] != UINT8_MAX && weightMap[xPos][yPos] < currentBest)
            {
                currentBest = weightMap[xPos][yPos];
                bestMoveX   = *currentX + x - 1;
                bestMoveY   = *currentY + y - 1;
            }
        }
    }
    if (bestMoveX != UINT8_MAX && bestMoveY != UINT8_MAX)
    {
        *currentX = bestMoveX;
        *currentY = bestMoveY;
    }
}

NavigationMap2DPath NavigationMap2D_CalculatePath(NavigationMap2D* map, uint8_t startX, uint8_t startY, uint8_t stopX,
                                                  uint8_t stopY)
{
    uint8_t weightMap[NAVIGATIONMAP2D_MAX_SIZE][NAVIGATIONMAP2D_MAX_SIZE];
    for (uint8_t y = 0; y < NAVIGATIONMAP2D_MAX_SIZE; y++)
    {
        for (uint8_t x = 0; x < NAVIGATIONMAP2D_MAX_SIZE; x++)
        {
            weightMap[x][y] = 0;
        }
    }
    uint8_t             position = 0;
    NavigationMap2DPath path;
    uint8_t             currentX = startX;
    uint8_t             currentY = startY;
    while (position < NAVIGATIONMAP2D_MAX_PATH)
    {
        NavigationMap2D_FillSurroundingWeights(map, weightMap, currentX, currentY, stopX, stopY);
        NavigationMap2D_GetNextMove(map, weightMap, &currentX, &currentY);
        path.path[position].x = currentX;
        path.path[position].y = currentY;
        position++;
        if (currentX == stopX && currentY == stopY)
        {
            break;
        }
    }
    path.count = position;
    for (int i = 0; i < position - 1; i++)
    {
        DrawLine(path.path[i].x * map->sNavigation2DSquareSize
                     - (NAVIGATIONMAP2D_MAX_SIZE / 2) * map->sNavigation2DSquareSize,
                 path.path[i].y * map->sNavigation2DSquareSize
                     - (NAVIGATIONMAP2D_MAX_SIZE / 2) * map->sNavigation2DSquareSize,
                 path.path[i + 1].x * map->sNavigation2DSquareSize
                     - (NAVIGATIONMAP2D_MAX_SIZE / 2) * map->sNavigation2DSquareSize,
                 path.path[i + 1].y * map->sNavigation2DSquareSize
                     - (NAVIGATIONMAP2D_MAX_SIZE / 2) * map->sNavigation2DSquareSize,
                 BLUE);
    }
    return path;
}

// void NavigationMap2D_FillMap(NavigationMap2D * map, Entity2D * entityList)
// {
//     for(uint8_t y = 0; y < NAVIGATIONMAP2D_MAX_SIZE; y++)
//     {
//         for(uint8_t x = 0; x < NAVIGATIONMAP2D_MAX_SIZE; x++)
//         {
//             NavigationMap2D_SetPosition(map, x, y, UINT32_MAX);
//         }
//     }
//     Entity2D * current = entityList;
//     while(current != NULL)
//     {
//         uint8_t positionX = (uint8_t)((current->position.x/sNavigation2DSquareSize)+(NAVIGATIONMAP2D_MAX_SIZE/2));
//         uint8_t positionY = (uint8_t)((current->position.y/sNavigation2DSquareSize)+(NAVIGATIONMAP2D_MAX_SIZE/2));
//         NavigationMap2D_SetPosition(map, positionX, positionY, current->id);
//         current = current->next;
//     }
// }

void NavigationMap2D_Clear(NavigationMap2D* map)
{
    for (uint8_t y = 0; y < NAVIGATIONMAP2D_MAX_SIZE; y++)
    {
        for (uint8_t x = 0; x < NAVIGATIONMAP2D_MAX_SIZE; x++)
        {
            NavigationMap2D_SetPosition(map, x, y, UINT32_MAX);
        }
    }
}

Vector2 NavigationMap2D_ConvertWorldToMap(NavigationMap2D* map, Vector2 worldPosition)
{
    Vector2 mapPosition;
    mapPosition.x = (int16_t)(worldPosition.x / (map->sNavigation2DSquareSize));
    mapPosition.y = (int16_t)(worldPosition.y / (map->sNavigation2DSquareSize));
    mapPosition.x += (NAVIGATIONMAP2D_MAX_SIZE / 2);
    mapPosition.y += (NAVIGATIONMAP2D_MAX_SIZE / 2);
    return mapPosition;
}

Vector2 NavigationMap2D_ConvertMapToWorld(NavigationMap2D* map, Vector2 mapPosition)
{
    Vector2 worldPosition;
    mapPosition.x -= (NAVIGATIONMAP2D_MAX_SIZE / 2);
    mapPosition.y -= (NAVIGATIONMAP2D_MAX_SIZE / 2);
    worldPosition.x = (mapPosition.x * (map->sNavigation2DSquareSize));
    worldPosition.y = (mapPosition.y * (map->sNavigation2DSquareSize));
    return worldPosition;
}


void NavigationMap2D_Fill(NavigationMap2D* map, Collider2D* collider)
{
    Vector2 topLeftIndex, topRightIndex, bottomLeftIndex, bottomRightIndex;
    Vector2 centerPos;
    uint8_t entityId = 0;
    centerPos.x      = collider->position.x;
    centerPos.y      = collider->position.y;
    if (collider->parent != NULL)
    {
        centerPos.x += collider->parent->position.x;
        centerPos.y += collider->parent->position.y;
        entityId = collider->parent->id;
    }
    Vector2 current;
    current.x    = centerPos.x - collider->size.x;
    current.y    = centerPos.y - collider->size.y;
    topLeftIndex = NavigationMap2D_ConvertWorldToMap(map, current);

    // those two are useless computation power when colliders are always squares
    //  current.x = centerPos.x + collider->size.x;
    //  current.y = centerPos.y - collider->size.y;
    //  topRightIndex = NavigationMap2D_ConvertWorldToMap(map, current);

    // current.x = centerPos.x - collider->size.x;
    // current.y = centerPos.y + collider->size.y;
    // bottomLeftIndex = NavigationMap2D_ConvertWorldToMap(map, current);

    current.x        = centerPos.x + collider->size.x;
    current.y        = centerPos.y + collider->size.y;
    bottomRightIndex = NavigationMap2D_ConvertWorldToMap(map, current);

    for (uint32_t y = topLeftIndex.y; y <= bottomRightIndex.y; y++)
    {
        for (uint32_t x = topLeftIndex.x; x <= bottomRightIndex.x; x++)
        {
            NavigationMap2D_SetPosition(map, x, y, entityId);
        }
    }

    // uint8_t positionX = (uint8_t)((current->position.x/sNavigation2DSquareSize)+(NAVIGATIONMAP2D_MAX_SIZE/2));
    // uint8_t positionY = (uint8_t)((current->position.y/sNavigation2DSquareSize)+(NAVIGATIONMAP2D_MAX_SIZE/2));

    // NavigationMap2D_SetPosition(map, positionX, positionY, current->id);
}

void NavigationMap2D_Debug(NavigationMap2D* map)
{
    for (uint8_t y = 0; y < NAVIGATIONMAP2D_MAX_SIZE; y++)
    {
        for (uint8_t x = 0; x < NAVIGATIONMAP2D_MAX_SIZE; x++)
        {
            if (NavigationMap2D_GetPosition(map, x, y) == UINT32_MAX)
            {
                DrawRectangleLines((x - NAVIGATIONMAP2D_MAX_SIZE / 2) * map->sNavigation2DSquareSize + 2,
                                   (y - NAVIGATIONMAP2D_MAX_SIZE / 2) * map->sNavigation2DSquareSize + 2,
                                   map->sNavigation2DSquareSize - 4, map->sNavigation2DSquareSize - 4, GREEN);
            }
            else
            {
                DrawRectangleLines((x - NAVIGATIONMAP2D_MAX_SIZE / 2) * map->sNavigation2DSquareSize + 2,
                                   (y - NAVIGATIONMAP2D_MAX_SIZE / 2) * map->sNavigation2DSquareSize + 2,
                                   map->sNavigation2DSquareSize - 4, map->sNavigation2DSquareSize - 4, RED);
            }
        }
    }
}
