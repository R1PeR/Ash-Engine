
#ifndef LIBS_ENGINE_ENDGAMEMODE_H
#define LIBS_ENGINE_ENDGAMEMODE_H
#include "engine/context/Mode.h"

#include <stdint.h>

extern Mode EndGameMode;

void EndGameMode_OnStart();
void EndGameMode_OnPause();
void EndGameMode_Update();
void EndGameMode_OnStop();
void EndGameMode_OnResume();

#endif
