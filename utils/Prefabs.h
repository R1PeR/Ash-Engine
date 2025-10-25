#ifndef UTILS_SPRITEDEFINES_H
#define UTILS_SPRITEDEFINES_H
#include "Structs.h"

#include <stdint.h>

enum TextureID : uint32_t
{
    TEX_ID_EMPTY_TILE = 3,
    TEX_ID_WALL_TILE  = 180,
    TEX_ID_PLAYER     = 4,
    TEX_ID_ENEMY_RAT  = 5,
    // Add more texture IDs as needed
};

#define WORLD_MAP_SIZE  7

extern char   worldMap[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
extern Object emptyTilePrefab;
extern Object wallTilePrefab;
extern Object playerPrefab;
extern Object enemyRatPrefab;

#endif
