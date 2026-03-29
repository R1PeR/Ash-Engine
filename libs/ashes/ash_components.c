#include "ash_components.h"

#include "ash_misc.h"

#include <cstring>
#include <stdio.h>

Updatable       animatedSpriteUpdatable = { AnimatedSprite_Update };
uint32_t        sAnimatedSpriteCount    = 0;
uint32_t        sAnimatedSpriteMaxCount = 0;
AnimatedSprite* sAnimatedSpriteArray;

void AnimatedSprite_SetPool(AnimatedSprite* pool, size_t poolSize)
{
    sAnimatedSpriteArray    = pool;
    sAnimatedSpriteMaxCount = poolSize;
    sAnimatedSpriteCount    = 0;
}

void AnimatedSprite_Initialize(AnimatedSprite* animatedSprite)
{
    animatedSprite->frameTime        = ANIMATEDSPRITE_DEFAULT_ANIMATION_SPEED;
    animatedSprite->currentAnimation = NULL;
    animatedSprite->isPlaying        = false;
    animatedSprite->repeat           = false;
    animatedSprite->currentFrame     = 0;
    Sprite_Initialize(&animatedSprite->sprite);
}

bool AnimatedSprite_Add(AnimatedSprite* animatedSprite)
{
    if (sAnimatedSpriteCount >= sAnimatedSpriteMaxCount)
    {
        LOG_ERR("Max AnimatedSprite count reached! Cannot add more AnimatedSprites.");
        return false;
    }
    sAnimatedSpriteArray[sAnimatedSpriteCount] = *animatedSprite;
    sAnimatedSpriteCount++;
    return true;
}

