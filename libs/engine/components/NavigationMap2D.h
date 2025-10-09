#ifndef LIBS_ENGINE_NAVIGATIONMAP2D_H
#define LIBS_ENGINE_NAVIGATIONMAP2D_H
#include "Collider2D.h"
#include "raylib.h"

#include <stdbool.h>
#include <stdint.h>
#define NAVIGATIONMAP2D_MAX_SIZE 64
#define NAVIGATIONMAP2D_MAX_PATH 64

// typedef struct NavigationMap2DPosition
// {
//     uint8_t x;
//     uint8_t y;
// } NavigationMap2DPosition;

typedef struct NavigationMap2DPath
{
    Vector2 path[NAVIGATIONMAP2D_MAX_PATH];
    uint8_t count;
} NavigationMap2DPath;

typedef struct NavigationMap2D
{
    // can make optimalization here to make 8bit hold 8 squares
    uint32_t square[NAVIGATIONMAP2D_MAX_SIZE][NAVIGATIONMAP2D_MAX_SIZE];  // 0 - free, other - entity id
    float    sNavigation2DSquareSize;
} NavigationMap2D;

void                NavigationMap2D_Initialize(NavigationMap2D* map, float squareSize);
uint32_t            NavigationMap2D_GetPosition(NavigationMap2D* map, uint8_t x, uint8_t y);
void                NavigationMap2D_SetPosition(NavigationMap2D* map, uint8_t x, uint8_t y, uint32_t state);
void                NavigationMap2D_Clear(NavigationMap2D* map);
void                NavigationMap2D_Fill(NavigationMap2D* map, Collider2D* entity);
NavigationMap2DPath NavigationMap2D_CalculatePath(NavigationMap2D* map, uint8_t startX, uint8_t startY, uint8_t stopX,
                                                  uint8_t stopY);
void                NavigationMap2D_Debug(NavigationMap2D* map);
Vector2             NavigationMap2D_ConvertWorldToMap(NavigationMap2D* map, Vector2 worldPosition);
Vector2             NavigationMap2D_ConvertMapToWorld(NavigationMap2D* map, Vector2 mapPosition);
#endif
