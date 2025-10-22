#include "Utils.h"
#include "raylib.h"

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
    position.x = pos.x * gridSize + (gridSize / 2);
    position.y = pos.y * gridSize + (gridSize / 2);
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
