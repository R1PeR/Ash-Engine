#include "Context.h"

#include "engine/io/Window.h"
#include "engine/misc/Logger.h"
#include "raylib.h"
#include "rlImGui.h"

#include <stdio.h>

Mode*      screen[MAX_MODES];
Updatable* updatables[MAX_UPDATABLES];
uint8_t    screenCount     = 0;
uint8_t    updatablesCount = 0;
bool       currentFinished = false;
bool       resumed         = false;

void Context_SetMode(Mode* mode)
{
    if(screenCount + 1 < MAX_MODES)
    {
        if(screenCount != 0)
        {
            screen[screenCount - 1]->OnPause();
        }
        screen[screenCount] = mode;
        screenCount += 1;
        currentFinished = false;
        screen[screenCount - 1]->OnStart();
        while(!currentFinished && !WindowShouldClose())
        {
            BeginDrawing();
            rlImGuiBegin();
            ClearBackground(BLACK);
            BeginMode2D(*Window_GetCamera());
            for(int i = 0; i < updatablesCount; i++)
            {
                updatables[i]->Update();
            }

            // for IUpdatables
            if(resumed)
            {
                screen[screenCount - 1]->OnResume();
                resumed = false;
            }
            screen[screenCount - 1]->Update();
            EndMode2D();
            DrawFPS(10, 10);
            rlImGuiEnd();
            EndDrawing();
        }
        screen[screenCount - 1]->OnStop();
        screenCount -= 1;
        if(screenCount != 0)
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
    if(updatablesCount + 1 < MAX_UPDATABLES)
    {
        updatables[updatablesCount] = updatable;
        updatablesCount++;
        return true;
    }
    return false;
}

void Context_ClearUpdatables()
{
    for(int i = 0; i < updatablesCount; i++)
    {
        updatables[updatablesCount] = 0;
    }
    updatablesCount = 0;
}

void Context_FinishMode()
{
    currentFinished = true;
}
