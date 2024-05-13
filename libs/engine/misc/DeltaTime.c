#include "DeltaTime.h"
#include <time.h>
Updatable deltaTimeUpdatable = {DeltaTime_Update};
long deltaClocks = 0;
float deltaTime = 0.0f;

void DeltaTime_Update()
{
    deltaClocks = clock() - deltaClocks;
    deltaTime = deltaClocks / CLOCKS_PER_SEC;
}

float DeltaTime_GetDeltaTime()
{
    return deltaTime;
}

Updatable * DeltaTime_GetUpdatable()
{
    return &deltaTimeUpdatable;
}