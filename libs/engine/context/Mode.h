#ifndef LIBS_ENGINE_MODE_H
#define LIBS_ENGINE_MODE_H

typedef struct Mode
{
    //OnStart runs once on mode start
    void (*OnStart)();
    //OnPause runs once when mode is paused 
    void (*OnPause)();
    //Update runs every frame
    void (*Update)();
    //OnStop runs once when mode is stopped
    void (*OnStop)();
    //OnResume runs once when mode is resumed
    void (*OnResume)();
} Mode;
#endif