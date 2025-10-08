#include "Window.h"

#include "rlImGui.h"

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
