
#ifndef LIBS_ENGINE_MAPEDITORMODE_H
#define LIBS_ENGINE_MAPEDITORMODE_H
#include "ashes/ash_context.h"

#include <stdint.h>

extern Mode mapEditorMode;

void MapEditorMode_OnStart();
void MapEditorMode_OnPause();
void MapEditorMode_Update();
void MapEditorMode_OnStop();
void MapEditorMode_OnResume();

#endif
