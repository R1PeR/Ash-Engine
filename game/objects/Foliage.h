#ifndef LIBS_PREFABS_GAMEOBJECT_FOLIAGE_H
#define LIBS_PREFABS_GAMEOBJECT_FOLIAGE_H
#include "engine/prefabs/GameObject.h"

typedef enum FoliageFlags
{
    flagFoliageCanBeBurned = 0,
    flagFoliageCanBeFrozen,
    flagFoliageCanBeBroken,
    flagFoliageIsTree,
} FoliageFlags;

typedef struct FoliageData
{
    uint32_t flags;
} FoliageData;

typedef struct Foliage
{
    GameObject  gameObject;
    FoliageData data;
} Foliage;

#endif