bool AnimatedSprite_Clear()
{
    sAnimatedSpriteCount = 0;
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
    for (int i = 0; i < sAnimatedSpriteCount; i++)
    {
        if (sAnimatedSpriteArray[i].isPlaying && sAnimatedSpriteArray[i].currentAnimation
            && Stopwatch_IsZero(&sAnimatedSpriteArray[i].stopwatch))
        {
            sAnimatedSpriteArray[i].sprite.currentTexture =
                sAnimatedSpriteArray[i].currentAnimation->animationFrames[sAnimatedSpriteArray[i].currentFrame++];
            if (sAnimatedSpriteArray[i].currentFrame >= sAnimatedSpriteArray[i].currentAnimation->animationFrameCount)
            {
                if (sAnimatedSpriteArray[i].repeat)
                {
                    sAnimatedSpriteArray[i].currentFrame = 0;
                }
                else
                {
                    sAnimatedSpriteArray[i].isPlaying = false;
                }
            }
            Stopwatch_Start(&sAnimatedSpriteArray[i].stopwatch, sAnimatedSpriteArray[i].frameTime);
        }
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

AnimatedSprite* AnimatedSprite_GetAnimatedSpriteArray()
{
    return sAnimatedSpriteArray;
}

Updatable* AnimatedSprite_GetUpdatable()
{
    return &animatedSpriteUpdatable;
}

void AsciiWindow_Initalize(AsciiWindow* window, const char* textureName)
{
    char       name[TEXTURE_MAX_NAME];
    uint32_t   textureId = 0;
    Texture2D* texture;
    while (true)
    {
        sprintf(name, "%s_%d", textureName, textureId);
        texture = Texture_GetTextureByName(name);
        if (texture == NULL)
        {
            break;
        }
        window->textureBuffer[textureId] = texture;
        textureId++;
    }
    AsciiWindow_Clear(window);
    for (uint32_t i = 0; i < window->height * window->width; i++)
    {
        Sprite_Initialize(&window->spriteBuffer[i]);
    }
    Entity2D_Add(&window->entity);
    window->spriteHeight = window->textureBuffer[0]->height;
    window->spriteWidth  = window->textureBuffer[0]->width;
    for (uint32_t i = 0; i < window->height; i++)
    {
        for (uint32_t j = 0; j < window->width; j++)
        {
            window->spriteBuffer[i * window->width + j].position.x = window->textureBuffer[0]->width
                                                                     * window->spriteBuffer[i * window->width + j].scale
                                                                     * window->entity.scale * j;
            window->spriteBuffer[i * window->width + j].position.y = window->textureBuffer[0]->height
                                                                     * window->spriteBuffer[i * window->width + j].scale
                                                                     * window->entity.scale * i;
            window->spriteBuffer[i * window->width + j].currentTexture = window->textureBuffer[0];
            window->spriteBuffer[i * window->width + j].parent         = &window->entity;
            Sprite_Add(&window->spriteBuffer[i * window->width + j]);
        }
    }
}

void AsciiWindow_SetCharacter(AsciiWindow* window, uint32_t x, uint32_t y, char c)
{
    AsciiWindow_SetCell(window, x, y, static_cast<uint32_t>(c) - ASCIIWINDOW_ASCII_START);
}

char AsciiWindow_GetCharacter(AsciiWindow* window, uint32_t x, uint32_t y)
{
    return static_cast<char>(AsciiWindow_GetCell(window, x, y) + ASCIIWINDOW_ASCII_START);
}

void AsciiWindow_SetCell(AsciiWindow* window, uint32_t x, uint32_t y, uint32_t value)
{
    window->windowBuffer[y * window->width + x] = value;
}

uint32_t AsciiWindow_GetCell(AsciiWindow* window, uint32_t x, uint32_t y)
{
    return window->windowBuffer[y * window->width + x];
}

void AsciiWindow_Clear(AsciiWindow* window)
{
    for (uint32_t i = 0; i < window->height * window->width; i++)
    {
        window->windowBuffer[i] = 0;
    }
}

void AsciiWindow_Draw(AsciiWindow* window)
{
    for (uint32_t i = 0; i < window->height; i++)
    {
        for (uint32_t j = 0; j < window->width; j++)
        {
            uint32_t currentChar                                       = AsciiWindow_GetCell(window, j, i);
            window->spriteBuffer[i * window->width + j].currentTexture = window->textureBuffer[currentChar];
        }
    }
}

void AsciiWindow_DrawBorder(AsciiWindow* window, AsciiWindowBorder border)
{
    for (uint32_t i = 0; i < window->height; i++)
    {
        for (uint32_t j = 0; j < window->width; j++)
        {
            if (i == 0 && j == 0)
            {
                AsciiWindow_SetCell(window, j, i, border.top[0]);
            }
            else if (i == 0 && j == window->width - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.top[2]);
            }
            else if (i == window->height - 1 && j == 0)
            {
                AsciiWindow_SetCell(window, j, i, border.bottom[0]);
            }
            else if (i == window->height - 1 && j == window->width - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.bottom[2]);
            }
            else if (i == 0 && j != 0 && j != window->width - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.top[1]);
            }
            else if (i == window->height - 1 && j != 0 && j != window->width - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.bottom[1]);
            }
            else if (j == 0 && i != 0 && i != window->height - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.middle[0]);
            }
            else if (j == window->width - 1 && i != 0 && i != window->height - 1)
            {
                AsciiWindow_SetCell(window, j, i, border.middle[2]);
            }
        }
    }
}

void AsciiWindow_DrawFill(AsciiWindow* window, uint8_t fill)
{
    for (uint32_t i = 0; i < window->height; i++)
    {
        for (uint32_t j = 0; j < window->width; j++)
        {
            AsciiWindow_SetCharacter(window, j, i, fill);
        }
    }
}

void AsciiWindow_DrawString(AsciiWindow* window, uint8_t x, uint8_t y, const char* string)
{
    uint32_t position = 0;
    while (string[position] != '\0')
    {
        AsciiWindow_SetCharacter(window, x, y, string[position]);
        x++;
        position++;
        if (x >= window->width)
        {
            x = 0;
            y++;
        }
        if (y >= window->height)
        {
            break;
        }
    }
}

uint32_t   sAudioCount    = 0;
uint32_t   sAudioMaxCount = 0;
AudioData* sAudios        = NULL;

