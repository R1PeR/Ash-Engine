#ifndef LIBS_ENGINE_CONTEXT_H
#define LIBS_ENGINE_CONTEXT_H
#include <stdint.h>
#include <stdbool.h>
#include "Mode.h"
#include "Updatable.h"
#define MAX_MODES 8
#define MAX_UPDATABLES 8

void Context_SetMode(Mode* mode);
void Context_ClearUpdatables();
bool Context_AddUpdatable(Updatable * updatable);
void Context_FinishMode();
#endif