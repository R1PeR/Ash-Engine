#ifndef LIBS_ENGINE_AUDIO_H
#define LIBS_ENGINE_AUDIO_H
#include <stdint.h>
#include <stdlib.h>
#include "raylib.h"
#include "utils.h"
#define AUDIO_MAX_COUNT 1024
#define AUDIO_MAX_NAME 32

typedef struct AudioData 
{
    Sound sound;
    char soundName[AUDIO_MAX_NAME];
    uint32_t id;
} AudioData;

bool Audio_Init();
void Audio_Deinit();
bool Audio_LoadAudio(const char *fileName);
bool Audio_UnloadAudioByName(const char *audioName);
void Audio_UnloadAudios();

uint32_t Audio_GetCount();
AudioData * Audio_GetAudios();
#endif