void Audio_SetPool(AudioData* pool, size_t poolSize)
{
    sAudios        = pool;
    sAudioMaxCount = poolSize;
    sAudioCount    = 0;
}

bool Audio_Init()
{
    InitAudioDevice();
    return IsAudioDeviceReady();
}

void Audio_Deinit()
{
    CloseAudioDevice();
}

bool Audio_AddSound(Sound sound, const char* soundName)
{
    if (sAudioCount > sAudioMaxCount)
    {
        LOG_ERR("Audio: Audio_AddSound() failed, not enough space");
        return false;
    }
    sAudios[sAudioCount].sound = sound;
    sAudios[sAudioCount].id    = sAudioCount;
    strcpy(sAudios[sAudioCount].soundName, soundName);
    sAudioCount++;
    return true;
}


bool Audio_LoadAudio(const char* fileName)
{
    if (fileName == NULL)
    {
        return false;
    }
    Sound sound = LoadSound(fileName);
    if (sound.frameCount > 0)  // Texture loaded correctly(?)
    {
        return Audio_AddSound(sound, GetFileNameWithoutExt(fileName));
    }
    return false;
}

bool Audio_UnloadAudioByName(const char* audioName)
{
    if (audioName == NULL)
    {
        return false;
    }
    bool moveSounds = false;
    for (uint32_t i = 0; i < sAudioCount; i++)
    {
        if ((strcmp(audioName, sAudios[i].soundName) == 0) && moveSounds == false)
        {
            moveSounds = true;
            UnloadSound(sAudios[i].sound);
        }
        if (moveSounds && i != sAudioCount - 1)
        {
            sAudios[i] = sAudios[i + 1];
        }
    }
    if (moveSounds)
    {
        sAudioCount--;
    }
    return moveSounds;
}

void Audio_UnloadAudios()
{
    for (uint32_t i = 0; i < sAudioCount; i++)
    {
        UnloadSound(sAudios[i].sound);
    }
    sAudioCount = 0;
}

uint32_t Audio_GetCount()
{
    return sAudioCount;
}

AudioData* Audio_GetAudios()
{
    return sAudios;
}

uint32_t         sAudioPlayerCount    = 0;
uint32_t         sAudioPlayerMaxCount = 0;
AudioPlayerData* sAudioPlayers        = NULL;

void AudioPlayer_SetPool(AudioPlayerData* pool, size_t poolSize)
{
    sAudioPlayers        = pool;
    sAudioPlayerMaxCount = poolSize;
    sAudioPlayerCount    = 0;
}

int32_t AudioPlayer_PlaySoundByName(const char* audioName)
{
    if (sAudioPlayerCount >= sAudioPlayerMaxCount)
    {
        return -1;
    }
    for (uint32_t i = 0; i < Audio_GetCount(); i++)
    {
        if (strcmp(audioName, Audio_GetAudios()[i].soundName) == 0)
        {
            uint16_t currentId                     = Audio_GetAudios()[i].id;
            sAudioPlayers[sAudioPlayerCount].id    = currentId;
            sAudioPlayers[sAudioPlayerCount].sound = LoadSoundAlias(Audio_GetAudios()[i].sound);
            PlaySound(sAudioPlayers[sAudioPlayerCount].sound);
            sAudioPlayerCount++;
            return currentId;
        }
    }
    return -1;
}

