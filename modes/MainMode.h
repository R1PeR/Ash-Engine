#ifndef LIBS_ENGINE_MAINMODE_H
#define LIBS_ENGINE_MAINMODE_H
#include "engine/context/Mode.h"

extern Mode mainMode;

void MainMode_OnStart();
void MainMode_OnPause();
void MainMode_Update();
void MainMode_OnStop();
void MainMode_OnResume();
#endif
