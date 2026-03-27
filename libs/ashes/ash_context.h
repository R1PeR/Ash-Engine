#ifndef ASH_CONTEXT_H
#define ASH_CONTEXT_H

/* Defines */
#define MAX_MODES      8
#define MAX_UPDATABLES 8
#define LIBS_ENGINE_UPDATABLE_H
#define MODE_FROM_CLASSNAME(className) \
    { className##_OnStart, className##_OnPause, className##_Update, className##_OnStop, className##_OnResume }

/* Structs, Enums, and Unions */
typedef struct Updatable Updatable;

typedef struct Mode
{
    // OnStart runs once on mode start
    void (*OnStart)();
    // OnPause runs once when mode is paused
    void (*OnPause)();
    // Update runs every frame
    void (*Update)();
    // OnStop runs once when mode is stopped
    void (*OnStop)();
    // OnResume runs once when mode is resumed
    void (*OnResume)();
} Mode;

typedef struct Updatable
{
    void (*Update)();
} Updatable;

/* Function Prototypes */

void Context_SetMode(Mode* mode);
void Context_ClearUpdatables();
bool Context_AddUpdatable(Updatable* updatable);
void Context_FinishMode();

#endif  // ASH_CONTEXT_H
