#include "ash_debug.h"
#include "ash_misc.h"
#include "ash_platform.h"

#define Sound     R_Sound
#define Texture2D R_Texture2D
#define Camera    R_Camera
#define Camera2D  R_Camera2D
#define Camera3D  R_Camera3D
#define Color     R_Color
#define Rectangle R_Rectangle
#define Vector2   R_Vector2
#include "raylib.h"
#undef Sound
#undef Texture2D
#undef Camera
#undef Camera2D
#undef Camera3D
#undef Color
#undef Rectangle
#undef Vector2

#define Texture2D R_Texture2D
#define Vector2   R_Vector2
#include "rlImGui.h"
#undef Texture2D

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

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
    ClearBackground({ color.r, color.g, color.b, color.a });
}

void Platform_BeginMode2D(Camera2D camera)
{
    BeginMode2D({ { camera.offset.x, camera.offset.y },
                  { camera.position.x, camera.position.y },
                  camera.rotation,
                  camera.zoom });
}

void Platform_EndMode2D()
{
    EndMode2D();
}

void Platform_DrawRectangle(Rectangle rect, Color color)
{
    DrawRectangleRec({ rect.x, rect.y, rect.width, rect.height }, { color.r, color.g, color.b, color.a });
}

void Platform_DrawRectangleLines(Rectangle rect, float thickness, Color color)
{
    DrawRectangleLinesEx({ rect.x, rect.y, rect.width, rect.height }, thickness,
                         { color.r, color.g, color.b, color.a });
}

void Platform_DrawLine(Vector2Float start, Vector2Float end, float thickness, Color color)
{
    DrawLineEx({ start.x, start.y }, { end.x, end.y }, thickness, { color.r, color.g, color.b, color.a });
}

void Platform_DrawCircle(Vector2Float center, float radius, Color color)
{
    DrawCircleV({ center.x, center.y }, radius, { color.r, color.g, color.b, color.a });
}

void Platform_DrawRing(Vector2Float center, float innerRadius, float outerRadius, float startAngle, float endAngle,
                       int segments, Color color)
{
    DrawRing({ center.x, center.y }, innerRadius, outerRadius, startAngle, endAngle, segments,
             { color.r, color.g, color.b, color.a });
}
void Platform_SetClipRectangle(Rectangle rect, Camera2D camera)
{
    // convert from local coordinates to screen coordinates
    Vector2Float startPos = Utils_WorldToScreen2D({ rect.x, rect.y }, camera);
    Vector2Float endPos   = Utils_WorldToScreen2D({ rect.x + rect.width, rect.y + rect.height }, camera);

    BeginScissorMode(startPos.x, startPos.y, endPos.x - startPos.x, endPos.y - startPos.y);
}

void Platform_ClearClipRectangle()
{
    EndScissorMode();
}

Texture2D Platform_LoadTexture(const char* fileName, Arena* arena)
{
    (void)arena;  // Unused parameter
    R_Texture2D temp = LoadTexture(fileName);
    Texture2D   texture;
    texture.id      = temp.id;
    texture.width   = temp.width;
    texture.height  = temp.height;
    texture.mipmaps = temp.mipmaps;
    texture.format  = temp.format;
    return texture;
}

void Platform_UnloadTexture(Texture2D texture)
{
    R_Texture2D temp;
    temp.id      = texture.id;
    temp.width   = texture.width;
    temp.height  = texture.height;
    temp.mipmaps = texture.mipmaps;
    temp.format  = texture.format;
    UnloadTexture(temp);
}

