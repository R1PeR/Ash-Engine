#ifndef LIBS_ENGINE_ANIMATED_SPRITES_H
#define LIBS_ENGINE_ANIMATED_SPRITES_H
#include "engine/components/Sprite.h"
#include "engine/misc/Stopwatch.h"
#include <stdint.h>
#include <stdlib.h>
#define MAX_ANIMATION_FRAMES 32
#define MAX_ANIMATED_SPRITES 32

typedef struct AnimationData
{
    Texture2D * animationFrames[MAX_ANIMATION_FRAMES];
    uint8_t animationFrameCount;
}AnimationData;

typedef struct AnimatedSprite
{
    Sprite sprite;
    AnimationData * currentAnimation;
    uint8_t id;
    uint16_t frameTime;
    bool isPlaying;
    bool repeat;
    uint8_t currentFrame;
    Stopwatch stopwatch;
}AnimatedSprite;

void AnimatedSprite_Initialize(AnimatedSprite * animatedSprite);
bool AnimatedSprite_Add(AnimatedSprite * animatedSprite);
bool AnimatedSprite_Clear();
void AnimatedSprite_Play(AnimatedSprite * animatedSprite, AnimationData * animation, bool repeat);
void AnimatedSprite_Stop(AnimatedSprite * animatedSprite);
void AnimatedSprite_Update();

uint8_t AnimatedSprite_GetCount();
AnimatedSprite ** AnimatedSprite_GetAnimatedSprites();
Updatable * AnimatedSprite_GetUpdatable();
#endif