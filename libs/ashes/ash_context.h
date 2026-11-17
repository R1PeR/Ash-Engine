#ifndef ASH_CONTEXT_H
#define ASH_CONTEXT_H

#include "ash_io.h"
#include "ash_memory.h"

/* Defines */
#define MODE_FROM_CLASSNAME(className)                              \
    { className##_OnStart, className##_OnPause, className##_Update, \
      className##_Draw,    className##_OnStop,  className##_OnResume }

/* Structs, Enums, and Unions */
typedef struct Updatable Updatable;
typedef struct Context   Context;
typedef struct Mode
{
    // OnStart runs once on mode start
    void (*OnStart)(Context* context, void* data);
    // OnPause runs once when mode is paused
    void (*OnPause)(Context* context, void* data);
    // Update runs every frame
    void (*Update)(Context* context, void* data);
    // Draw runs every frame after Update
    void (*Draw)(Context* context, void* data);
    // OnStop runs once when mode is stopped
    void (*OnStop)(Context* context, void* data);
    // OnResume runs once when mode is resumed
    void (*OnResume)(Context* context, void* data);
} Mode;

typedef struct Updatable
{
    void (*Update)();
} Updatable;

typedef struct Context
{
    Mode*       screen;
    Updatable** updatables;
    uint8_t     screenCount;
    uint8_t     updatablesCount;
    bool        currentFinished;
    bool        resumed;
    bool        exitAll;
    uint8_t     maxModes;
    uint8_t     maxUpdatables;
    Window*     window;
} Context;

#define CONTEXT_SET_MODE(context, mode, data) Context_SetMode(context, MODE_FROM_CLASSNAME(mode), data)
#define CONTEXT_DATA_FROM_MODE(mode)          mode* state = ((mode*)data)

/* Function Prototypes */
void Context_Init(Context* context, uint8_t maxModes, uint8_t maxUpdatables, Arena* arena);
void Context_SetMode(Context* context, Mode mode, void* data);
void Context_ClearUpdatables(Context* context);
bool Context_AddUpdatable(Context* context, Updatable* updatable);
void Context_FinishMode(Context* context);

#endif  // ASH_CONTEXT_H
