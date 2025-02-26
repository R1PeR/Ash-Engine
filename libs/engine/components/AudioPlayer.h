#ifndef LIBS_ENGINE_AUDIOPLAYER_H
#define LIBS_ENGINE_AUDIOPLAYER_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "raylib.h"
#define AUDIOPLAYER_MAX_COUNT 128
#define AUDIOPLAYER_MAX_NAME 32

typedef struct AudioPlayerData 
{
    Sound sound;
    uint32_t id;
} AudioPlayerData;

int32_t AudioPlayer_PlaySoundByName(const char *audioName);
bool AudioPlayer_StopSoundById(uint32_t id);
void AudioPlayer_StopAll();
#endif