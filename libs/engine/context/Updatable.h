#ifndef LIBS_ENGINE_UPDATABLE_H
#define LIBS_ENGINE_UPDATABLE_H

typedef struct Updatable 
{
    void (*Update)();
} Updatable;
#endif