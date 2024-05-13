#ifndef LIBS_ENGINE_WINDOW_H
#define LIBS_ENGINE_WINDOW_H
#include <stdint.h>
#include "raylib.h"

void Window_Init(uint16_t width, uint16_t height, const char *title);
Camera2D * Window_GetCamera();
#endif