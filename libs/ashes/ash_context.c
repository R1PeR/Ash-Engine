#include "ash_context.h"

#include "ash_debug.h"
#include "ash_io.h"
#include "ash_platform.h"

#include <stdint.h>

Mode*      screen[MAX_MODES];
Updatable* updatables[MAX_UPDATABLES];
uint8_t    screenCount     = 0;
uint8_t    updatablesCount = 0;
bool       currentFinished = false;
bool       resumed         = false;
bool       exitAll         = false;

void Context_SetMode(Mode* mode)
{
    if (screenCount + 1 < MAX_MODES)
    {
        if (screenCount != 0)
        {
            screen[screenCount - 1]->OnPause();
            Platform_EndMode2D();
            Platform_ImGuiEnd();
            Platform_EndDrawing();
        }
        screen[screenCount] = mode;
        screenCount += 1;
        currentFinished = false;
        screen[screenCount - 1]->OnStart();
        while (!currentFinished && !exitAll)
        {
            for (int i = 0; i < updatablesCount; i++)
            {
                updatables[i]->Update();
            }

            // for IUpdatables
            if (resumed)
            {
                screen[screenCount - 1]->OnResume();
                resumed = false;
            }
            screen[screenCount - 1]->Update();
            // drawing
            Platform_BeginDrawing();
            Platform_ImGuiBegin();
            Platform_ClearBackground(BLACK);
            Platform_BeginMode2D(*Window_GetCamera());
            screen[screenCount - 1]->Draw();
            Platform_EndMode2D();
            Platform_ImGuiEnd();
            Platform_EndDrawing();
            if (Platform_WindowShouldClose())
            {
                exitAll = true;
            }
        }

        screen[screenCount - 1]->OnStop();
        screenCount -= 1;
        if (screenCount != 0)
        {
            currentFinished = false;
            resumed         = true;
        }
    }
    else
    {
        LOG_INF("Context: SetMode() failed, not enough space");
    }
}

bool Context_AddUpdatable(Updatable* updatable)
{
    if (updatablesCount + 1 < MAX_UPDATABLES)
    {
        updatables[updatablesCount] = updatable;
        updatablesCount++;
        return true;
    }
    return false;
}

void Context_ClearUpdatables()
{
    for (int i = 0; i < updatablesCount; i++)
    {
        updatables[updatablesCount] = 0;
    }
    updatablesCount = 0;
}

void Context_FinishMode()
{
    currentFinished = true;
}
