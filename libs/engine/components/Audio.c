#include "Audio.h"
#include "engine/misc/Logger.h"
#include <string.h>
#include <stdio.h>

uint32_t sAudioCount;
AudioData sAudios[AUDIO_MAX_COUNT];

bool Audio_Init()
{
    InitAudioDevice();
    return IsAudioDeviceReady();
}

void Audio_Deinit()
{
    CloseAudioDevice();
}

bool Audio_AddSound(Sound sound, const char * soundName)
{
    if(sAudioCount+1 < AUDIO_MAX_COUNT)
    {
        sAudios[sAudioCount].sound = sound;
        sAudios[sAudioCount].id = sAudioCount;
        strcpy(sAudios[sAudioCount].soundName, soundName);
        sAudioCount++;
        return true;
    }
    LOG_ERR("Audio: Texture_AddSound() failed, not enough space");
    return false;
}


bool Audio_LoadAudio(const char *fileName)
{
    if(fileName == NULL)
    {
        return false;
    }
    Sound sound = LoadSound(fileName);
    if(sound.frameCount > 0)//Texture loaded correctly(?)
    {
        return Audio_AddSound(sound, GetFileNameWithoutExt(fileName));
    }
    return false;
}

bool Audio_UnloadAudioByName(const char *audioName)
{
    if(audioName == NULL)
    {
        return false;
    }
    bool moveSounds = false;
    for(uint32_t i = 0; i < sAudioCount; i++)
    {
        if((strcmp(audioName, sAudios[i].soundName) == 0) && moveSounds == false)
        {
            moveSounds = true;
            UnloadSound(sAudios[i].sound);
        }
        if(moveSounds && i != sAudioCount-1)
        {
            sAudios[i] = sAudios[i+1];
        }
    }
    if(moveSounds)
    {
        sAudioCount--;
    }
    return moveSounds;
}

void Audio_UnloadAudios()
{
    for(uint32_t i = 0; i < sAudioCount; i++)
    {
        UnloadSound(sAudios->sound);
    }
    sAudioCount = 0;
}

uint32_t Audio_GetCount()
{
    return sAudioCount;
}

AudioData * Audio_GetAudios()
{
    return sAudios;
}
