#include "Prefabs.h"

#include "Structs.h"

char worldMap[WORLD_MAP_SIZE][WORLD_MAP_SIZE] =
{
    {'1', '1', '1', '1', '1', '1', '0'},
    {'1', '0', '0', '0', '0', '1', '0'},
    {'1', '0', '0', '0', 'r', '1', '0'},
    {'1', '0', '0', '0', '0', '1', '0'},
    {'1', '0', 'p', '0', '0', '1', '0'},
    {'1', '1', '1', '1', '1', '1', '0'},
    {'0', '0', '0', '0', '0', '0', '0'}
};

Object emptyTilePrefab = {
    .id           = 0,
    .type         = Type::TILE,
    .layer        = 0,
    .textureId    = TEX_ID_EMPTY_TILE,
    .isCollidable = false,
    .position     = { 0, 0, 0 },
};

Object wallTilePrefab = {
    .id           = 0,
    .type         = Type::TILE,
    .layer        = 0,
    .textureId    = TEX_ID_WALL_TILE,
    .isCollidable = true,
    .position     = { 0, 0, 0 },
};

Object playerPrefab = {
    .id           = 0,
    .type         = Type::ENTITY,
    .layer        = 0,
    .textureId    = TEX_ID_PLAYER,
    .isCollidable = true,
    .position     = { 0, 0, 0 },
    .entity       = {
        .entityType            = EntityType::PLAYER,
        .entityHealth          = 100,
        .entityExperience      = 0,
        .entityLevel           = 1,
        .entitySpeed           = 100,
        .entityDamage          = 10,
        .entityAttackSpeed     = 1000,
        .entityArmor           = 0,
        .entityStrength        = 10,
        .entityDexterity       = 10,
        .entityVitality        = 10,
        .entityEnergy          = 10,
        .entityItems           = { 0 },
        .entityMovementDirection = { 0, 0 },
    },
};

Object enemyRatPrefab = {
    .id           = 0,
    .type         = Type::ENTITY,
    .layer        = 0,
    .textureId    = TEX_ID_ENEMY_RAT,
    .isCollidable = true,
    .position     = { 0, 0, 0 },
    .entity       = {
        .entityType            = EntityType::ENEMY,
        .entityHealth          = 50,
        .entityExperience      = 10,
        .entityLevel           = 1,
        .entitySpeed           = 80,
        .entityDamage          = 5,
        .entityAttackSpeed     = 1500,
        .entityArmor           = 0,
        .entityStrength        = 5,
        .entityDexterity       = 5,
        .entityVitality        = 5,
        .entityEnergy          = 5,
        .entityItems           = { 0 },
        .entityState             = EntityState::PATROLLING,
        .entityOriginalPosition  = { 0, 0 },
        .entityPatrolRadius      = 4,
        .entityMovementDirection = { 0, 0 },
    },
};
