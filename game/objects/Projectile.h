#ifndef GAME_PROJECTILE_H
#define GAME_PROJECTILE_H
#include "game/GameObject.h"

typedef enum ProjectileFlag
{
    flagProjectileIsGuided = 0,
    flagProjectileExplodeAfterTime,
    flagProjectileExplodeOnContact,
    flagProjectileBurn,
    flagProjectileFreeze,
    flagProjectileBleed,
    flagProjectilePoison,
    flagProjectileFreeRunning,
} ProjectileFlag;

typedef struct ProjectileData
{
    Vector2 direction;
    GameObject * parent;
    GameObject * target;
    uint32_t flags;
} ProjectileData;

typedef struct Projectile
{
    GameObject gameObject;
    ProjectileData data;
} Projectile;

#endif
