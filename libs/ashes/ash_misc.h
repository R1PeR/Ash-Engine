#ifndef ASH_MISC_H
#define ASH_MISC_H

#include <raylib.h>
#include <stdint.h>

#define CLOCKS_PER_MS        CLOCKS_PER_SEC / 1000
#define Utils_ArraySize(arr) (sizeof(arr) / sizeof(arr[0]))
#define Utils_AddToArray(arr, value, currentSize, maxSize) \
    (((currentSize) < (maxSize)) ? ((arr)[(currentSize)++] = (value), true) : false)

/* Structs, Enums, and Unions */
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
    uint32_t x;
    uint32_t y;
} Vector2UInt;
typedef struct Vector3UInt
{
    uint32_t x;
    uint32_t y;
    uint32_t z;
} Vector3UInt;
typedef struct Vector2UInt16
{
    uint16_t x;
    uint16_t y;
} Vector2UInt16;
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
} Vector2UInt8;
typedef struct Vector3UInt8
{
    uint8_t x;
    uint8_t y;
    uint8_t z;
} Vector3UInt8;
typedef struct Vector2Float
{
    float x;
    float y;
} Vector2Float;
typedef struct Vector4Float
{
    float x;
    float y;
    float w;
    float h;
} Vector4Float;
typedef struct Vector4UInt
{
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
} Vector4UInt;
typedef struct Stopwatch
{
    uint32_t startTime;
    uint32_t endTime;
} Stopwatch;
typedef struct AStar_Node
{
    Vector2Int  position;
    uint16_t    gCost;
    uint16_t    hCost;
    uint32_t    fCost;
    AStar_Node* parent;
    bool        closed;
    bool        valid;
} AStar_Node;

typedef bool (*HeuristicFuncPtr)(const Vector2Int, const Vector2Int, uint16_t& outCost);
typedef void (*GetPositionScoreFunc)(Vector2Int8);

/* Function Prototypes */

typedef bool (*HeuristicFuncPtr)(const Vector2Int, const Vector2Int, uint16_t& outCost);

Vector2Int8 AStar_GetMoveDirection(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea,
                                   HeuristicFuncPtr hFunc);

bool AStar_IsPathAvailable(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea,
                           HeuristicFuncPtr hFunc);

uint16_t AStar_GetPath(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea,
                       Vector2Int* outPathBuffer, size_t outPathBufferSize, size_t& outPathLength,
                       HeuristicFuncPtr hFunc);

void  DeltaTime_Update();
float DeltaTime_GetDeltaTime();

void Stopwatch_Start(Stopwatch* stopwatch, uint32_t milis);
void Stopwatch_Stop(Stopwatch* stopwatch);

uint32_t Stopwatch_GetElapsedTime(Stopwatch* stopwatch);
float    Stopwatch_GetPercentRemainingTime(Stopwatch* stopwatch);

uint32_t Stopwatch_GetRemainingTime(Stopwatch* stopwatch);
float    Stopwatch_GetPercentRemainingTime(Stopwatch* stopwatch);

bool Stopwatch_IsRunning(Stopwatch* stopwatch);
bool Stopwatch_IsElapsed(Stopwatch* stopwatch);
bool Stopwatch_IsZero(Stopwatch* stopwatch);

uint32_t Utils_AbsInt32(int32_t value);
uint16_t Utils_AbsInt16(int16_t value);
float    Utils_AbsFloat(float value);

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
uint16_t    Utils_Vector2DistanceInt(Vector2Int a, Vector2Int b);
uint16_t    Utils_ManhattanDistance(Vector2Int a, Vector2Int b);
int16_t     Utils_GetRandomInRange(int16_t min, int16_t max);

bool Utils_PointInRectangle(Vector2 point, Rectangle rect);
bool Utils_RectangleOverlap(Rectangle point, Rectangle rect);

#endif  // ASH_MISC_H
