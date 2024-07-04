#include "Collider2D.h"
#include "engine/misc/Logger.h"

Updatable collider2DUpdatable = {Collider2D_Update};
uint32_t sCollider2DCount = 0;
Collider2D * sColliders[COLLIDER2D_MAX_COUNT];

void Collider2D_Initialize(Collider2D * col)
{
    col->parent = 0;
    col->position.x = 0;
    col->position.y = 0;
    col->size.x = 0;
    col->size.y = 0;
    col->isEnabled = true;
    col->id = 0;
    col->collision.collisionCount = 0;
}

bool Collider2D_Add(Collider2D * col)
{
    if(sCollider2DCount < COLLIDER2D_MAX_COUNT)
    {
        col->id = sCollider2DCount;
        sColliders[sCollider2DCount] = col;
        sCollider2DCount++;
        return true;
    }
    else
    {
        LOG_INF("Collider2D: AddCollider() failed, not enough space");
        return false;
    }
}

bool Collider2D_Clear()
{
    sCollider2DCount = 0;
    for(int i = 0; i < COLLIDER2D_MAX_COUNT; i++)
    {
        sColliders[i] = 0;
    }
    return true;
}

void Collider2D_Update()
{
    for(uint32_t i = 0; i < sCollider2DCount; i++)
    {
        sColliders[i]->collision.collisionCount = 0;
    }

    for(uint32_t i = 0; i < sCollider2DCount; i++)
    {
        for(uint32_t j = 0; j < sCollider2DCount; j++)
        {
            if(i != j)
            {
                Collider2D * a = sColliders[i];
                Collider2D * b = sColliders[j];
                if(a->isEnabled && b->isEnabled)
                {
                    Vector2 aPos = {0,0};
                    Vector2 bPos = {0,0};
                    Vector2 aSize = a->size;
                    Vector2 bSize = b->size;
                    if(a->parent)
                    {
                        aPos = a->parent->position;
                    }
                    if(b->parent)
                    {
                        bPos = b->parent->position;
                    }
                    aPos.x += a->position.x;
                    aPos.y += a->position.y;
                    bPos.x += b->position.x;
                    bPos.y += b->position.y;
#ifdef COLLIDER2D_SIMPLE_CHECK
                    if( aPos.x - (aSize.x/2.0f) < bPos.x + (bSize.x/2.0f) &&
                        aPos.x + (aSize.x/2.0f) > bPos.x - (bSize.x/2.0f) &&
                        aPos.y - (aSize.y/2.0f) < bPos.y + (bSize.y/2.0f) &&
                        aPos.y + (aSize.y/2.0f) > bPos.y - (bSize.y/2.0f))
                    {
                        if(a->collision.collisionCount < COLLIDER2D_MAX_COLLISIONS && b->collision.collisionCount < COLLIDER2D_MAX_COLLISIONS)
                        {
                            a->collision.collision[a->collision.collisionCount++] = b;
                            b->collision.collision[b->collision.collisionCount++] = a;
                            LOG_DBG("Collider2D: UpdateCollider(), collider %d collided with %d", a->id, b->id);
                        }
                        else
                        {
                            LOG_ERR("Collider2D: UpdateCollider(), collider %d or %d exceeded maximal collision count", a->id, b->id);
                        }
                        
                    }
#elif
                    LOG_ERR("Collider2D: UpdateCollider(), no advanced collision detection written yet");
#endif
                }
            }
        }
    }
}

bool Collider2D_Check(Collider2D * a, Collider2D * b)
{
    if(!a || !b)
    {
        TRACELOG(LOG_ERROR, "Collider2D: CheckCollision(), collider A or B is nullptr");
        return false;
    }
    if(a->collision.collisionCount > 0)
    {
        for(uint32_t i = 0; i < a->collision.collisionCount; i++)
        {
            if(a->collision.collision[i] == b)
            {
                return true;
            }
        }
        return false;
    }
    return false;
}

uint8_t Collider2D_GetCount()
{
    return sCollider2DCount;
}

Collider2D ** Collider2D_GetColliders()
{
    return sColliders;
}

Updatable * Collider2D_GetUpdatable()
{
    return &collider2DUpdatable;
}