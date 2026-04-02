#ifndef ASH_COMPONENTS_H
#define ASH_COMPONENTS_H

#include "ash_misc.h"

#include <stdint.h>

/* Defines */
#define ANIMATEDSPRITE_MAX_FRAMES              32
#define ASCIIWINDOW_MAX_TEXURES                256
#define ANIMATEDSPRITE_DEFAULT_ANIMATION_SPEED 33
#define ASCIIWINDOW_ASCII_START                0
#define AUDIO_MAX_NAME                         32
#define COLLIDER2D_MAX_COUNT                   16
#define COLLIDER2D_MAX_COLLISIONS              16
#define TEXTURE_INFO_FILE_MAX_NAME             64
#define TEXTURE_INFO_LINE_MAX                  128

/* Structs, Enums, and Unions */
typedef struct Entity2D
{
    Vector2Float position;
    float        scale;
    float        rotation;
    uint8_t      id;
} Entity2D;

typedef struct TextureData TextureData;
typedef struct Sprite
{
    Entity2D*    parent;
    Vector2Float position;
    float        scale;
    float        rotation;
    TextureData* currentTexture;
    uint8_t      zOrder;
    bool         isVisible;
    Color        tint;
    bool         drawPortion;
    Rectangle    portionRect;
} Sprite;

typedef struct AnimationData
{
    TextureData* animationFrames[ANIMATEDSPRITE_MAX_FRAMES];
    uint8_t      animationFrameCount;
} AnimationData;

typedef struct AnimatedSprite
{
    Sprite         sprite;
    AnimationData* currentAnimation;
    uint8_t        id;
    uint16_t       frameTime;
    bool           isPlaying;
    bool           repeat;
    uint8_t        currentFrame;
    Stopwatch      stopwatch;
} AnimatedSprite;

typedef struct AsciiWindow
{
    Entity2D     entity;
    uint8_t*     windowBuffer;
    Sprite*      spriteBuffer;
    TextureData* textureBuffer[ASCIIWINDOW_MAX_TEXURES];
    Vector2Float position;
    uint32_t     width;
    uint32_t     height;
    uint32_t     spriteWidth;
    uint32_t     spriteHeight;
} AsciiWindow;

typedef struct AsciiSubWindow
{
    AsciiWindow* parent;
    Vector2Float position;
    uint32_t     width;
    uint32_t     height;
} AsciiSubWindow;

typedef struct AsciiWindowBorder
{
    uint32_t top[3];
    uint32_t middle[3];
    uint32_t bottom[3];
} AsciiWindowBorder;

typedef struct AudioData
{
    Sound sound;
} AudioData;

typedef struct AudioPlayerData
{
    Sound sound;
} AudioPlayerData;

typedef struct Collider2D Collider2D;

typedef struct Collision2D
{
    Collider2D* collision[COLLIDER2D_MAX_COUNT];
    uint8_t     collisionCount;
} Collision2D;

typedef struct Collider2D
{
    Entity2D*    parent;
    Vector2Float position;
    Vector2Float size;
    bool         isEnabled;
    bool         isTrigger;
    uint8_t      id;
    Collision2D  collision;
} Collider2D;

typedef struct TextureData
{
    Texture2D    texture;
    Vector4Float uv;
    Vector2UInt  size;
} TextureData;

/* Function Prototypes */

void AnimatedSprite_Initialize(AnimatedSprite* animatedSprite);
void AnimatedSprite_Play(AnimatedSprite* animatedSprite, AnimationData* animation, bool repeat);
void AnimatedSprite_Stop(AnimatedSprite* animatedSprite);
void AnimatedSprite_Update(AnimatedSprite* animatedSprite);

void     AsciiWindow_Initalize(AsciiWindow* window, Texture2D* texture);
void     AsciiWindow_SetCharacter(AsciiWindow* window, uint32_t x, uint32_t y, char c);
char     AsciiWindow_GetCharacter(AsciiWindow* window, uint32_t x, uint32_t y);
void     AsciiWindow_SetCell(AsciiWindow* window, uint32_t x, uint32_t y, uint32_t c);
uint32_t AsciiWindow_GetCell(AsciiWindow* window, uint32_t x, uint32_t y);
void     AsciiWindow_Clear(AsciiWindow* window);
void     AsciiWindow_Draw(AsciiWindow* window);

void AsciiWindow_DrawBorder(AsciiWindow* window, AsciiWindowBorder border);
void AsciiWindow_DrawFill(AsciiWindow* window, uint8_t fill);
void AsciiWindow_DrawString(AsciiWindow* window, uint8_t x, uint8_t y, const char* string);

void     AsciiSubWindow_Initalize(AsciiSubWindow* window);
void     AsciiSubWindow_SetCell(AsciiSubWindow* window, uint32_t x, uint32_t y, uint32_t c);
uint32_t AsciiSubWindow_GetCell(AsciiSubWindow* window, uint32_t x, uint32_t y);
void     AsciiSubWindow_SetCharacter(AsciiSubWindow* window, uint32_t x, uint32_t y, char c);
char     AsciiSubWindow_GetCharacter(AsciiSubWindow* window, uint32_t x, uint32_t y);
void     AsciiSubWindow_Clear(AsciiSubWindow* window);

void AsciiSubWindow_DrawBorder(AsciiSubWindow* window, AsciiWindowBorder border);
void AsciiSubWindow_DrawFill(AsciiSubWindow* window, uint32_t fill);
void AsciiSubWindow_DrawString(AsciiSubWindow* window, const char* string);

bool      Audio_Init();
void      Audio_Deinit();
AudioData Audio_LoadAudio(const char* fileName);
void      Audio_UnloadAudio(AudioData* audio);
void      Audio_Play(AudioData* audio);
void      Audio_Stop(AudioData* audio);


void Collider2D_Initialize(Collider2D* col);
void Collider2D_DrawDebug(Collider2D* col);
bool Collider2D_Check(Collider2D* a, Collider2D* b);
bool Collider2D_CheckCollider(Collider2D* a, Collider2D* b);
bool Collider2D_CheckPoint(Collider2D* a, Vector2 b);
bool Collider2D_CheckRect(Collider2D* a, Rectangle b);

void Entity2D_Initialize(Entity2D* ent);

void Sprite_Initialize(Sprite* spr);
void Sprite_Update(Sprite* spr);
void Sprite_Draw(Sprite* spr);

TextureData Texture_LoadTexture(const char* fileName);
bool        Texture_CreateTextureAtlas(TextureData texture, uint32_t columns, uint32_t rows, TextureData* output);
void        Texture_UnloadTexture(TextureData* texture);

#endif  // ASH_COMPONENTS_H
