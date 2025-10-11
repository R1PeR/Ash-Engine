#include "Entity2D.h"

#include "engine/misc/Logger.h"
uint32_t  sEntityCount = 0;
Entity2D* sEntityList  = NULL;

void Entity2D_Initialize(Entity2D* ent)
{
    ent->id         = 0;
    ent->position.x = 0;
    ent->position.y = 0;
    ent->rotation   = 0;
    ent->scale      = 1.0f;
    ent->next       = NULL;
}

bool Entity2D_Add(Entity2D* ent)
{
    if (sEntityCount == 0)
    {
        sEntityList       = ent;
        sEntityList->next = NULL;
    }
    else
    {
        Entity2D* current = sEntityList;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = ent;
        ent->next     = NULL;
    }
    sEntityCount++;
    return true;
}

bool Entity2D_Clear()
{
    sEntityCount = 0;
    sEntityList  = NULL;
    return true;
}

uint32_t Entitiy2D_GetCount()
{
    return sEntityCount;
}

Entity2D* Entitiy2D_GetEntityList()
{
    return sEntityList;
}
