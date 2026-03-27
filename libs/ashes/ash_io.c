#include "ash_io.h"

#include "raylib.h"
#include "rlImGui.h"

bool Input_IsKeyPressed(uint16_t key)
{
    return IsKeyPressed(key);
}

bool Input_IsKeyDown(uint16_t key)
{
    return IsKeyDown(key);
}

bool Input_IsKeyReleased(uint16_t key)
{
    return IsKeyReleased(key);
}

bool Input_IsKeyUp(uint16_t key)
{
    return IsKeyUp(key);
}

bool Input_IsMouseButtonPressed(uint16_t button)
{
    return IsMouseButtonPressed(button);
}

bool Input_IsMouseButtonDown(uint16_t button)
{
    return IsMouseButtonDown(button);
}

bool Input_IsMouseButtonReleased(uint16_t button)
{
    return IsMouseButtonReleased(button);
}

bool Input_IsMouseButtonUp(uint16_t button)
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

Camera2D camera;

void Window_Init(uint16_t width, uint16_t height, const char* title)
{
    InitWindow(width, height, title);

    camera.offset   = (Vector2){ width / 2.0f, height / 2.0f };
    camera.target   = (Vector2){ 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom     = 1.0f;

    SetTargetFPS(60);
    rlImGuiSetup(true);
}

void Window_Deinit()
{
    CloseWindow();
}

Camera2D* Window_GetCamera()
{
    return &camera;
}

uint32_t Window_GetWidth()
{
    return GetScreenWidth();
}

uint32_t Window_GetHeight()
{
    return GetScreenHeight();
}
