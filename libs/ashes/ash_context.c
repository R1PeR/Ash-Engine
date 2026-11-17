#include "ash_context.h"

#include "ash_debug.h"
#include "ash_io.h"
#include "ash_platform.h"

#include <cstddef>
#include <stdint.h>

void Context_Init(Context* context, uint8_t maxModes, uint8_t maxUpdatables, Arena* arena)
{
    if (context == NULL)
    {
        LOG_ERR("Context_Init: context is NULL");
        return;
    }

    if (arena == NULL)
    {
        LOG_ERR("Context_Init: arena is NULL");
        return;
    }

    context->screen          = Arena_PushArray(Mode, arena, maxModes);
    context->updatables      = Arena_PushArray(Updatable*, arena, maxUpdatables);
    context->screenCount     = 0;
    context->updatablesCount = 0;
    context->currentFinished = false;
    context->resumed         = false;
    context->exitAll         = false;
    context->maxModes        = maxModes;
    context->maxUpdatables   = maxUpdatables;

    Context_ClearUpdatables(context);
}

void Context_SetMode(Context* context, Mode mode, void* data)
{
    if (context == NULL)
    {
        LOG_ERR("Context_SetMode: context is NULL");
        return;
    }

    if (context->screenCount + 1 < context->maxModes)
    {
        if (context->screenCount != 0)
        {
            context->screen[context->screenCount - 1].OnPause(context, data);
            Platform_EndDrawing();
        }
        context->screen[context->screenCount] = mode;
        context->screenCount += 1;
        context->currentFinished = false;
        context->screen[context->screenCount - 1].OnStart(context, data);
        while (!context->currentFinished && !context->exitAll)
        {
            for (int i = 0; i < context->updatablesCount; i++)
            {
                context->updatables[i]->Update();
            }

            // for IUpdatables
            if (context->resumed)
            {
                context->screen[context->screenCount - 1].OnResume(context, data);
                context->resumed = false;
            }
            context->screen[context->screenCount - 1].Update(context, data);
            // drawing
            Platform_BeginDrawing();
            context->screen[context->screenCount - 1].Draw(context, data);
            Platform_EndDrawing();
            if (Platform_WindowShouldClose())
            {
                context->exitAll = true;
            }
        }

        context->screen[context->screenCount - 1].OnStop(context, data);
        context->screenCount -= 1;
        if (context->screenCount != 0)
        {
            context->currentFinished = false;
            context->resumed         = true;
        }
    }
    else
    {
        LOG_INF("Context: SetMode() failed, not enough space");
    }
}

bool Context_AddUpdatable(Context* context, Updatable* updatable)
{
    if (context->updatablesCount + 1 < context->maxUpdatables)
    {
        context->updatables[context->updatablesCount] = updatable;
        context->updatablesCount++;
        return true;
    }
    return false;
}

void Context_ClearUpdatables(Context* context)
{
    for (int i = 0; i < context->updatablesCount; i++)
    {
        context->updatables[context->updatablesCount] = 0;
    }
    context->updatablesCount = 0;
}

void Context_FinishMode(Context* context)
{
    context->currentFinished = true;
}
