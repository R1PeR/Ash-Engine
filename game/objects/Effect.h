#ifndef LIBS_PREFABS_GAMEOBJECT_PROJECTILE_H
#define LIBS_PREFABS_GAMEOBJECT_PROJECTILE_H
#include "engine/prefabs/GameObject.h"

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
    Vector2     direction;
    GameObject* parent;
    GameObject* target;
    uint32_t    flags;
} ProjectileData;

typedef struct Projectile
{
    GameObject     gameObject;
    ProjectileData data;
} Projectile;

#endif
