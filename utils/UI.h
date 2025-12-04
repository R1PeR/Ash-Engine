#ifndef LIBS_UTILS_UI_H
#define LIBS_UTILS_UI_H
#include "libs/engine/components/Sprite.h"
#include "libs/engine/components/Texture.h"
#include "libs/engine/misc/Utils.h"

#include <cstdint>

struct Button
{
    Vector2Float position;
    Texture2D*   onTexture;
    Texture2D*   offTexture;
    Rectangle    bounds;
    float        scale;
    bool         isPressed;
};

struct Text
{
    Vector2Float position;
    Rectangle    bounds;
    float        scale;
    char*        buffer;
    size_t       bufferSize;
};

struct TextButton
{
    Vector2Float position;
    Texture2D*   onTexture;
    Texture2D*   offTexture;
    Rectangle    bounds;
    float        scale;
    bool         isPressed;
    char*        buffer;
    size_t       bufferSize;
};

struct SliderFloat
{
    Vector2Float position;
    Texture2D*   backgroundSprite;
    Texture2D*   sliderSprite;
    Rectangle    bounds;
    float        scale;
    float        minValue;
    float        maxValue;
    float        currentValue;
    bool         isDragging;
};

struct SliderInt
{
    Vector2Float position;
    Texture2D*   background;
    Texture2D*   slider;
    Rectangle    bounds;
    float        scale;
    int32_t      minValue;
    int32_t      maxValue;
    int32_t      currentValue;
    bool         isDragging;
};

struct ProgressBar
{
    Vector2Float position;
    Texture2D*   background;
    Texture2D*   slider;
    Rectangle    bounds;
    float        scale;
    float        minValue;
    float        maxValue;
    float        currentValue;
};

struct Checkbox
{
    Vector2Float position;
    Sprite       sprite;
    Texture2D*   onTexture;
    Texture2D*   offTexture;
    Rectangle    bounds;
    float        scale;
    bool         isChecked;
};

struct ItemSlot
{
    Vector2Float position;
    Texture2D*   backgroundSprite;
    Texture2D*   itemSprite;
    Rectangle    bounds;
    float        scale;
};

void UI_Init(Entity2D* parentEntity);
bool UI_TextureButton(Button* uiButton);
bool UI_TextButton(TextButton* uiTextButton);
bool UI_Text(Text* uiText, const char* fontName);
bool UI_SliderFloat(SliderFloat* uiSlider);
bool UI_SliderInt(SliderInt* uiSlider);
bool UI_ProgressBar(ProgressBar* uiProgressBar);
bool UI_Checkbox(Checkbox* uiCheckbox);
bool UI_ItemSlot(ItemSlot* uiItemSlot);

#endif  // LIBS_UTILS_UI_H
