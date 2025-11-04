#ifndef UTILS_STRUCTS_H
#define UTILS_STRUCTS_H
#include "engine/components/Sprite.h"
#include "engine/misc/Stopwatch.h"
#include "engine/misc/Utils.h"

#include <stdint.h>

#define MAX_LAYERS        8
#define CHUNK_SIZE        16
#define CHUNK_MAX_OBJECTS 256
#define TEXTURE_SCALE     4
#define TEXTURE_SIZE      8
#define SPRITE_MAX_COUNT  256

enum Type : uint8_t
{
    TILE = 0,
    ENTITY,
    PROJECTILE,
    EFFECT,
    INTERACTIVE,
    ITEM,
};
enum EntityType : uint8_t
{
    PLAYER = 0,
    ENEMY,
};
enum InteractiveType : uint8_t
{
    CHEST = 0,
    DOOR,
    STAIRS_UP,
    STAIRS_DOWN,
};

enum EntityState : uint8_t
{
    PATROLLING = 0,
    CHASING,
    GOING_BACK,
};

struct Chunk;
struct Object
{
    uint16_t   id;
    uint8_t    type;
    uint16_t   layer;
    uint32_t   textureId;
    bool       isCollidable;
    Vector3Int position;  // global world position
    union
    {
        struct
        {
            // for PLAYER, ENEMY
            uint8_t  entityType;
            uint16_t entityHealth;
            uint32_t entityExperience;
            uint8_t  entityLevel;
            uint16_t entitySpeed;
            uint32_t entityDamage;
            uint16_t entityAttackSpeed;
            uint16_t entityArmor;
            uint16_t entityStrength;
            uint16_t entityDexterity;
            uint16_t entityVitality;
            uint16_t entityEnergy;
            uint16_t entityItems[8];

            EntityState entityState;
            Vector2Int  entityOriginalPosition;
            uint16_t    entityPatrolRadius;

            Vector2Int8 entityMovementDirection;
            Stopwatch   entityMovementTimer;
            Stopwatch   entityAttackTimer;
            Vector2Int  entityCurrentPath[8];

            Object* entityTarget;
        } entity;
        struct
        {
            // for PROJECTILE
            float   projectileSpeed;
            Vector2 projectileDestination;
        } projectile;
        struct
        {
            // for EFFECT
            uint8_t   effectType;
            uint16_t  effectDuration;
            Stopwatch effectTimer;
        } effect;
        struct
        {
            // for INTERACTIVE
            uint8_t interactiveType;
            uint8_t state;
        } interactive;
        struct
        {
            // for ITEM
            uint8_t itemId;
        } item;
    };
};

struct Chunk
{
    Vector3Int8 chunkPosition;
    Object      objects[CHUNK_MAX_OBJECTS];
    uint16_t    objectCount;
};

struct GameData
{
    Sprite   sprites[SPRITE_MAX_COUNT];
    Chunk    chunks[CHUNK_SIZE * CHUNK_SIZE];
    uint16_t chunkCount;
    uint16_t spriteCount;
    int      currentZPos;
};

struct Objects
{
    Object   objects[MAX_LAYERS];
    uint16_t count;
};

struct Chunks
{
    Chunk    chunks[CHUNK_SIZE * CHUNK_SIZE];
    uint16_t count;
};

struct DebugData
{
    bool       isVisible;
    bool       isGridVisible;
    Texture2D* selectedTextureId;
    Object     currentObject;
    int        currentPrefab;
};
#endif  // UTILS_STRUCTS_H
