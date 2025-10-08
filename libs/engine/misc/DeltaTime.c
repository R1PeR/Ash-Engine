#include "DeltaTime.h"

#include "Logger.h"

#include <time.h>
Updatable deltaTimeUpdatable = { DeltaTime_Update };
long      lastClock          = 0;
long      deltaClock         = 0;
float     deltaTime          = 0.0f;

void DeltaTime_Update()
{
    long currentClock = clock();
    deltaClock        = currentClock - lastClock;
    lastClock         = currentClock;
    deltaTime         = (float)deltaClock / (float)CLOCKS_PER_SEC;
}

float DeltaTime_GetDeltaTime()
{
    return deltaTime;
}

Updatable* DeltaTime_GetUpdatable()
{
    return &deltaTimeUpdatable;
}
