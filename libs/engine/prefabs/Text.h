#ifndef PREFABS_TEXT_H
#define PREFABS_TEXT_H
#include "context/Updatable.h"
#include "engine/components/Entity2D.h"
#include "engine/prefabs/Font.h"
#define MAX_TEXT_LENGTH 32

enum TextAlign
{
    TEXT_CENTER = 0,
    TEXT_LEFT,
    TEXT_RIGHT,
};

enum TextWrap
{
    TEXT_WRAP = 0,
    TEXT_NOWRAP,
};

typedef struct Text
{
    Entity2D* parent;
    Vector2   position;
    Vector2   size;
    char      buffer[MAX_TEXT_LENGTH];
    Font*     font;
    TextAlign align;
    TextWrap  wrap;
} Text;

void Text_Initialize(Text* text);
void Text_Add(Text* text);
bool Text_Clear();
void Text_Update();

uint8_t    Text_GetCount();
Text**     Text_GetTexts();
Updatable* Text_GetUpdatable();
#endif
