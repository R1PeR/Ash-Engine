#ifndef LIBS_ENGINE_UTILS_H
#define LIBS_ENGINE_UTILS_H
#include "raylib.h"

#include <stdint.h>

typedef struct Vector2Int
{
    int32_t x;
    int32_t y;
} Vector2Int;

typedef struct Vector3Int
{
    int32_t x;
    int32_t y;
    int32_t z;
} Vector3Int;

typedef struct Vector2Int16
{
    int16_t x;
    int16_t y;
} Vector2Int16;

typedef struct Vector3Int16
{
    int16_t x;
    int16_t y;
    int16_t z;
} Vector3Int16;

typedef struct Vector2Int8
{
    int8_t x;
    int8_t y;
} Vector2Int8;

typedef struct Vector3Int8
{
    int8_t x;
    int8_t y;
    int8_t z;
} Vector3Int8;

typedef struct Vector2UInt
{
    int32_t x;
    int32_t y;
} Vector2UInt;

typedef struct Vector3UInt
{
    uint32_t x;
    uint32_t y;
    uint32_t z;
} Vector3Uuint;

typedef struct Vector2UInt16
{
    uint16_t x;
    uint16_t y;
} Vector2Uuint16;

typedef struct Vector3UInt16
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
} Vector3Uuint16;

typedef struct Vector2UInt8
{
    uint8_t x;
    uint8_t y;
} Vector2Uuint8;

typedef struct Vector3UInt8
{
    uint8_t x;
    uint8_t y;
    uint8_t z;
} Vector3Uuint8;

typedef void (*GetPositionScoreFunc)(Vector2Int8);

uint32_t Utils_AbsInt32(int32_t value);
uint16_t Utils_AbsInt16(int16_t value);

Vector2     Utils_WorldToScreen2D(Vector2 position, Camera2D camera);
Vector2     Utils_ScreenToWorld2D(Vector2 position, Camera2D camera);
Vector2     Utils_ScaleWithCamera(Vector2 value, Camera2D camera);
Vector3Int  Utils_WorldToGrid(Vector2 pos, uint8_t gridSize);
Vector3Int8 Utils_WorldToChunk(Vector2 pos, uint8_t gridSize, uint8_t chunkSize);
Vector2     Utils_GridToWorld(Vector3Int pos, uint8_t gridSize);
Vector2     Utils_GridCenterToWorld(Vector3Int pos, uint8_t gridSize);
Vector3Int8 Utils_GridToChunk(Vector3Int pos, uint8_t chunkSize);
bool        Utils_IsInGridRadius(Vector2Int originalPos, Vector2Int currentPos, uint16_t radius);
float       Utils_Vector2Distance(Vector2 a, Vector2 b);
uint16_t    Utils_ManhattanDistance(Vector2Int a, Vector2Int b);

#endif
