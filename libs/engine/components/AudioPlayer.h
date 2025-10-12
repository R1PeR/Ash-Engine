#ifndef LIBS_ENGINE_AUDIOPLAYER_H
#define LIBS_ENGINE_AUDIOPLAYER_H
#include "raylib.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define AUDIOPLAYER_MAX_COUNT 128
#define AUDIOPLAYER_MAX_NAME  32

typedef struct AudioPlayerData
{
    Sound    sound;
    uint32_t id;
} AudioPlayerData;

int32_t AudioPlayer_PlaySoundByName(const char* audioName);
int32_t AudioPlayer_PlaySoundById(uint32_t id);
bool    AudioPlayer_StopSoundById(uint32_t id);
void    AudioPlayer_StopAll();
#endif
