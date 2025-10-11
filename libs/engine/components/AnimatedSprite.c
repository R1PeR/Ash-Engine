#include "AnimatedSprite.h"

#include "engine/components/Texture.h"
#include "engine/misc/Logger.h"

#include <stdio.h>
#include <string.h>

#define DEFAULT_ANIMATION_SPEED 33
Updatable       animatedSpriteUpdatable = { AnimatedSprite_Update };
uint32_t        sAnimatedSpriteCount    = 0;
AnimatedSprite* sAnimatedSpriteList;  //[ANIMATEDSPRITE_MAX_COUNT];

void AnimatedSprite_Initialize(AnimatedSprite* animatedSprite)
{
    animatedSprite->frameTime        = DEFAULT_ANIMATION_SPEED;
    animatedSprite->currentAnimation = NULL;
    animatedSprite->isPlaying        = false;
    animatedSprite->repeat           = false;
    animatedSprite->currentFrame     = 0;
    Sprite_Initialize(&animatedSprite->sprite);
}

bool AnimatedSprite_Add(AnimatedSprite* animatedSprite)
{
    if (sAnimatedSpriteCount == 0)
    {
        sAnimatedSpriteList       = animatedSprite;
        sAnimatedSpriteList->next = NULL;
    }
    else
    {
        AnimatedSprite* current = sAnimatedSpriteList;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next        = animatedSprite;
        animatedSprite->next = NULL;
    }
    sAnimatedSpriteCount++;
    return true;
}

bool AnimatedSprite_Clear()
{
    sAnimatedSpriteCount = 0;
    sAnimatedSpriteList  = NULL;
    return true;
}

void AnimatedSprite_Play(AnimatedSprite* animatedSprite, AnimationData* animation, bool repeat)
{
    animatedSprite->currentAnimation = animation;
    animatedSprite->repeat           = repeat;
    animatedSprite->isPlaying        = true;
    animatedSprite->currentFrame     = 0;
}

void AnimatedSprite_Stop(AnimatedSprite* animatedSprite)
{
    animatedSprite->isPlaying = false;
}

void AnimatedSprite_Update()
{
    AnimatedSprite* current = sAnimatedSpriteList;
    while (current != NULL)
    {
        if (current->isPlaying && current->currentAnimation && Stopwatch_IsZero(&current->stopwatch))
        {
            current->sprite.currentTexture = current->currentAnimation->animationFrames[current->currentFrame++];
            if (current->currentFrame >= current->currentAnimation->animationFrameCount)
            {
                if (current->repeat)
                {
                    current->currentFrame = 0;
                }
                else
                {
                    current->isPlaying = false;
                }
            }
            Stopwatch_Start(&current->stopwatch, current->frameTime);
        }
        current = current->next;
    }
}

bool AnimatedSprite_SetAnimationDataFromTextureSheet(AnimationData* data, const char* textureName, uint8_t startFrame,
                                                     uint8_t frameCount)
{
    char buffor[TEXTURE_MAX_NAME];
    for (uint8_t i = 0; i < frameCount; i++)
    {
        sprintf(buffor, "%s_%d", textureName, startFrame + i);
        data->animationFrames[i] = Texture_GetTextureByName(buffor);
    }
    data->animationFrameCount = frameCount;
    return frameCount > 0;
}

uint32_t AnimatedSprite_GetCount()
{
    return sAnimatedSpriteCount;
}

AnimatedSprite* AnimatedSprite_GetAnimatedSpriteList()
{
    return sAnimatedSpriteList;
}

Updatable* AnimatedSprite_GetUpdatable()
{
    return &animatedSpriteUpdatable;
}
