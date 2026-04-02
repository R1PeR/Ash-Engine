#ifndef ASH_DEBUG_H
#define ASH_DEBUG_H

#include "ash_components.h"

#define LOG_LEVEL_DEBUG   4
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR   1

#ifdef DEBUG
#    ifdef LOG_LEVEL
#        if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
#            define LOG_DBG(...) TraceLog(LOG_INFO, __VA_ARGS__)
#        else
#            define LOG_DBG(...) (void)0
#        endif

#        if (LOG_LEVEL >= LOG_LEVEL_INFO)
#            define LOG_INF(...) TraceLog(LOG_INFO, __VA_ARGS__)
#        else
#            define LOG_INF(...) (void)0
#        endif

#        if (LOG_LEVEL >= LOG_LEVEL_WARNING)
#            define LOG_WRN(...) TraceLog(LOG_WARNING, __VA_ARGS__)
#        else
#            define LOG_WRN(...) (void)0
#        endif

#        if (LOG_LEVEL >= LOG_LEVEL_ERROR)
#            define LOG_ERR(...) TraceLog(LOG_ERROR, __VA_ARGS__)
#        else
#            define LOG_ERR(...) (void)0
#        endif
#    else
#        define LOG_LEVEL 0
#    endif
#else
#    define LOG_DBG(...) (void)0
#    define LOG_INF(...) (void)0
#    define LOG_WRN(...) (void)0
#    define LOG_ERR(...) (void)0
#endif

void Logger_Init();
void Logger_Deinit();

void Debug_ShowDebugWindow(Entity2D* ent, uint32_t entSize, Sprite* spr, uint32_t sprSize, Collider2D* col,
                           uint32_t colSize, TextureData* tex, uint32_t texSize, AnimatedSprite* anim,
                           uint32_t animSize, AudioData* aud, uint32_t audSize);

#endif  // ASH_DEBUG_H
