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

enum Type : uint16_t
{
    TILE = 0,
    ENTITY,
    PROJECTILE,
    EFFECT,
    INTERACTIVE,
    ITEM,
};
enum EntityType : uint16_t
{
    PLAYER = 0,
    ENEMY,
};
enum InteractiveType : uint16_t
{
    CHEST = 0,
    DOOR,
    STAIRS_UP,
    STAIRS_DOWN,
};

struct Chunk;
struct Object
{
    int        id;
    int        type;
    int        layer;
    uint32_t   textureId;
    bool       isCollidable;
    Vector3Int position;  // global world position
    union
    {
        struct
        {
            // for PLAYER, ENEMY
            int entityType;
            int entityHealth;
            int entityExperience;
            int entityLevel;
            int entitySpeed;
            int entityDamage;
            int entityAttackSpeed;
            int entityArmor;
            int entityStrength;
            int entityDexterity;
            int entityVitality;
            int entityEnergy;
            int entityItems[8];

            Stopwatch   entityMovementTimer;
            Vector2Int8 entityMovementDirection;
            Stopwatch   entityAttackTimer;
        } entity;
        struct
        {
            // for PROJECTILE
            float   projectileSpeed;
            Vector2 projectileDirection;  // in degrees
            int     projectileRange;
            int     projectileDistanceTraveled;
            int     projectileDamage;
        } projectile;
        struct
        {
            // for EFFECT
            int   effectType;
            float effectDuration;
            float effectTimeElapsed;
        } effect;
        struct
        {
            // for INTERACTIVE
            int  interactiveType;
            bool isOpen;  // for CHEST, DOOR
        } interactive;
        struct
        {
            // for ITEM
            int itemId;
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
};
#endif  // UTILS_STRUCTS_H
