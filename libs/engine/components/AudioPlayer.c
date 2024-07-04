#include "AudioPlayer.h"
#include "engine/components/Audio.h"
#include "raylib.h"
#include <string.h>

uint32_t sAudioPlayerCount = 0;
AudioPlayerData sAudioPlayers[AUDIOPLAYER_MAX_COUNT];

int32_t AudioPlayer_PlaySoundByName(const char *audioName)
{
    if(sAudioPlayerCount >= AUDIOPLAYER_MAX_COUNT)
    {
        return -1;
    }
    for (uint32_t i = 0; i < Audio_GetCount(); i++)
    {
        if(strcmp(audioName, Audio_GetAudios()[i].soundName) == 0)
        {
            uint16_t currentId = Audio_GetAudios()[i].id;
            sAudioPlayers[sAudioPlayerCount].id = currentId;
            sAudioPlayers[sAudioPlayerCount].sound = LoadSoundAlias(Audio_GetAudios()[i].sound);
            PlaySound(sAudioPlayers[sAudioPlayerCount].sound);
            sAudioPlayerCount++;
            return currentId;
        }
    }
    return -1;
}

bool AudioPlayer_StopSoundById(uint32_t id)
{
    bool moveAudioPlayers = false;
    for(uint32_t i = 0; i < sAudioPlayerCount; i++)
    {
        if(sAudioPlayers[i].id == id && moveAudioPlayers == false)
        {
            moveAudioPlayers = true;
            UnloadSoundAlias(sAudioPlayers[i].sound);
        }
        if(moveAudioPlayers && i != sAudioPlayerCount-1)
        {
            sAudioPlayers[i] = sAudioPlayers[i+1];
        }
    }
    if(moveAudioPlayers)
    {
        sAudioPlayerCount--;
    }
    return moveAudioPlayers;
}

void AudioPlayer_StopAll()
{
    for (uint32_t i = 0; i < Audio_GetCount(); i++)
    {
        StopSound(Audio_GetAudios()[i].sound);
        sAudioPlayerCount = 0;
    }
}
