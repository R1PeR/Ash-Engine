#ifndef PREFABS_FONT_H
#define PREFABS_FONT_H
#include "engine/components/Texture.h"

typedef struct Font
{
    Texture* texture;
    Vector2  charSize;
    uint8_t  asciiStart;
} Font;
#endif
