#ifndef LIBS_ENGINE_AUDIO_H
#define LIBS_ENGINE_AUDIO_H
#include <stdint.h>
#include <stdlib.h>
#include "raylib.h"
#include "utils.h"
#define MAX_AUDIO 1024
#define MAX_NAME 32

typedef struct AudioData 
{
    Sound sound;
    char soundName[MAX_NAME];
    uint32_t id;
} AudioData;

bool Audio_LoadAudio(const char *fileName);
bool Audio_UnloadAudioByName(const char *audioName);
void Audio_UnloadAudios();

uint32_t Audio_GetCount();
AudioData * Audio_GetAudios();
#endif