#include "ash_components.h"

#include "ash_debug.h"
#include "ash_misc.h"
#include "raylib.h"

#include <cstring>
#include <stdio.h>

void AnimatedSprite_Initialize(AnimatedSprite* animatedSprite)
{
    animatedSprite->frameTime        = ANIMATEDSPRITE_DEFAULT_ANIMATION_SPEED;
    animatedSprite->currentAnimation = NULL;
    animatedSprite->isPlaying        = false;
    animatedSprite->repeat           = false;
    animatedSprite->currentFrame     = 0;
    Sprite_Initialize(&animatedSprite->sprite);
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

void AnimatedSprite_Update(AnimatedSprite* animatedSprite)
{
    if (animatedSprite->isPlaying && animatedSprite->currentAnimation && Stopwatch_IsZero(&animatedSprite->stopwatch))
    {
        animatedSprite->sprite.currentTexture =
            animatedSprite->currentAnimation->animationFrames[animatedSprite->currentFrame++];
        if (animatedSprite->currentFrame >= animatedSprite->currentAnimation->animationFrameCount)
        {
            if (animatedSprite->repeat)
            {
                animatedSprite->currentFrame = 0;
            }
            else
            {
                animatedSprite->isPlaying = false;
            }
        }
        Stopwatch_Start(&animatedSprite->stopwatch, animatedSprite->frameTime);
    }
}

void AsciiWindow_Initalize(AsciiWindow* window, Texture2D texture)
{
    AsciiWindow_Clear(window);
    for (uint32_t i = 0; i < window->height * window->width; i++)
    {
        Sprite_Initialize(&window->spriteBuffer[i]);
    }
    window->spriteHeight = window->textureBuffer[0]->size.y;
    window->spriteWidth  = window->textureBuffer[0]->size.x;
    for (uint32_t i = 0; i < window->height; i++)
    {
        for (uint32_t j = 0; j < window->width; j++)
        {
            window->spriteBuffer[i * window->width + j].position.x = window->textureBuffer[0]->size.x
                                                                     * window->spriteBuffer[i * window->width + j].scale
                                                                     * window->entity.scale * j;
            window->spriteBuffer[i * window->width + j].position.y = window->textureBuffer[0]->size.y
                                                                     * window->spriteBuffer[i * window->width + j].scale
                                                                     * window->entity.scale * i;
            window->spriteBuffer[i * window->width + j].currentTexture = window->textureBuffer[0];
            window->spriteBuffer[i * window->width + j].parent         = &window->entity;
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

bool Audio_Init()
{
    InitAudioDevice();
    return IsAudioDeviceReady();
}

void Audio_Deinit()
{
    CloseAudioDevice();
}

AudioData Audio_LoadAudio(const char* fileName)
{
    if (fileName == NULL)
    {
        return (AudioData){ 0 };
    }
    Sound     sound = LoadSound(fileName);
    AudioData audio;
    audio.sound = sound;
    return audio;
}

void Audio_UnloadAudio(AudioData* audio)
{
    UnloadSound(audio->sound);
}

void AudioPlayer_Stop(AudioData* audio)
{
    StopSound(audio->sound);
}

void AudioPlayer_Start(AudioData* audio)
{
    PlaySound(audio->sound);
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

void Entity2D_Initialize(Entity2D* ent)
{
    ent->id         = 0;
    ent->position.x = 0;
    ent->position.y = 0;
    ent->rotation   = 0;
    ent->scale      = 1.0f;
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
    spr->portionRect    = (Rectangle){ 0, 0, 0, 0 };
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

    Rectangle sourceRect;
    Rectangle destRect;

    sourceRect.x      = spr->currentTexture->uv.x;
    sourceRect.y      = spr->currentTexture->uv.y;
    sourceRect.width  = spr->currentTexture->uv.w;
    sourceRect.height = spr->currentTexture->uv.h;

    destRect.x      = position.x;
    destRect.y      = position.y;
    destRect.width  = 1.0;
    destRect.height = 1.0;

    DrawTexturePro(spr->currentTexture->texture, sourceRect, destRect, { 0, 0 }, rotation, spr->tint);
    // TODO: Bring back portion drawing
    //      Rectangle destRect;
    //      destRect.x      = position.x;
    //      destRect.y      = position.y;
    //      destRect.width  = spr->portionRect.width * scale;
    //      destRect.height = spr->portionRect.height * scale;
    //      // DrawTexturePro(*spr->currentTexture, spr->portionRect, destRect, { 0, 0 }, rotation, spr->tint);
}

TextureData Texture_LoadTexture(const char* fileName)
{
    if (fileName == NULL)
    {
        return (TextureData){ 0 };
    }
    Texture2D   texture = LoadTexture(fileName);
    TextureData textureData;
    textureData.texture = texture;
    textureData.uv      = { 0.0f, 0.0f, 1.0f, 1.0f };
    textureData.size    = { (int16_t)texture.width, (int16_t)texture.height };
    return textureData;
}

bool Texture_CreateTextureAtlas(TextureData texture, uint32_t columns, uint32_t rows, TextureData* output)
{
    if (output == NULL)
    {
        LOG_ERR("Texture: Texture_LoadTextureAtlas() failed, output is nullptr");
        return false;
    }
    for (int i = 0; i < columns; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            Rectangle portionRect;
            portionRect.x      = ((float)texture.texture.width / (float)columns) * i;
            portionRect.y      = ((float)texture.texture.height / (float)rows) * j;
            portionRect.width  = (float)texture.texture.width / (float)columns;
            portionRect.height = (float)texture.texture.height / (float)rows;
            TextureData textureData;
            textureData.texture = texture.texture;
            textureData.uv.x    = portionRect.x / texture.texture.width;
            textureData.uv.y    = portionRect.y / texture.texture.height;
            textureData.uv.w    = portionRect.width / texture.texture.width;
            textureData.uv.h    = portionRect.height / texture.texture.height;
            textureData.size.x  = portionRect.width;
            textureData.size.y  = portionRect.height;

            output[j * columns + i] = textureData;
        }
    }
    return true;
}

void Texture_UnloadTexture(TextureData* textureData)
{
    UnloadTexture(textureData->texture);
}