void Platform_DrawTexture(Texture2D texture, Rectangle source, Rectangle dest, Vector2Float origin, float rotation,
                          Color tint)
{
    DrawTexturePro({ texture.id, texture.width, texture.height, texture.mipmaps, texture.format },
                   { source.x, source.y, source.width, source.height }, { dest.x, dest.y, dest.width, dest.height },
                   { origin.x, origin.y }, rotation, { tint.r, tint.g, tint.b, tint.a });
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

Sound Platform_LoadSound(const char* fileName, Arena* arena)
{
    (void)arena;  // Unused parameter
    R_Sound temp = LoadSound(fileName);
    Sound   sound;
    sound.data        = temp.stream.buffer;
    sound.sampleCount = temp.stream.sampleSize;
    sound.channels    = temp.stream.channels;
    sound.sampleRate  = temp.stream.sampleRate;
    return sound;
}

void Platform_UnloadSound(Sound sound)
{
    R_Sound temp;
    temp.stream.buffer     = (rAudioBuffer*)sound.data;
    temp.stream.sampleSize = sound.sampleCount;
    temp.stream.channels   = sound.channels;
    temp.stream.sampleRate = sound.sampleRate;
    temp.frameCount        = sound.sampleCount / sound.channels;
    (void)sound;
    UnloadSound(temp);
}

void Platform_PlaySound(Sound sound)
{
    R_Sound temp;
    temp.stream.buffer     = (rAudioBuffer*)sound.data;
    temp.stream.sampleSize = sound.sampleCount;
    temp.stream.channels   = sound.channels;
    temp.stream.sampleRate = sound.sampleRate;
    temp.frameCount        = sound.sampleCount / sound.channels;
    PlaySound(temp);
}

void Platform_StopSound(Sound sound)
{
    R_Sound temp;
    temp.stream.buffer     = (rAudioBuffer*)sound.data;
    temp.stream.sampleSize = sound.sampleCount;
    temp.stream.channels   = sound.channels;
    temp.stream.sampleRate = sound.sampleRate;
    temp.frameCount        = sound.sampleCount / sound.channels;
    PlaySound(temp);
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

int32_t Platform_GetMouseX()
{
    return (int32_t)GetMouseX();
}

int32_t Platform_GetMouseY()
{
    return (int32_t)GetMouseY();
}

int32_t Platform_GetMouseDeltaX()
{
    return (int32_t)GetMouseDelta().x;
}

int32_t Platform_GetMouseDeltaY()
{
    return (int32_t)GetMouseDelta().y;
}

float Platform_GetMouseWheelMove()
{
    return GetMouseWheelMove();
}

Vector2Float Platform_GetWorldToScreen2D(Vector2Float position, Camera2D camera)
{
    R_Vector2 temp = GetWorldToScreen2D({ position.x, position.y }, { { camera.offset.x, camera.offset.y },
                                                                      { camera.position.x, camera.position.y },
                                                                      camera.rotation,
                                                                      camera.zoom });
    return { temp.x, temp.y };
}

Vector2Float Platform_GetScreenToWorld2D(Vector2Float position, Camera2D camera)
{
    R_Vector2 temp = GetScreenToWorld2D({ position.x, position.y }, { { camera.offset.x, camera.offset.y },
                                                                      { camera.position.x, camera.position.y },
                                                                      camera.rotation,
                                                                      camera.zoom });
    return { temp.x, temp.y };
}

void Platform_TraceLog(int logType, const char* text, ...)
{
    va_list args;
    va_start(args, text);
    char textBuffer[2048];
    char buffer[2048];
    switch (logType)
    {
        case LOG_LEVEL_INFO:
            snprintf(textBuffer, sizeof(textBuffer), "ASH_INFO: %s", text);
            break;
        case LOG_LEVEL_WARNING:
            snprintf(textBuffer, sizeof(textBuffer), "ASH_WARNING: %s", text);
            break;
        case LOG_LEVEL_ERROR:
            snprintf(textBuffer, sizeof(textBuffer), "ASH_ERROR: %s", text);
            break;
        case LOG_LEVEL_DEBUG:
            snprintf(textBuffer, sizeof(textBuffer), "ASH_DEBUG: %s", text);
            break;
        default:
            break;
    }
    vsnprintf(buffer, sizeof(buffer), textBuffer, args);
    va_end(args);
    // TraceLog(logType, "%s", buffer);
    printf("%s\n", buffer);
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

uint32_t Platform_GetRandom()
{
    return rand();
}

uint32_t Platform_GetRandomMax()
{
    return RAND_MAX;
}
