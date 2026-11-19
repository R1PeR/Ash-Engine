#ifndef ASH_PLATFORM_H
#define ASH_PLATFORM_H

#include "ashes/ash_memory.h"
#include "ashes/ash_misc.h"

#include <stdint.h>

void     Platform_InitWindow(uint16_t width, uint16_t height, const char* title);
void     Platform_CloseWindow();
void     Platform_SetTargetFPS(uint32_t fps);
uint32_t Platform_GetScreenWidth();
uint32_t Platform_GetScreenHeight();
bool     Platform_WindowShouldClose();

void Platform_BeginDrawing();
void Platform_EndDrawing();
void Platform_ClearBackground(Color color);
void Platform_BeginMode2D(Camera2D camera);
void Platform_EndMode2D();

void Platform_DrawRectangle(Rectangle rect, Color color);
void Platform_DrawRectangleLines(Rectangle rect, float thickness, Color color);
void Platform_DrawLine(Vector2Float start, Vector2Float end, float thickness, Color color);
void Platform_DrawCircle(Vector2Float center, float radius, Color color);
void Platform_DrawRing(Vector2Float center, float innerRadius, float outerRadius, float startAngle, float endAngle,
                       int segments, Color color);
void Platform_SetClipRectangle(Rectangle rect, Camera2D camera);
void Platform_ClearClipRectangle();

Texture2D Platform_LoadTexture(const char* fileName, Arena* arena);
void      Platform_UnloadTexture(Texture2D texture);
void      Platform_DrawTexture(Texture2D texture, Rectangle source, Rectangle dest, Vector2Float origin, float rotation,
                               Color tint);

bool  Platform_InitAudioDevice();
void  Platform_CloseAudioDevice();
Sound Platform_LoadSound(const char* fileName, Arena* arena);
void  Platform_UnloadSound(Sound sound);
void  Platform_PlaySound(Sound sound);
void  Platform_StopSound(Sound sound);

bool    Platform_IsKeyPressed(uint16_t key);
bool    Platform_IsKeyDown(uint16_t key);
bool    Platform_IsKeyReleased(uint16_t key);
bool    Platform_IsKeyUp(uint16_t key);
bool    Platform_IsMouseButtonPressed(uint16_t button);
bool    Platform_IsMouseButtonDown(uint16_t button);
bool    Platform_IsMouseButtonReleased(uint16_t button);
bool    Platform_IsMouseButtonUp(uint16_t button);
int32_t Platform_GetMouseX();
int32_t Platform_GetMouseY();
int32_t Platform_GetMouseDeltaX();
int32_t Platform_GetMouseDeltaY();
float   Platform_GetMouseWheelMove();

Vector2Float Platform_GetWorldToScreen2D(Vector2Float position, Camera2D camera);
Vector2Float Platform_GetScreenToWorld2D(Vector2Float position, Camera2D camera);

void Platform_TraceLog(int logType, const char* text, ...);
void Platform_SetTraceLogLevel(int logType);

int Platform_GetFPS();

// void Platform_ImGuiSetup(bool dark);
// void Platform_ImGuiBegin();
// void Platform_ImGuiEnd();

uint32_t Platform_GetRandom();
uint32_t Platform_GetRandomMax();

#endif  // ASH_PLATFORM_H
