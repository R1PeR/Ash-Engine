#include "Entity2D.h"
#include "engine/misc/Logger.h"

uint32_t sEntityCount = 0;
Entity2D * sEntities2D[ENTITY2D_MAX_COUNT];

void Entity2D_Initialize(Entity2D * ent)
{
    ent->id = 0;
    ent->position.x = 0;
    ent->position.y = 0;
    ent->rotation = 0;
    ent->scale = 1.0f;
}

bool Entity2D_Add(Entity2D * ent)
{
    if(sEntityCount < ENTITY2D_MAX_COUNT)
    {
        ent->id = sEntityCount;
        sEntities2D[sEntityCount] = ent;
        sEntityCount++;
        return true;
    }
    else
    {
        LOG_INF("Entity2D: AddEntity2D() failed, not enough space");
        return false;
    }
}

bool Entity2D_Clear()
{
    sEntityCount = 0;
    for(int i = 0; i < ENTITY2D_MAX_COUNT; i++)
    {
        sEntities2D[i] = 0;
    }
    return true;
}

uint8_t Entitiy2D_GetCount()
{
    return sEntityCount;
}

Entity2D ** Entitiy2D_GetEntities()
{
    return sEntities2D;
}