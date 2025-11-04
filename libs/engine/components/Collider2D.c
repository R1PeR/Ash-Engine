#include "Collider2D.h"

#include "engine/misc/Logger.h"
#include <stddef.h>

Updatable   collider2DUpdatable = { Collider2D_Update };
uint32_t    sCollider2DCount    = 0;
Collider2D* sCollider2DList     = NULL;

void Collider2D_Initialize(Collider2D* col)
{
    col->parent                   = 0;
    col->position.x               = 0;
    col->position.y               = 0;
    col->size.x                   = 0;
    col->size.y                   = 0;
    col->isEnabled                = true;
    col->id                       = 0;
    col->collision.collisionCount = 0;
    col->next                     = NULL;
}

bool Collider2D_Add(Collider2D* col)
{
    if (sCollider2DCount == 0)
    {
        sCollider2DList       = col;
        sCollider2DList->next = NULL;
    }
    else
    {
        Collider2D* current = sCollider2DList;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = col;
        col->next     = NULL;
    }
    sCollider2DCount++;
    return true;
}

bool Collider2D_Clear()
{
    sCollider2DCount = 0;
    sCollider2DList  = NULL;
    return true;
}

void Collider2D_Update()
{

    Collider2D* current = sCollider2DList;
    while (current != NULL)
    {
        current->collision.collisionCount = 0;
        current                           = current->next;
    }


    Collider2D* currentA = sCollider2DList;
    while (currentA != NULL)
    {
#ifdef DEBUG
        Collider2D_DrawDebug(currentA);
#endif
        Collider2D* currentB = sCollider2DList;
        while (currentB != NULL)
        {
            if (currentA != currentB)
            {
                if (currentA->isEnabled && currentB->isEnabled)
                {
                    if (Collider2D_CheckCollider(currentA, currentB))
                    {
                        if (currentA->collision.collisionCount < COLLIDER2D_MAX_COLLISIONS
                            && currentB->collision.collisionCount < COLLIDER2D_MAX_COLLISIONS)
                        {
                            currentA->collision.collision[currentA->collision.collisionCount++] = currentB;
                            currentB->collision.collision[currentB->collision.collisionCount++] = currentA;
                            LOG_DBG("Collider2D: UpdateCollider(), collider %d collided with %d", currentA->id,
                                    currentB->id);
                        }
                        else
                        {
                            LOG_ERR("Collider2D: UpdateCollider(), collider %d or %d exceeded maximal collision count",
                                    currentA->id, currentB->id);
                        }
                    }
                    // Vector2 aPos = {0,0};
                    // Vector2 bPos = {0,0};
                    // Vector2 aSize = currentA->size;
                    // Vector2 bSize = currentB->size;
                    // if(currentA->parent)
                    // {
                    //     aPos = currentA->parent->position;
                    // }
                    // if(currentB->parent)
                    // {
                    //     bPos = currentB->parent->position;
                    // }
                    // aPos.x += currentA->position.x;
                    // aPos.y += currentA->position.y;
                    // bPos.x += currentB->position.x;
                    // bPos.y += currentB->position.y;
                    // if( aPos.x - (aSize.x/2.0f) < bPos.x + (bSize.x/2.0f) &&
                    //     aPos.x + (aSize.x/2.0f) > bPos.x - (bSize.x/2.0f) &&
                    //     aPos.y - (aSize.y/2.0f) < bPos.y + (bSize.y/2.0f) &&
                    //     aPos.y + (aSize.y/2.0f) > bPos.y - (bSize.y/2.0f))
                    // {
                    //     if(currentA->collision.collisionCount < COLLIDER2D_MAX_COLLISIONS &&
                    //     currentB->collision.collisionCount < COLLIDER2D_MAX_COLLISIONS)
                    //     {
                    //         currentA->collision.collision[currentA->collision.collisionCount++] = currentB;
                    //         currentB->collision.collision[currentB->collision.collisionCount++] = currentA;
                    //         LOG_DBG("Collider2D: UpdateCollider(), collider %d collided with %d", currentA->id,
                    //         currentB->id);
                    //     }
                    //     else
                    //     {
                    //         LOG_ERR("Collider2D: UpdateCollider(), collider %d or %d exceeded maximal collision
                    //         count", currentA->id, currentB->id);
                    //     }

                    // }
                }
            }
            currentB = currentB->next;
        }
        currentA = currentA->next;
    }
}

