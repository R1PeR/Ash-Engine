#ifndef LIBS_ENGINE_LOGGER_H
#define LIBS_ENGINE_LOGGER_H
#include "raylib.h"
#include "utils.h"

#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 1

#ifdef DEBUG
    #ifdef LOG_LEVEL
        #if(LOG_LEVEL >= LOG_LEVEL_DEBUG)
            #define LOG_DBG(...) TraceLog(LOG_INFO, __VA_ARGS__)
        #else
            #define LOG_DBG(...) (void)0
        #endif

        #if(LOG_LEVEL >= LOG_LEVEL_INFO)
            #define LOG_INF(...) TraceLog(LOG_INFO, __VA_ARGS__)
        #else
            #define LOG_DBG(...) (void)0
        #endif

        #if(LOG_LEVEL >= LOG_LEVEL_WARNING)
            #define LOG_WRN(...) TraceLog(LOG_WARNING, __VA_ARGS__)
        #else
            #define LOG_WRN(...) (void)0
        #endif

        #if(LOG_LEVEL >= LOG_LEVEL_ERROR)
            #define LOG_ERR(...) TraceLog(LOG_ERROR, __VA_ARGS__)
        #else
            #define LOG_ERR(...) (void)0
        #endif
    #else
        #define LOG_LEVEL 0
    #endif
#endif

void Logger_Init();

#endif