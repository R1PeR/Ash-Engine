#ifndef UTILS_SPRITEDEFINES_H
#define UTILS_SPRITEDEFINES_H
#include "Structs.h"

#include <stdint.h>

#define WORLD_MAP_SIZE  7

enum TextureID : uint32_t
{
    TEX_ID_EMPTY_TILE = 3,
    TEX_ID_WALL_TILE  = 180,
    TEX_ID_PLAYER     = 4,
    TEX_ID_ENEMY_RAT  = 5,
    TEX_ID_ITEM_SWORD = 47,
    // Add more texture IDs as needed
};

extern char   worldMap[WORLD_MAP_SIZE][WORLD_MAP_SIZE];
extern Object emptyTilePrefab;
extern Object wallTilePrefab;
extern Object playerPrefab;
extern Object enemyRatPrefab;
extern Object itemSwordPrefab;

#endif
