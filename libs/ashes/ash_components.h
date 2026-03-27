#ifndef ASH_COMPONENTS_H
#define ASH_COMPONENTS_H

/* Includes */
#include "ash_misc.h"

#include <stdint.h>

/* Defines */
#define ANIMATEDSPRITE_MAX_FRAMES  32
#define ANIMATEDSPRITE_MAX_COUNT   32
#define ASCIIWINDOW_MAX_TEXURES    256
#define ASCIIWINDOW_ASCII_START    0
#define AUDIO_MAX_COUNT            1024
#define AUDIO_MAX_NAME             32
#define AUDIOPLAYER_MAX_COUNT      128
#define AUDIOPLAYER_MAX_NAME       32
#define COLLIDER2D_MAX_COUNT       16
#define COLLIDER2D_MAX_COLLISIONS  16
#define COLLIDER2D_SIMPLE_CHECK    true
#define NAVIGATIONMAP2D_MAX_SIZE   64
#define NAVIGATIONMAP2D_MAX_PATH   64
#define TEXTURE_MAX_COUNT          1024
#define TEXTURE_MAX_NAME           32
#define TEXTURE_ALPHA_COLOR        { 0xff, 0xff, 0x00, 0xff }
#define TEXTURE_INFO_FILE_MAX_NAME 64
#define TEXTURE_INFO_LINE_MAX      128

/* Structs, Enums, and Unions */
typedef struct Entity2D
{
    Vector2   position;
    float     scale;
    float     rotation;
    uint8_t   id;
    Entity2D* next;
} Entity2D;

typedef struct Sprite
{
    Entity2D*  parent;
    Vector2    position;
    float      scale;
    float      rotation;
    Texture2D* currentTexture;
    uint8_t    zOrder;
    bool       isVisible;
    Color      tint;

    bool      extendedDraw;
    Rectangle portionRect;
    // Rectangle  destRect;
    // Vector2    origin;
    // Sprite*    next;
} Sprite;

typedef struct AnimationData
{
    Texture2D* animationFrames[ANIMATEDSPRITE_MAX_FRAMES];
    uint8_t    animationFrameCount;
} AnimationData;

typedef struct AnimatedSprite
{
    Sprite          sprite;
    AnimationData*  currentAnimation;
    uint8_t         id;
    uint16_t        frameTime;
    bool            isPlaying;
    bool            repeat;
    uint8_t         currentFrame;
    Stopwatch       stopwatch;
    AnimatedSprite* next;
} AnimatedSprite;

typedef struct AsciiWindow
{
    Entity2D entity;
    uint8_t* windowBuffer;
    Sprite*  spriteBuffer;
    Texture* textureBuffer[ASCIIWINDOW_MAX_TEXURES];
    Vector2  position;
    uint32_t width;
    uint32_t height;
    uint32_t spriteWidth;
    uint32_t spriteHeight;
} AsciiWindow;

typedef struct AsciiSubWindow
{
    AsciiWindow* parent;
    Vector2      position;
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
    Sound    sound;
    char     soundName[AUDIO_MAX_NAME];
    uint32_t id;
} AudioData;

typedef struct AudioPlayerData
{
    Sound    sound;
    uint32_t id;
} AudioPlayerData;

typedef struct Collider2D Collider2D;

typedef struct Collision2D
{
    Collider2D* collision[COLLIDER2D_MAX_COUNT];
    uint8_t     collisionCount;
} Collision2D;

typedef struct Collider2D
{
    Entity2D*   parent;
    Vector2     position;
    Vector2     size;
    bool        isEnabled;
    bool        isTrigger;
    uint8_t     id;
    Collision2D collision;
    Collider2D* next;
} Collider2D;

typedef struct NavigationMap2DPath
{
    Vector2 path[NAVIGATIONMAP2D_MAX_PATH];
    uint8_t count;
} NavigationMap2DPath;

typedef struct NavigationMap2D
{
    // can make optimalization here to make 8bit hold 8 squares
    uint32_t square[NAVIGATIONMAP2D_MAX_SIZE][NAVIGATIONMAP2D_MAX_SIZE];  // 0 - free, other - entity id
    float    sNavigation2DSquareSize;
} NavigationMap2D;

typedef struct TextureData
{
    Texture2D texture;
    char      textureName[TEXTURE_MAX_NAME];
} TextureData;

/* Function Prototypes */

