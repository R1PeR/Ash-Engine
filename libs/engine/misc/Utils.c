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
