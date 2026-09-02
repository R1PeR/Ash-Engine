#include "ash_platform.h"
#include "rlImGui.h"

#include <cstdarg>
#include <cstdint>
#include <cstdio>

void Platform_InitWindow(uint16_t width, uint16_t height, const char* title)
{
    InitWindow(width, height, title);
}

void Platform_CloseWindow()
{
    CloseWindow();
}

void Platform_SetTargetFPS(uint32_t fps)
{
    SetTargetFPS(fps);
}

uint32_t Platform_GetScreenWidth()
{
    return (uint32_t)GetScreenWidth();
}

uint32_t Platform_GetScreenHeight()
{
    return (uint32_t)GetScreenHeight();
}

bool Platform_WindowShouldClose()
{
    return WindowShouldClose();
}

void Platform_BeginDrawing()
{
    BeginDrawing();
}

void Platform_EndDrawing()
{
    EndDrawing();
}

void Platform_ClearBackground(Color color)
{
    ClearBackground(color);
}

void Platform_BeginMode2D(Camera2D camera)
{
    BeginMode2D(camera);
}

void Platform_EndMode2D()
{
    EndMode2D();
}

void Platform_DrawRectangleRec(Rectangle rect, Color color)
{
    DrawRectangleRec(rect, color);
}

void Platform_DrawRectangleLinesEx(Rectangle rect, float thickness, Color color)
{
    DrawRectangleLinesEx(rect, thickness, color);
}

void Platform_DrawRectangleLines(int x, int y, int w, int h, Color color)
{
    DrawRectangleLines(x, y, w, h, color);
}

void Platform_DrawLineEx(Vector2 start, Vector2 end, float thickness, Color color)
{
    DrawLineEx(start, end, thickness, color);
}

void Platform_DrawCircleV(Vector2 center, float radius, Color color)
{
    DrawCircleV(center, radius, color);
}

void Platform_DrawRing(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle,
                       int segments, Color color)
{
    DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, segments, color);
}

Texture2D Platform_LoadTexture(const char* fileName)
{
    return LoadTexture(fileName);
}

void Platform_UnloadTexture(Texture2D texture)
{
    UnloadTexture(texture);
}

void Platform_DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation,
                             Color tint)
{
    DrawTexturePro(texture, source, dest, origin, rotation, tint);
}

bool Platform_InitAudioDevice()
{
    InitAudioDevice();
    return IsAudioDeviceReady();
}

void Platform_CloseAudioDevice()
{
    CloseAudioDevice();
}

Sound Platform_LoadSound(const char* fileName)
{
    return LoadSound(fileName);
}

void Platform_UnloadSound(Sound sound)
{
    UnloadSound(sound);
}

void Platform_PlaySound(Sound sound)
{
    PlaySound(sound);
}

void Platform_StopSound(Sound sound)
{
    StopSound(sound);
}

bool Platform_IsKeyPressed(uint16_t key)
{
    return IsKeyPressed(key);
}

bool Platform_IsKeyDown(uint16_t key)
{
    return IsKeyDown(key);
}

bool Platform_IsKeyReleased(uint16_t key)
{
    return IsKeyReleased(key);
}

bool Platform_IsKeyUp(uint16_t key)
{
    return IsKeyUp(key);
}

bool Platform_IsMouseButtonPressed(uint16_t button)
{
    return IsMouseButtonPressed(button);
}

bool Platform_IsMouseButtonDown(uint16_t button)
{
    return IsMouseButtonDown(button);
}

bool Platform_IsMouseButtonReleased(uint16_t button)
{
    return IsMouseButtonReleased(button);
}

bool Platform_IsMouseButtonUp(uint16_t button)
{
    return IsMouseButtonUp(button);
}

int16_t Platform_GetMouseX()
{
    return (int16_t)GetMouseX();
}

int16_t Platform_GetMouseY()
{
    return (int16_t)GetMouseY();
}

int16_t Platform_GetMouseDeltaX()
{
    return (int16_t)GetMouseDelta().x;
}

int16_t Platform_GetMouseDeltaY()
{
    return (int16_t)GetMouseDelta().y;
}

float Platform_GetMouseWheelMove()
{
    return GetMouseWheelMove();
}

Vector2 Platform_GetWorldToScreen2D(Vector2 position, Camera2D camera)
{
    return GetWorldToScreen2D(position, camera);
}

Vector2 Platform_GetScreenToWorld2D(Vector2 position, Camera2D camera)
{
    return GetScreenToWorld2D(position, camera);
}

void Platform_TraceLog(int logType, const char* text, ...)
{
    va_list args;
    va_start(args, text);
    /* Raylib has no public TraceLogVa, so use the callback-style function via SetTraceLogCallback would be complex.
       Instead we format into a buffer and forward through TraceLog. */
    char buffer[2048];
    vsnprintf(buffer, sizeof(buffer), text, args);
    va_end(args);
    TraceLog(logType, "%s", buffer);
}

void Platform_SetTraceLogLevel(int logType)
{
    SetTraceLogLevel(logType);
}

int Platform_GetFPS()
{
    return GetFPS();
}

void Platform_ImGuiSetup(bool dark)
{
    rlImGuiSetup(dark);
}

void Platform_ImGuiBegin()
{
    rlImGuiBegin();
}

void Platform_ImGuiEnd()
{
    rlImGuiEnd();
}
