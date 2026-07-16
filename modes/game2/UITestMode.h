
#ifndef LIBS_ENGINE_UITESTMODE_H
#define LIBS_ENGINE_UITESTMODE_H
#include "ashes/ash_context.h"
#include "ashes/ash_misc.h"

#include <stdint.h>

extern Mode uiTestMode;

void UITestMode_OnStart();
void UITestMode_OnPause();
void UITestMode_Update();
void UITestMode_OnStop();
void UITestMode_OnResume();

#endif  // LIBS_ENGINE_UITESTMODE_H
