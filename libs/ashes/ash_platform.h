#ifndef ASH_PLATFORM_H
#define ASH_PLATFORM_H

#include <raylib.h>
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

void Platform_DrawRectangleRec(Rectangle rect, Color color);
void Platform_DrawRectangleLinesEx(Rectangle rect, float thickness, Color color);
void Platform_DrawRectangleLines(int x, int y, int w, int h, Color color);
void Platform_DrawLineEx(Vector2 start, Vector2 end, float thickness, Color color);
void Platform_DrawCircleV(Vector2 center, float radius, Color color);
void Platform_DrawRing(Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle,
                       int segments, Color color);

Texture2D Platform_LoadTexture(const char* fileName);
void      Platform_UnloadTexture(Texture2D texture);
void      Platform_DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation,
                                  Color tint);

bool  Platform_InitAudioDevice();
void  Platform_CloseAudioDevice();
Sound Platform_LoadSound(const char* fileName);
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
int16_t Platform_GetMouseX();
int16_t Platform_GetMouseY();
int16_t Platform_GetMouseDeltaX();
int16_t Platform_GetMouseDeltaY();
float   Platform_GetMouseWheelMove();

Vector2 Platform_GetWorldToScreen2D(Vector2 position, Camera2D camera);
Vector2 Platform_GetScreenToWorld2D(Vector2 position, Camera2D camera);

void Platform_TraceLog(int logType, const char* text, ...);
void Platform_SetTraceLogLevel(int logType);

int Platform_GetFPS();

void Platform_ImGuiSetup(bool dark);
void Platform_ImGuiBegin();
void Platform_ImGuiEnd();

#endif  // ASH_PLATFORM_H
