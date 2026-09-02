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

int16_t Input_GetMouseX(void)
{
    return Platform_GetMouseX();
}

int16_t Input_GetMouseY(void)
{
    return Platform_GetMouseY();
}

int16_t Input_GetMouseDeltaX(void)
{
    return Platform_GetMouseDeltaX();
}

int16_t Input_GetMouseDeltaY(void)
{
    return Platform_GetMouseDeltaY();
}

Camera2D camera;

void Window_Init(uint16_t width, uint16_t height, const char* title)
{
    Platform_InitWindow(width, height, title);

    camera.offset   = (Vector2){ width / 2.0f, height / 2.0f };
    camera.target   = (Vector2){ 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom     = 1.0f;

    Platform_SetTargetFPS(60);
    Platform_ImGuiSetup(true);
}

void Window_Deinit()
{
    Platform_CloseWindow();
}

Camera2D* Window_GetCamera()
{
    return &camera;
}

uint32_t Window_GetWidth()
{
    return Platform_GetScreenWidth();
}

uint32_t Window_GetHeight()
{
    return Platform_GetScreenHeight();
}
