#ifndef LIBS_ENGINE_DELTATIME_H
#define LIBS_ENGINE_DELTATIME_H
#include "engine/context/Updatable.h"

void DeltaTime_Update();
float DeltaTime_GetDeltaTime();
Updatable * DeltaTime_GetUpdatable();
#endif