void Collider2D_DrawDebug(Collider2D* col)
{
    if (col == NULL)
    {
        LOG_ERR("Collider2D: DrawDebug(), collider is nullptr");
        return;
    }
    if (col->parent == NULL)
    {
        DrawRectangleLines(col->parent->position.x + col->position.x, col->parent->position.y + col->position.y,
                           col->size.x * col->parent->scale, col->size.y, YELLOW);
    }
    else
    {
        DrawRectangleLines(col->parent->position.x + (col->position.x * col->parent->scale),
                           col->parent->position.y + (col->position.y * col->parent->scale),
                           col->size.x * col->parent->scale, col->size.y * col->parent->scale, YELLOW);
    }
}

bool Collider2D_Check(Collider2D* a, Collider2D* b)
{
    if (a == NULL || b == NULL)
    {
        LOG_ERR("Collider2D: CheckCollision(), collider A or B is nullptr");
        return false;
    }
    if (a->collision.collisionCount > 0)
    {
        for (uint32_t i = 0; i < a->collision.collisionCount; i++)
        {
            if (a->collision.collision[i] == b)
            {
                return true;
            }
        }
        return false;
    }
    return false;
}

bool Collider2D_CheckCollider(Collider2D* a, Collider2D* b)
{
    Vector2 aPos  = { 0, 0 };
    Vector2 bPos  = { 0, 0 };
    Vector2 aSize = a->size;
    Vector2 bSize = b->size;
    if (a->parent)
    {
        aPos = { a->parent->position.x + (a->position.x * a->parent->scale),
                 a->parent->position.y + (a->position.y * a->parent->scale) };
    }
    else
    {
        aPos = a->position;
    }
    if (b->parent)
    {
        bPos = { b->parent->position.x + (b->position.x * b->parent->scale),
                 b->parent->position.y + (b->position.y * b->parent->scale) };
    }
    else
    {
        bPos = b->position;
    }
    if (aPos.x < bPos.x + bSize.x && aPos.x + aSize.x > bPos.x && aPos.y < bPos.y + bSize.y
        && aPos.y + aSize.y > bPos.y)
    {
        return true;
    }
    return false;
}

bool Collider2D_CheckPoint(Collider2D* a, Vector2 b)
{
    Vector2 aPos  = { 0, 0 };
    Vector2 aSize = a->size;
    if (a->parent)
    {
        aPos = a->parent->position;
    }
    aPos.x += a->position.x;
    aPos.y += a->position.y;
    if (aPos.x < b.x && aPos.x + aSize.x > b.x && aPos.y < b.y && aPos.y + aSize.y > b.y)
    {
        return true;
    }
    return false;
}

bool Collider2D_CheckRect(Collider2D* a, Rectangle b)
{
    Vector2 aPos  = { 0, 0 };
    Vector2 bPos  = { b.x, b.y };
    Vector2 aSize = a->size;
    Vector2 bSize = { b.width, b.height };
    if (a->parent)
    {
        aPos = a->parent->position;
    }
    aPos.x += a->position.x;
    aPos.y += a->position.y;
    if (aPos.x < bPos.x + bSize.x && aPos.x + aSize.x > bPos.x && aPos.y < bPos.y + bSize.y
        && aPos.y + aSize.y > bPos.y)
    {
        return true;
    }
    return false;
}

uint32_t Collider2D_GetCount()
{
    return sCollider2DCount;
}

Collider2D* Collider2D_GetCollider2DList()
{
    return sCollider2DList;
}

Updatable* Collider2D_GetUpdatable()
{
    return &collider2DUpdatable;
}
