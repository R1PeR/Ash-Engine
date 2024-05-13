#ifndef LIBS_ENGINE_STOPWATCH_H
#define LIBS_ENGINE_STOPWATCH_H
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#define CLOCKS_PER_MS CLOCKS_PER_SEC/1000

typedef struct Stopwatch
{
    int32_t startTime;
    int32_t endTime;
} Stopwatch;

void Stopwatch_Start(Stopwatch * stopwatch, uint32_t milis);
void Stopwatch_Stop(Stopwatch * stopwatch);

uint32_t Stopwatch_GetElapsedTime(Stopwatch * stopwatch);
uint32_t Stopwatch_GetRemainingTime(Stopwatch * stopwatch);

bool Stopwatch_IsRunning(Stopwatch * stopwatch);
bool Stopwatch_IsElapsed(Stopwatch * stopwatch);
bool Stopwatch_IsZero(Stopwatch * stopwatch);

#endif