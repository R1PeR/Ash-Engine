#ifndef GAME_OBJECTS_ENTITY_H
#define GAME_OBJECTS_ENTITY_H
#include "game/GameObject.h"

typedef struct Statistics
{
    uint32_t health;
    uint32_t armor;
    uint32_t rage;
    uint32_t mana;
    uint32_t fireResistance;
    uint32_t iceResistance;
    uint32_t poisonResistance;
    uint32_t lightningResistance;
    uint32_t holyResistance;
    uint32_t magicResistance; 
    uint32_t movementSpeed;
} Statistics;

typedef enum EntityFlags
{
    flagEntityAlive = 0,
    flagEntityBurning,
    flagEntityFrozen,
    flagEntityBleeding,
    flagEntityPoisoned,
    flagEntityStunned,
    flagEntityShocked,
} EntityFlags;

typedef enum EntityType
{
    typeEntityWorker,
    typeEntityMelee,
    typeEntityRanged,
} EntityType;

typedef enum EntityState
{
    stateIdle,
    stateMoveTo,
    stateAttack,
} EntityState;

typedef struct EntityData
{
    EntityType type;
    Statistics stats;
    uint32_t state;
    uint8_t team;
    uint32_t flags;
    Vector2 moveDest;
} EntityData;

typedef struct Entity
{
    GameObject gameObject;
    EntityData data;
} Entity;

void Entity_Initialize(Entity * unit);
void Entity_Update(Entity * unit);
void Entity_MoveTo(Entity * unit, Vector2 destintation);
void Entity_Attack(Entity * unit, GameObject target);
void Entity_Gather(Entity * unit, GameObject target);

#endif
