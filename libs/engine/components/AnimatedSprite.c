#include "AnimatedSprite.h"
#include "engine/misc/Logger.h"
#include "engine/components/Texture.h"
#include <string.h>
#include <stdio.h>

#define DEFAULT_ANIMATION_SPEED 33
Updatable animatedSpriteUpdatable = {AnimatedSprite_Update};
uint32_t sAnimatedSpriteCount = 0;
AnimatedSprite * sAnimatedSprites[ANIMATEDSPRITE_MAX_COUNT];

void AnimatedSprite_Initialize(AnimatedSprite * animatedSprite)
{
    animatedSprite->frameTime = DEFAULT_ANIMATION_SPEED;
    animatedSprite->currentAnimation = NULL;
    animatedSprite->isPlaying = false;
    animatedSprite->repeat = false;
    animatedSprite->currentFrame = 0;
    Sprite_Initialize(&animatedSprite->sprite);
}

bool AnimatedSprite_Add(AnimatedSprite * animatedSprite)
{
    if(sAnimatedSpriteCount < ANIMATEDSPRITE_MAX_COUNT)
    {
        if(Sprite_Add(&animatedSprite->sprite))
        {
            animatedSprite->id = sAnimatedSpriteCount;
            sAnimatedSprites[sAnimatedSpriteCount] = animatedSprite;
            sAnimatedSpriteCount++;
            return true;
        }
        return false;
    }
    LOG_ERR("AnimatedSprite: Add() failed, not enough space");
    return false;
}

bool AnimatedSprite_Clear()
{
    sAnimatedSpriteCount = 0;
    for(uint32_t i = 0; i < ANIMATEDSPRITE_MAX_COUNT; i++)
    {
        sAnimatedSprites[i] = 0;
    }
    return true;
}

void AnimatedSprite_Play(AnimatedSprite * animatedSprite, AnimationData * animation, bool repeat)
{
    animatedSprite->currentAnimation = animation;
    animatedSprite->repeat = repeat;
    animatedSprite->isPlaying = true;
    animatedSprite->currentFrame = 0;
}

void AnimatedSprite_Stop(AnimatedSprite * animatedSprite)
{
    animatedSprite->isPlaying = false;
}

void AnimatedSprite_Update()
{
    for(uint32_t i = 0; i < sAnimatedSpriteCount; i++)
    {
        if(sAnimatedSprites[i]->isPlaying && sAnimatedSprites[i]->currentAnimation && Stopwatch_IsZero(&sAnimatedSprites[i]->stopwatch))
        {
            sAnimatedSprites[i]->sprite.currentTexture = sAnimatedSprites[i]->currentAnimation->animationFrames[sAnimatedSprites[i]->currentFrame++];
            if(sAnimatedSprites[i]->currentFrame >= sAnimatedSprites[i]->currentAnimation->animationFrameCount)
            {
                if(sAnimatedSprites[i]->repeat)
                {
                    sAnimatedSprites[i]->currentFrame = 0;
                }
                else
                {
                    sAnimatedSprites[i]->isPlaying = false;
                }
            }
            Stopwatch_Start(&sAnimatedSprites[i]->stopwatch, sAnimatedSprites[i]->frameTime);
        }
    }
}

bool AnimatedSprite_SetAnimationDataFromTextureSheet(AnimationData * data, const char * textureName, uint8_t startFrame, uint8_t frameCount)
{
    char buffor[TEXTURE_MAX_NAME];
    for(uint8_t i = 0; i < frameCount; i++)
    {
        sprintf(buffor, "%s_%d", textureName, i);
        data->animationFrames[i] = Texture_GetTextureByName(buffor);
    }
    data->animationFrameCount = frameCount;
}

uint8_t AnimatedSprite_GetCount()
{
    return sAnimatedSpriteCount;
}

AnimatedSprite ** AnimatedSprite_GetAnimatedSprites()
{
    return sAnimatedSprites;
}

Updatable * AnimatedSprite_GetUpdatable()
{
    return &animatedSpriteUpdatable;
}