#include "Input.h"
#include "raylib.h"

bool Input_IsKeyPressed(uint8_t key)
{
    return IsKeyPressed(key);
}

bool Input_IsKeyDown(uint8_t key)
{
    return IsKeyDown(key);
}

bool Input_IsKeyReleased(uint8_t key)
{
    return IsKeyReleased(key);
}

bool Input_IsKeyUp(uint8_t key)
{
    return IsKeyUp(key);
}

bool Input_IsMouseButtonPressed(uint8_t button)
{
    return IsMouseButtonPressed(button);
}

bool Input_IsMouseButtonDown(uint8_t button)
{
    return IsMouseButtonDown(button);
}

bool Input_IsMouseButtonReleased(uint8_t button)
{
    return IsMouseButtonReleased(button);
}

bool Input_IsMouseButtonUp(uint8_t button)
{
    return IsMouseButtonUp(button);
}

int16_t Input_GetMouseX(void)
{
    return GetMouseX();
}

int16_t Input_GetMouseY(void)
{
    return GetMouseY();
}

int16_t Input_GetMouseDeltaX(void)
{
    return GetMouseDelta().x;
}

int16_t Input_GetMouseDeltaY(void)
{
    return GetMouseDelta().y;
}