void AnimatedSprite_Initialize(AnimatedSprite* animatedSprite);
bool AnimatedSprite_Add(AnimatedSprite* animatedSprite);
bool AnimatedSprite_Clear();
void AnimatedSprite_Play(AnimatedSprite* animatedSprite, AnimationData* animation, bool repeat);
void AnimatedSprite_Stop(AnimatedSprite* animatedSprite);
void AnimatedSprite_Update();

bool AnimatedSprite_SetAnimationDataFromTextureSheet(AnimationData* data, const char* textureName, uint8_t startFrame,
                                                     uint8_t frameCount);

uint32_t        AnimatedSprite_GetCount();
AnimatedSprite* AnimatedSprite_GetAnimatedSpriteList();
Updatable*      AnimatedSprite_GetUpdatable();

void     AsciiWindow_Initalize(AsciiWindow* window, const char* textureName);
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

bool Audio_Init();
void Audio_Deinit();
bool Audio_LoadAudio(const char* fileName);
bool Audio_UnloadAudioByName(const char* audioName);
void Audio_UnloadAudios();

uint32_t   Audio_GetCount();
AudioData* Audio_GetAudios();

int32_t AudioPlayer_PlaySoundByName(const char* audioName);
int32_t AudioPlayer_PlaySoundById(uint32_t id);
bool    AudioPlayer_StopSoundById(uint32_t id);
void    AudioPlayer_StopAll();

void Collider2D_Initialize(Collider2D* col);
bool Collider2D_Add(Collider2D* col);
bool Collider2D_Clear();
void Collider2D_Update();
void Collider2D_DrawDebug(Collider2D* col);
bool Collider2D_Check(Collider2D* a, Collider2D* b);
bool Collider2D_CheckCollider(Collider2D* a, Collider2D* b);
bool Collider2D_CheckPoint(Collider2D* a, Vector2 b);
bool Collider2D_CheckRect(Collider2D* a, Rectangle b);

uint32_t    Collider2D_GetCount();
Collider2D* Collider2D_GetCollider2DList();
Updatable*  Collider2D_GetUpdatable();

void Entity2D_Initialize(Entity2D* ent);
bool Entity2D_Add(Entity2D* ent);
bool Entity2D_Clear();

uint32_t  Entitiy2D_GetCount();
Entity2D* Entitiy2D_GetEntityList();

void                NavigationMap2D_Initialize(NavigationMap2D* map, float squareSize);
uint32_t            NavigationMap2D_GetPosition(NavigationMap2D* map, uint8_t x, uint8_t y);
void                NavigationMap2D_SetPosition(NavigationMap2D* map, uint8_t x, uint8_t y, uint32_t state);
void                NavigationMap2D_Clear(NavigationMap2D* map);
void                NavigationMap2D_Fill(NavigationMap2D* map, Collider2D* entity);
NavigationMap2DPath NavigationMap2D_CalculatePath(NavigationMap2D* map, uint8_t startX, uint8_t startY, uint8_t stopX,
                                                  uint8_t stopY);
void                NavigationMap2D_Debug(NavigationMap2D* map);
Vector2             NavigationMap2D_ConvertWorldToMap(NavigationMap2D* map, Vector2 worldPosition);
Vector2             NavigationMap2D_ConvertMapToWorld(NavigationMap2D* map, Vector2 mapPosition);

void Sprite_SetPool(Sprite* pool, size_t poolSize);
void Sprite_Initialize(Sprite* spr);
bool Sprite_Add(Sprite* spr);
bool Sprite_Clear();
void Sprite_Update();
void Sprite_Draw(Sprite* spr);

uint32_t   Sprite_GetCount();
Sprite*    Sprite_GetSpriteList();
Updatable* Sprite_GetUpdatable();

bool    Texture_LoadTexture(const char* fileName);
uint8_t Texture_LoadTextureSheet(const char* fileName, uint32_t textureWidth, uint32_t textureHeight,
                                 uint32_t texturesCount);
uint8_t Texture_LoadTextureSheetWithInfo(const char* fileName);

bool Texture_UnloadTextureByName(const char* textureName);
bool Texture_UnloadTextureById(uint32_t textureId);
void Texture_UnloadTextures();

uint32_t     Texture_GetCount();
TextureData* Texture_GetTextures();
Texture2D*   Texture_GetTextureByName(const char* textureName);
Texture2D*   Texture_GetTextureById(uint32_t textureId);

#endif  // ASH_COMPONENTS_H