int32_t AudioPlayer_PlaySoundById(uint32_t id)
{
    if (sAudioPlayerCount >= sAudioPlayerMaxCount)
    {
        return -1;
    }
    for (uint32_t i = 0; i < Audio_GetCount(); i++)
    {
        if (Audio_GetAudios()[i].id == id)
        {
            uint16_t currentId                     = Audio_GetAudios()[i].id;
            sAudioPlayers[sAudioPlayerCount].id    = currentId;
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
    for (uint32_t i = 0; i < sAudioPlayerCount; i++)
    {
        if (sAudioPlayers[i].id == id && moveAudioPlayers == false)
        {
            moveAudioPlayers = true;
            UnloadSoundAlias(sAudioPlayers[i].sound);
        }
        if (moveAudioPlayers && i != sAudioPlayerCount - 1)
        {
            sAudioPlayers[i] = sAudioPlayers[i + 1];
        }
    }
    if (moveAudioPlayers)
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

Updatable   collider2DUpdatable = { Collider2D_Update };
uint32_t    sCollider2DCount    = 0;
uint32_t    sCollider2DMaxCount = 0;
Collider2D* sCollider2DArray    = NULL;

void Collider2D_SetPool(Collider2D* pool, size_t poolSize)
{
    sCollider2DArray    = pool;
    sCollider2DCount    = 0;
    sCollider2DMaxCount = poolSize;
}

void Collider2D_Initialize(Collider2D* col)
{
    col->parent                   = 0;
    col->position.x               = 0;
    col->position.y               = 0;
    col->size.x                   = 0;
    col->size.y                   = 0;
    col->isEnabled                = true;
    col->id                       = 0;
    col->collision.collisionCount = 0;
}

bool Collider2D_Add(Collider2D* col)
{
    if (sCollider2DCount >= sCollider2DMaxCount)
    {
        LOG_ERR("Collider2D: AddCollider(), failed to add collider, not enough space");
        return false;
    }
    sCollider2DArray[sCollider2DCount] = *col;
    sCollider2DCount++;
    return true;
}

bool Collider2D_Clear()
{
    sCollider2DCount = 0;
    return true;
}

void Collider2D_Update()
{
    for (int i = 0; i < sCollider2DCount; i++)
    {
        sCollider2DArray[i].collision.collisionCount = 0;
    }

    for (int i = 0; i < sCollider2DCount; i++)
    {
        Collider2D* currentA = &sCollider2DArray[i];
#ifdef DEBUG
        Collider2D_DrawDebug(currentA);
#endif
        for (int j = 0; j < sCollider2DCount; j++)
        {
            Collider2D* currentB = &sCollider2DArray[j];
            if (currentA != currentB)
            {
                if (currentA->isEnabled && currentB->isEnabled)
                {
                    if (Collider2D_CheckCollider(currentA, currentB))
                    {
                        if (currentA->collision.collisionCount < COLLIDER2D_MAX_COLLISIONS
                            && currentB->collision.collisionCount < COLLIDER2D_MAX_COLLISIONS)
                        {
                            currentA->collision.collision[currentA->collision.collisionCount++] = currentB;
                            currentB->collision.collision[currentB->collision.collisionCount++] = currentA;
                            LOG_DBG("Collider2D: UpdateCollider(), collider %d collided with %d", currentA->id,
                                    currentB->id);
                        }
                        else
                        {
                            LOG_ERR("Collider2D: UpdateCollider(), collider %d or %d exceeded maximal collision count",
                                    currentA->id, currentB->id);
                        }
                    }
                    // Vector2 aPos = {0,0};
                    // Vector2 bPos = {0,0};
                    // Vector2 aSize = currentA->size;
                    // Vector2 bSize = currentB->size;
                    // if(currentA->parent)
                    // {
                    //     aPos = currentA->parent->position;
                    // }
                    // if(currentB->parent)
                    // {
                    //     bPos = currentB->parent->position;
                    // }
                    // aPos.x += currentA->position.x;
                    // aPos.y += currentA->position.y;
                    // bPos.x += currentB->position.x;
                    // bPos.y += currentB->position.y;
                    // if( aPos.x - (aSize.x/2.0f) < bPos.x + (bSize.x/2.0f) &&
                    //     aPos.x + (aSize.x/2.0f) > bPos.x - (bSize.x/2.0f) &&
                    //     aPos.y - (aSize.y/2.0f) < bPos.y + (bSize.y/2.0f) &&
                    //     aPos.y + (aSize.y/2.0f) > bPos.y - (bSize.y/2.0f))
                    // {
                    //     if(currentA->collision.collisionCount < COLLIDER2D_MAX_COLLISIONS &&
                    //     currentB->collision.collisionCount < COLLIDER2D_MAX_COLLISIONS)
                    //     {
                    //         currentA->collision.collision[currentA->collision.collisionCount++] = currentB;
                    //         currentB->collision.collision[currentB->collision.collisionCount++] = currentA;
                    //         LOG_DBG("Collider2D: UpdateCollider(), collider %d collided with %d", currentA->id,
                    //         currentB->id);
                    //     }
                    //     else
                    //     {
                    //         LOG_ERR("Collider2D: UpdateCollider(), collider %d or %d exceeded maximal collision
                    //         count", currentA->id, currentB->id);
                    //     }

                    // }
                }
            }
        }
    }
}

void Collider2D_DrawDebug(Collider2D* col)
{
    if (col == NULL)
    {
        LOG_ERR("Collider2D: DrawDebug(), collider is nullptr");
        return;
    }
    if (col->parent == NULL)
    {
        DrawRectangleLines(col->parent->position.x + col->position.x, col->parent->position.y + col->position.y,
                           col->size.x * col->parent->scale, col->size.y, YELLOW);
    }
    else
    {
        DrawRectangleLines(col->parent->position.x + (col->position.x * col->parent->scale),
                           col->parent->position.y + (col->position.y * col->parent->scale),
                           col->size.x * col->parent->scale, col->size.y * col->parent->scale, YELLOW);
    }
}

bool Collider2D_Check(Collider2D* a, Collider2D* b)
{
    if (a == NULL || b == NULL)
    {
        LOG_ERR("Collider2D: CheckCollision(), collider A or B is nullptr");
        return false;
    }
    if (a->collision.collisionCount > 0)
    {
        for (uint32_t i = 0; i < a->collision.collisionCount; i++)
        {
            if (a->collision.collision[i] == b)
            {
                return true;
            }
        }
        return false;
    }
    return false;
}

bool Collider2D_CheckCollider(Collider2D* a, Collider2D* b)
{
    Vector2 aPos  = { 0, 0 };
    Vector2 bPos  = { 0, 0 };
    Vector2 aSize = a->size;
    Vector2 bSize = b->size;
    if (a->parent)
    {
        aPos = { a->parent->position.x + (a->position.x * a->parent->scale),
                 a->parent->position.y + (a->position.y * a->parent->scale) };
    }
    else
    {
        aPos = a->position;
    }
    if (b->parent)
    {
        bPos = { b->parent->position.x + (b->position.x * b->parent->scale),
                 b->parent->position.y + (b->position.y * b->parent->scale) };
    }
    else
    {
        bPos = b->position;
    }
    if (aPos.x < bPos.x + bSize.x && aPos.x + aSize.x > bPos.x && aPos.y < bPos.y + bSize.y
        && aPos.y + aSize.y > bPos.y)
    {
        return true;
    }
    return false;
}

bool Collider2D_CheckPoint(Collider2D* a, Vector2 b)
{
    Vector2 aPos  = { 0, 0 };
    Vector2 aSize = a->size;
    if (a->parent)
    {
        aPos = a->parent->position;
    }
    aPos.x += a->position.x;
    aPos.y += a->position.y;
    if (aPos.x < b.x && aPos.x + aSize.x > b.x && aPos.y < b.y && aPos.y + aSize.y > b.y)
    {
        return true;
    }
    return false;
}

bool Collider2D_CheckRect(Collider2D* a, Rectangle b)
{
    Vector2 aPos  = { 0, 0 };
    Vector2 bPos  = { b.x, b.y };
    Vector2 aSize = a->size;
    Vector2 bSize = { b.width, b.height };
    if (a->parent)
    {
        aPos = a->parent->position;
    }
    aPos.x += a->position.x;
    aPos.y += a->position.y;
    if (aPos.x < bPos.x + bSize.x && aPos.x + aSize.x > bPos.x && aPos.y < bPos.y + bSize.y
        && aPos.y + aSize.y > bPos.y)
    {
        return true;
    }
    return false;
}

uint32_t Collider2D_GetCount()
{
    return sCollider2DCount;
}

Collider2D* Collider2D_GetCollider2DList()
{
    return sCollider2DArray;
}

Updatable* Collider2D_GetUpdatable()
{
    return &collider2DUpdatable;
}

uint32_t  sEntityCount    = 0;
uint32_t  sEntityMaxCount = 0;
Entity2D* sEntityList     = NULL;

void Entity2D_SetPool(Entity2D* pool, size_t poolSize)
{
    sEntityList     = pool;
    sEntityMaxCount = poolSize;
    sEntityCount    = 0;
}

void Entity2D_Initialize(Entity2D* ent)
{
    ent->id         = 0;
    ent->position.x = 0;
    ent->position.y = 0;
    ent->rotation   = 0;
    ent->scale      = 1.0f;
}

bool Entity2D_Add(Entity2D* ent)
{
    if (sEntityCount >= sEntityMaxCount)
    {
        LOG_ERR("Entity2D: Entity2D_Add() failed, not enough space");
        return false;
    }
    sEntityList[sEntityCount] = *ent;
    sEntityCount++;
    return true;
}

bool Entity2D_Clear()
{
    sEntityCount = 0;
    return true;
}

uint32_t Entitiy2D_GetCount()
{
    return sEntityCount;
}

Entity2D* Entitiy2D_GetEntityList()
{
    return sEntityList;
}

Updatable spriteUpdatable = { Sprite_Update };
uint32_t  sSpriteCount    = 0;
uint32_t  sSpriteMaxCount = 0;
Sprite*   sSpriteList     = NULL;

void Sprite_SetPool(Sprite* pool, size_t poolSize)
{
    sSpriteList     = pool;
    sSpriteMaxCount = poolSize;
    sSpriteCount    = 0;
}

void Sprite_Initialize(Sprite* spr)
{
    spr->currentTexture = NULL;
    spr->position.x     = 0;
    spr->position.y     = 0;
    spr->isVisible      = true;
    spr->rotation       = 0;
    spr->scale          = 1.0f;
    spr->zOrder         = 0;
    spr->tint           = WHITE;
    spr->parent         = NULL;
    spr->extendedDraw   = false;
    spr->portionRect    = (Rectangle){ 0, 0, 0, 0 };
}

bool Sprite_Add(Sprite* spr)
{
    if (sSpriteCount >= sSpriteMaxCount)
    {
        LOG_ERR("Sprite: Sprite_Add() failed, not enough space");
        return false;
    }
    sSpriteList[sSpriteCount] = *spr;
    sSpriteCount++;
    return true;
}

bool Sprite_Clear()
{
    sSpriteCount = 0;
    return true;
}

int Sprite_CompareFunction(const void* a, const void* b)
{
    Sprite spriteA = *(Sprite*)a;
    Sprite spriteB = *(Sprite*)b;
    return spriteA.zOrder - spriteB.zOrder;
}

void Sprite_Update()
{
    for (uint32_t i = 0; i < sSpriteCount; i++)
    {
        Sprite_Draw(&sSpriteList[i]);
    }
}

void Sprite_Draw(Sprite* spr)
{
    if (!spr->isVisible || !spr->currentTexture)
    {
        return;
    }
    Vector2 position = { 0.0f, 0.0f };
    float   scale    = 1.0f;
    float   rotation = 0.0f;
    if (spr->parent != NULL)
    {
        position.x = spr->parent->position.x;
        position.y = spr->parent->position.y;
        scale      = spr->parent->scale;
        rotation   = spr->parent->rotation;
    }
    position.x += spr->position.x * scale;
    position.y += spr->position.y * scale;
    scale *= spr->scale;
    rotation += spr->rotation;
    if (spr->extendedDraw == false)
    {
        DrawTextureEx(*spr->currentTexture, position, rotation, scale, spr->tint);
    }
    else
    {
        Rectangle destRect;
        destRect.x      = position.x;
        destRect.y      = position.y;
        destRect.width  = spr->portionRect.width * scale;
        destRect.height = spr->portionRect.height * scale;
        DrawTexturePro(*spr->currentTexture, spr->portionRect, destRect, { 0, 0 }, rotation, spr->tint);
    }
}

uint32_t Sprite_GetCount()
{
    return sSpriteCount;
}

Sprite* Sprite_GetSpriteList()
{
    return sSpriteList;
}

Updatable* Sprite_GetUpdatable()
{
    return &spriteUpdatable;
}


uint32_t     sTextureCount    = 0;
uint32_t     sTextureMaxCount = 0;
TextureData* sTextures        = NULL;

void Texture_SetPool(TextureData* pool, size_t poolSize)
{
    sTextures        = pool;
    sTextureMaxCount = poolSize;
    sTextureCount    = 0;
}

bool Texture_AddTexture(Texture2D texture, const char* textureName)
{
    if (sTextureCount > sTextureMaxCount)
    {
        LOG_ERR("Texture: Texture_AddTexture() failed, not enough space");
        return false;
        LOG_ERR("Max AnimatedSprite count reached! Cannot add more AnimatedSprites.");
    }
    sTextures[sTextureCount].texture = texture;
    strcpy(sTextures[sTextureCount].textureName, textureName);
    sTextureCount++;
    return true;
}

bool Texture_LoadTexture(const char* fileName)
{
    if (fileName == NULL)
    {
        return false;
    }
    Texture2D texture = LoadTexture(fileName);
    if (texture.id > 0)  // Texture loaded correctly(?)
    {
        return Texture_AddTexture(texture, GetFileNameWithoutExt(fileName));
    }
    return false;
}

uint8_t Texture_LoadTextureSheet(const char* fileName, uint32_t textureWidth, uint32_t textureHeight,
                                 uint32_t texturesCount)
{
    if (fileName == NULL)
    {
        return 0;
    }
    uint32_t    completed          = 0;
    Image       baseImage          = LoadImage(fileName);
    const char* fileNameWithoutExt = GetFileNameWithoutExt(fileName);
    for (uint32_t y = 0; y < baseImage.height / textureHeight; y++)
    {
        for (uint32_t x = 0; x < baseImage.width / textureWidth; x++)
        {
            Rectangle rect;
            rect.x      = x * textureWidth;
            rect.y      = y * textureHeight;
            rect.width  = textureWidth;
            rect.height = textureHeight;
            Image image = ImageFromImage(baseImage, rect);
            ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
            ImageColorReplace(&image, MAGENTA, BLANK);
            Texture texture = LoadTextureFromImage(image);
            if (texture.id > 0)  // Texture loaded correctly(?)
            {
                char name[TEXTURE_MAX_NAME];
                sprintf(name, "%s_%d", fileNameWithoutExt, (y * baseImage.height / textureHeight) + x);
                if (Texture_AddTexture(texture, name))
                {
                    completed++;
                    if (completed + 1 > texturesCount && texturesCount != 0)
                    {
                        return completed;
                    }
                }
            }
        }
    }
    return completed;
}

uint8_t Texture_LoadTextureSheetWithInfo(const char* fileName)
{
    if (fileName == NULL)
    {
        return 0;
    }
    uint32_t    completed          = 0;
    Image       baseImage          = LoadImage(fileName);
    const char* fileNameWithoutExt = GetFileNameWithoutExt(fileName);
    char        fileTextureInfo[TEXTURE_INFO_FILE_MAX_NAME];
    sprintf(fileTextureInfo, "resources/sprites/otsp_tiles_01.txt");
    LOG_INF("Texture: Texture_LoadTextureSheetWithInfo(), Loading texture info from %s", fileTextureInfo);
    FILE* file;
    file = fopen(fileTextureInfo, "r");
    if (file == NULL)
    {
        LOG_ERR("Texture: Texture_LoadTextureSheetWithInfo() failed to open %s", fileTextureInfo);
        return 0;
    }
    char line[TEXTURE_INFO_LINE_MAX];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        char     textureName[TEXTURE_MAX_NAME];
        uint32_t textureWidth;
        uint32_t textureHeight;
        uint32_t texturePosX;
        uint32_t texturePosY;
        if (sscanf(line, "%s %d %d %d %d", textureName, &textureWidth, &textureHeight, &texturePosX, &texturePosY) != 5)
        {
            LOG_ERR("Texture: Texture_LoadTextureSheetWithInfo() failed to read line from %s.txt", fileNameWithoutExt);
            fclose(file);
            return completed;
        }
        Rectangle rect;
        rect.x      = texturePosX;
        rect.y      = texturePosY;
        rect.width  = textureWidth;
        rect.height = textureHeight;
        Image image = ImageFromImage(baseImage, rect);
        ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        ImageColorReplace(&image, MAGENTA, BLANK);
        Texture texture = LoadTextureFromImage(image);
        if (texture.id > 0)  // Texture loaded correctly(?)
        {
            char name[TEXTURE_MAX_NAME];
            sprintf(name, "%s", textureName);
            if (Texture_AddTexture(texture, name))
            {
                completed++;
            }
        }
    }
    fclose(file);
    if (completed == 0)
    {
        LOG_ERR("Texture: Texture_LoadTextureSheetWithInfo() failed to load any textures from %s", fileTextureInfo);
    }
    return completed;
}

bool Texture_UnloadTextureByName(const char* textureName)
{
    if (textureName == NULL)
    {
        return false;
    }
    bool moveTextures = false;
    for (uint32_t i = 0; i < sTextureCount; i++)
    {
        if ((strcmp(textureName, sTextures[i].textureName) == 0) && moveTextures == false)
        {
            moveTextures = true;
            UnloadTexture(sTextures[i].texture);
        }
        if (moveTextures && i != sTextureCount - 1)
        {
            sTextures[i] = sTextures[i + 1];
        }
    }
    if (moveTextures)
    {
        sTextureCount--;
    }
    return moveTextures;
}

bool Texture_UnloadTextureById(uint32_t textureId)
{
    bool moveTextures = false;
    for (uint32_t i = 0; i < sTextureCount; i++)
    {
        if (sTextures[i].texture.id == textureId && moveTextures == false)
        {
            moveTextures = true;
            UnloadTexture(sTextures[i].texture);
        }
        if (moveTextures && i != sTextureCount - 1)
        {
            sTextures[i] = sTextures[i + 1];
        }
    }
    if (moveTextures)
    {
        sTextureCount--;
    }
    return moveTextures;
}

void Texture_UnloadTextures()
{
    for (uint32_t i = 0; i < sTextureCount; i++)
    {
        UnloadTexture(sTextures[i].texture);
    }
    sTextureCount = 0;
}

uint32_t Texture_GetCount()
{
    return sTextureCount;
}

TextureData* Texture_GetTextures()
{
    return sTextures;
}

Texture2D* Texture_GetTextureByName(const char* textureName)
{
    for (uint32_t i = 0; i < sTextureCount; i++)
    {
        if (strcmp(sTextures[i].textureName, textureName) == 0)
        {
            return &sTextures[i].texture;
        }
    }
    LOG_ERR("Texture: Texture_GetTextureByName() failed, texture of name `%s` not found", textureName);
    return NULL;
}

Texture2D* Texture_GetTextureById(uint32_t textureId)
{
    for (uint32_t i = 0; i < sTextureCount; i++)
    {
        if (sTextures[i].texture.id == textureId)
        {
            return &sTextures[i].texture;
        }
    }
    LOG_ERR("Texture: Texture_GetTextureByName() failed, texture of id `%d` not found", textureId);
    return NULL;
}

Texture2D* Texture_GetTextureByPosition(uint32_t index)
{
    if (index - 1 > sTextureCount)
    {
        LOG_ERR("Texture: Texture_GetTextureByPosition() failed, texture at position `%d` not found", index);
        return NULL;
    }
    return &sTextures[index].texture;
}
