#ifndef LIBS_ENGINE_WINDOW_H
#define LIBS_ENGINE_WINDOW_H
#include "raylib.h"
#include "engine/components/Entity2D.h"

#include <stdint.h>

void      Window_Init(uint16_t width, uint16_t height, const char* title);
void      Window_Deinit();
Camera2D* Window_GetCamera();
uint32_t  Window_GetWidth();
uint32_t  Window_GetHeight();

#endif
