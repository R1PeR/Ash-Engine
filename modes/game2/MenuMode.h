
#ifndef LIBS_ENGINE_MENUMODE_H
#define LIBS_ENGINE_MENUMODE_H
#include "ashes/ash_context.h"

extern Mode menuMode;

void MenuMode_OnStart();
void MenuMode_OnPause();
void MenuMode_Update();
void MenuMode_OnStop();
void MenuMode_OnResume();

#endif
