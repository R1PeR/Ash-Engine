#ifndef LIBS_UTILS_UI_H
#define LIBS_UTILS_UI_H
#include "ashes/ash_components.h"
#include "ashes/ash_misc.h"

#include <cstdint>

struct Button
{
    Vector2Float position;
    TextureData* onTexture;
    TextureData* offTexture;
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
    TextureData* onTexture;
    TextureData* offTexture;
    Rectangle    bounds;
    float        scale;
    bool         isPressed;
    Text         text;
};

struct SliderFloat
{
    Vector2Float position;
    TextureData* backgroundSprite;
    TextureData* sliderTexture;
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
    TextureData* backgroundTexture;
    TextureData* sliderTexture;
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
    TextureData* backgroundTexture;
    TextureData* progressTexture;
    Rectangle    bounds;
    float        scale;
    float        minValue;
    float        maxValue;
    float        currentValue;
};

struct Checkbox
{
    Vector2Float position;
    TextureData* onTexture;
    TextureData* offTexture;
    Rectangle    bounds;
    float        scale;
    bool         isChecked;
};

struct ItemSlot
{
    Vector2Float position;
    TextureData* backgroundTexture;
    TextureData* itemTexture;
    Rectangle    bounds;
    float        scale;
};

void UI_Initialize(Entity2D* parentEntity, Sprite* spriteArray, size_t* spriteArraySize, size_t spriteArrayMaxSize);
bool UI_TextureButton(Button* uiButton);
bool UI_TextButton(TextButton* uiTextButton, TextureData* fontAtlas);
bool UI_Text(Text* uiText, TextureData* fontAtlas);
bool UI_SliderFloat(SliderFloat* uiSlider);
bool UI_SliderInt(SliderInt* uiSlider);
bool UI_ProgressBar(ProgressBar* uiProgressBar);
bool UI_Checkbox(Checkbox* uiCheckbox);
bool UI_ItemSlot(ItemSlot* uiItemSlot);
void UI_Clear();

#endif  // LIBS_UTILS_UI_H
