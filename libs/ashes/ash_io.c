#include "ash_io.h"

#include "ash_platform.h"

bool Input_IsKeyPressed(uint16_t key)
{
    return Platform_IsKeyPressed(key);
}

bool Input_IsKeyDown(uint16_t key)
{
    return Platform_IsKeyDown(key);
}

bool Input_IsKeyReleased(uint16_t key)
{
    return Platform_IsKeyReleased(key);
}

bool Input_IsKeyUp(uint16_t key)
{
    return Platform_IsKeyUp(key);
}

bool Input_IsMouseButtonPressed(uint16_t button)
{
    return Platform_IsMouseButtonPressed(button);
}

bool Input_IsMouseButtonDown(uint16_t button)
{
    return Platform_IsMouseButtonDown(button);
}

bool Input_IsMouseButtonReleased(uint16_t button)
{
    return Platform_IsMouseButtonReleased(button);
}

bool Input_IsMouseButtonUp(uint16_t button)
{
    return Platform_IsMouseButtonUp(button);
}

int32_t Input_GetMouseX(void)
{
    return Platform_GetMouseX();
}

int32_t Input_GetMouseY(void)
{
    return Platform_GetMouseY();
}

int32_t Input_GetMouseDeltaX(void)
{
    return Platform_GetMouseDeltaX();
}

int32_t Input_GetMouseDeltaY(void)
{
    return Platform_GetMouseDeltaY();
}

void Window_Init(Window* window, uint16_t width, uint16_t height, const char* title)
{
    Platform_InitWindow(width, height, title);

    window->width           = width;
    window->height          = height;

    Platform_SetTargetFPS(60);
    // Platform_ImGuiSetup(true);
}

void Window_Deinit()
{
    Platform_CloseWindow();
}
