#include "Utils.h"

#include "raylib.h"

#include <math.h>

uint32_t Utils_AbsInt32(int32_t value)
{
    return (value < 0) ? -value : value;
}

uint16_t Utils_AbsInt16(int16_t value)
{
    return (value < 0) ? -value : value;
}
Vector2 Utils_WorldToScreen2D(Vector2 position, Camera2D camera)
{
    float   zoom = camera.zoom;
    Vector2 screenPosition;
    screenPosition.x = (position.x - camera.target.x) * zoom + GetScreenWidth() / 2.0f;
    screenPosition.y = (position.y - camera.target.y) * zoom + GetScreenHeight() / 2.0f;
    return screenPosition;
}

Vector2 Utils_ScreenToWorld2D(Vector2 position, Camera2D camera)
{
    Vector2 worldPosition = GetScreenToWorld2D(position, camera);
    return worldPosition;
}

Vector2 Utils_ScaleWithCamera(Vector2 value, Camera2D camera)
{
    Vector2 scaledValue;
    scaledValue.x = value.x * (1.0f / camera.zoom);
    scaledValue.y = value.y * (1.0f / camera.zoom);
    return scaledValue;
}

Vector3Int Utils_WorldToGrid(Vector2 pos, uint8_t gridSize)
{
    Vector3Int position;
    position.x = (int)(pos.x / gridSize);
    position.y = (int)(pos.y / gridSize);
    position.z = 0;
    if (pos.x < 0)
    {
        position.x -= 1;
    }
    if (pos.y < 0)
    {
        position.y -= 1;
    }
    return position;
}

Vector3Int8 Utils_WorldToChunk(Vector2 pos, uint8_t gridSize, uint8_t chunkSize)
{
    Vector3Int8 position;
    position.x = (pos.x / gridSize) / chunkSize;
    position.y = (pos.y / gridSize) / chunkSize;
    if (pos.x < 0)
    {
        position.x -= 1;
    }
    if (pos.y < 0)
    {
        position.y -= 1;
    }
    position.z = 0;
    return position;
}

Vector2 Utils_GridToWorld(Vector3Int pos, uint8_t gridSize)
{
    Vector2 position;
    position.x = pos.x * gridSize;
    position.y = pos.y * gridSize;
    return position;
}

Vector2 Utils_GridCenterToWorld(Vector3Int pos, uint8_t gridSize)
{
    Vector2 position;
    position.x = pos.x * gridSize + (gridSize / 2.0);
    position.y = pos.y * gridSize + (gridSize / 2.0);
    return position;
}

Vector3Int8 Utils_GridToChunk(Vector3Int pos, uint8_t chunkSize)
{
    Vector3Int8 chunkPos;
    chunkPos.x = pos.x / chunkSize;
    chunkPos.y = pos.y / chunkSize;
    if (pos.x < 0)
    {
        chunkPos.x -= 1;
    }
    if (pos.y < 0)
    {
        chunkPos.y -= 1;
    }
    chunkPos.z = pos.z;
    return chunkPos;
}

bool Utils_IsInGridRadius(Vector2Int center, Vector2Int point, uint16_t radius)
{
    int32_t dx = center.x - point.x;
    int32_t dy = center.y - point.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

float Utils_Vector2Distance(Vector2 a, Vector2 b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}
uint16_t    Utils_Vector2DistanceInt(Vector2Int a, Vector2Int b)
{
    int32_t dx = a.x - b.x;
    int32_t dy = a.y - b.y;
    return (uint16_t)sqrtf((float)(dx * dx + dy * dy));
}

uint16_t Utils_ManhattanDistance(Vector2Int a, Vector2Int b)
{
    return Utils_AbsInt16(a.x - b.x) + Utils_AbsInt16(a.y - b.y);
}

int16_t Utils_GerRandomInRange(int16_t min, int16_t max)
{
    return (rand() % (max - min + 1)) + min;
}
