#include "UI.h"

#include "ashes/ash_components.h"
#include "ashes/ash_io.h"

#include <stdio.h>
Entity2D* uiParentEntity       = NULL;
Sprite*   uiSpriteArray        = NULL;
size_t    uiSpriteArraySize    = 0;
size_t    uiSpriteArrayMaxSize = 0;

bool UI_CheckBounds(Rectangle bounds)
{
    Camera2D* camera   = Window_GetCamera();
    Vector2   mousePos = { (float)(Input_GetMouseX()), (float)(Input_GetMouseY()) };
    Vector2   worldPos = GetScreenToWorld2D(mousePos, *camera);

#if 1
    DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, RED);
#endif

    if (Utils_PointInRectangle(worldPos, bounds))
    {
        return true;
    }
    return false;
}

void UI_Init(Entity2D* parentEntity, Sprite* spriteArray, size_t spriteArraySize)
{
    uiParentEntity       = parentEntity;
    uiSpriteArray        = spriteArray;
    uiSpriteArraySize    = 0;
    uiSpriteArrayMaxSize = spriteArraySize;
}

bool UI_TextureButton(Button* button)
{
    button->bounds.x *= uiParentEntity->scale;
    button->bounds.y *= uiParentEntity->scale;
    button->bounds.width *= uiParentEntity->scale;
    button->bounds.height *= uiParentEntity->scale;

    button->bounds.x += uiParentEntity->position.x;
    button->bounds.y += uiParentEntity->position.y;

    Sprite buttonSprite;
    Sprite_Initialize(&buttonSprite);
    buttonSprite.scale      = button->scale;
    buttonSprite.position.x = button->position.x;
    buttonSprite.position.y = button->position.y;
    buttonSprite.parent     = uiParentEntity;
    if (button->isPressed)
    {
        buttonSprite.currentTexture = button->onTexture;
    }
    else
    {
        buttonSprite.currentTexture = button->offTexture;
    }
    if (!Utils_AddToArray(uiSpriteArray, buttonSprite, uiSpriteArraySize, uiSpriteArrayMaxSize))
    {
        LOG_ERR("UI sprite array is full! Cannot add more UI elements.");
    }

    return UI_CheckBounds(button->bounds);
}

bool UI_TextButton(TextButton* textButton, TextureData* fontAtlas)
{
    textButton->bounds.x *= uiParentEntity->scale;
    textButton->bounds.y *= uiParentEntity->scale;
    textButton->bounds.width *= uiParentEntity->scale;
    textButton->bounds.height *= uiParentEntity->scale;

    textButton->bounds.x += uiParentEntity->position.x;
    textButton->bounds.y += uiParentEntity->position.y;

    Sprite buttonSprite;
    Sprite_Initialize(&buttonSprite);
    buttonSprite.scale      = textButton->scale;
    buttonSprite.position.x = textButton->position.x;
    buttonSprite.position.y = textButton->position.y;
    buttonSprite.parent     = uiParentEntity;
    if (textButton->isPressed)
    {
        buttonSprite.currentTexture = textButton->onTexture;
    }
    else
    {
        buttonSprite.currentTexture = textButton->offTexture;
    }
    if (!Utils_AddToArray(uiSpriteArray, buttonSprite, uiSpriteArraySize, uiSpriteArrayMaxSize))
    {
        LOG_ERR("UI sprite array is full! Cannot add more UI elements.");
    }

    UI_Text(&textButton->text, fontAtlas);

    return UI_CheckBounds(textButton->bounds);
}

bool UI_Text(Text* uiText, TextureData* fontAtlas)
{
    uiText->bounds.x *= uiParentEntity->scale;
    uiText->bounds.y *= uiParentEntity->scale;
    uiText->bounds.width *= uiParentEntity->scale;
    uiText->bounds.height *= uiParentEntity->scale;

    uiText->bounds.x += uiParentEntity->position.x;
    uiText->bounds.y += uiParentEntity->position.y;

    for (size_t i = 0; i < uiText->bufferSize; i++)
    {
        Sprite textSprite;
        Sprite_Initialize(&textSprite);
        char c = uiText->buffer[i];
        char textureName[32];
        // sprintf(textureName, "%s_%d", fontName, c);
        TextureData* charTexture  = &fontAtlas[c - 32];
        textSprite.currentTexture = charTexture;
        textSprite.scale          = uiText->scale;
        textSprite.position.x     = uiText->position.x + (charTexture->size.x * uiText->scale * i);
        textSprite.position.y     = uiText->position.y;
        textSprite.parent         = uiParentEntity;
        // Sprite_Add(&textSprite);
    }

    return UI_CheckBounds(uiText->bounds);
}

bool UI_SliderFloat(SliderFloat* slider)
{
    slider->bounds.x *= uiParentEntity->scale;
    slider->bounds.y *= uiParentEntity->scale;
    slider->bounds.width *= uiParentEntity->scale;
    slider->bounds.height *= uiParentEntity->scale;

    slider->bounds.x += uiParentEntity->position.x;
    slider->bounds.y += uiParentEntity->position.y;

    if (slider->backgroundSprite != NULL)
    {
        Sprite backgroundSprite;
        Sprite_Initialize(&backgroundSprite);
        backgroundSprite.currentTexture = slider->backgroundSprite;
        backgroundSprite.scale          = slider->scale;
        backgroundSprite.position.x     = slider->position.x;
        backgroundSprite.position.y     = slider->position.y;
        backgroundSprite.parent         = uiParentEntity;
        if (!Utils_AddToArray(uiSpriteArray, backgroundSprite, uiSpriteArraySize, uiSpriteArrayMaxSize))
        {
            LOG_ERR("UI sprite array is full! Cannot add more UI elements.");
        }
    }

    Sprite sliderSprite;
    Sprite_Initialize(&sliderSprite);
    sliderSprite.currentTexture = slider->sliderTexture;
    sliderSprite.scale          = slider->scale;
    sliderSprite.position.x     = slider->position.x
                              + ((slider->currentValue - slider->minValue) / (slider->maxValue - slider->minValue))
                                    * slider->backgroundSprite->size.x
                              - (slider->sliderTexture->size.x * 0.5f * slider->scale);
    sliderSprite.position.y = slider->position.y;
    sliderSprite.parent     = uiParentEntity;
    if (!Utils_AddToArray(uiSpriteArray, sliderSprite, uiSpriteArraySize, uiSpriteArrayMaxSize))
    {
        LOG_ERR("UI sprite array is full! Cannot add more UI elements.");
    }

    return UI_CheckBounds(slider->bounds);
}

bool UI_SliderInt(SliderInt* slider)
{
    slider->bounds.x *= uiParentEntity->scale;
    slider->bounds.y *= uiParentEntity->scale;
    slider->bounds.width *= uiParentEntity->scale;
    slider->bounds.height *= uiParentEntity->scale;

    slider->bounds.x += uiParentEntity->position.x;
    slider->bounds.y += uiParentEntity->position.y;

    if (slider->backgroundTexture != NULL)
    {
        Sprite backgroundSprite;
        Sprite_Initialize(&backgroundSprite);
        backgroundSprite.currentTexture = slider->backgroundTexture;
        backgroundSprite.scale          = slider->scale;
        backgroundSprite.position.x     = slider->position.x;
        backgroundSprite.position.y     = slider->position.y;
        backgroundSprite.parent         = uiParentEntity;
        if (!Utils_AddToArray(uiSpriteArray, backgroundSprite, uiSpriteArraySize, uiSpriteArrayMaxSize))
        {
            LOG_ERR("UI sprite array is full! Cannot add more UI elements.");
        }
    }

    Sprite sliderSprite;
    Sprite_Initialize(&sliderSprite);
    sliderSprite.currentTexture = slider->sliderTexture;
    sliderSprite.scale          = slider->scale;
    sliderSprite.position.x =
        slider->position.x
        + ((float)(slider->currentValue - slider->minValue) / (float)(slider->maxValue - slider->minValue))
              * slider->backgroundTexture->size.x
        - (slider->sliderTexture->size.x * 0.5f * slider->scale);
    sliderSprite.position.y = slider->position.y;
    sliderSprite.parent     = uiParentEntity;
    if (!Utils_AddToArray(uiSpriteArray, sliderSprite, uiSpriteArraySize, uiSpriteArrayMaxSize))
    {
        LOG_ERR("UI sprite array is full! Cannot add more UI elements.");
    }

    return UI_CheckBounds(slider->bounds);
}

bool UI_ProgressBar(ProgressBar* progressBar)
{

    progressBar->bounds.x *= uiParentEntity->scale;
    progressBar->bounds.y *= uiParentEntity->scale;
    progressBar->bounds.width *= uiParentEntity->scale;
    progressBar->bounds.height *= uiParentEntity->scale;

    progressBar->bounds.x += uiParentEntity->position.x;
    progressBar->bounds.y += uiParentEntity->position.y;

    if (progressBar->backgroundTexture != NULL)
    {
        Sprite backgroundSprite;
        Sprite_Initialize(&backgroundSprite);
        backgroundSprite.currentTexture = progressBar->backgroundTexture;
        backgroundSprite.scale          = progressBar->scale;
        backgroundSprite.position.x     = progressBar->position.x;
        backgroundSprite.position.y     = progressBar->position.y;
        backgroundSprite.parent         = uiParentEntity;
        if (!Utils_AddToArray(uiSpriteArray, backgroundSprite, uiSpriteArraySize, uiSpriteArrayMaxSize))
        {
            LOG_ERR("UI sprite array is full! Cannot add more UI elements.");
        }
    }

    Sprite sliderSprite;
    Sprite_Initialize(&sliderSprite);
    sliderSprite.currentTexture    = progressBar->progressTexture;
    sliderSprite.scale             = progressBar->scale;
    sliderSprite.position.x        = progressBar->position.x;
    sliderSprite.position.y        = progressBar->position.y;
    sliderSprite.parent            = uiParentEntity;
    sliderSprite.extendedDraw      = true;
    sliderSprite.portionRect.x     = 0;
    sliderSprite.portionRect.y     = 0;
    sliderSprite.portionRect.width = (progressBar->currentValue - progressBar->minValue)
                                     / (progressBar->maxValue - progressBar->minValue)
                                     * progressBar->progressTexture->size.x;
    sliderSprite.portionRect.height = progressBar->progressTexture->size.y;

    if (!Utils_AddToArray(uiSpriteArray, sliderSprite, uiSpriteArraySize, uiSpriteArrayMaxSize))
    {
        LOG_ERR("UI sprite array is full! Cannot add more UI elements.");
    }

    return UI_CheckBounds(progressBar->bounds);
}

bool UI_Checkbox(Checkbox* checkbox)
{
    checkbox->bounds.x *= uiParentEntity->scale;
    checkbox->bounds.y *= uiParentEntity->scale;
    checkbox->bounds.width *= uiParentEntity->scale;
    checkbox->bounds.height *= uiParentEntity->scale;

    checkbox->bounds.x += uiParentEntity->position.x;
    checkbox->bounds.y += uiParentEntity->position.y;

    Sprite checkboxSprite;
    Sprite_Initialize(&checkboxSprite);
    checkboxSprite.scale      = checkbox->scale;
    checkboxSprite.position.x = checkbox->position.x;
    checkboxSprite.position.y = checkbox->position.y;
    checkboxSprite.parent     = uiParentEntity;
    if (checkbox->isChecked)
    {
        checkboxSprite.currentTexture = checkbox->onTexture;
    }
    else
    {
        checkboxSprite.currentTexture = checkbox->offTexture;
    }
    if (!Utils_AddToArray(uiSpriteArray, checkboxSprite, uiSpriteArraySize, uiSpriteArrayMaxSize))
    {
        LOG_ERR("UI sprite array is full! Cannot add more UI elements.");
    }

    return UI_CheckBounds(checkbox->bounds);
}

bool UI_ItemSlot(ItemSlot* itemSlot)
{
    itemSlot->bounds.x *= uiParentEntity->scale;
    itemSlot->bounds.y *= uiParentEntity->scale;
    itemSlot->bounds.width *= uiParentEntity->scale;
    itemSlot->bounds.height *= uiParentEntity->scale;

    itemSlot->bounds.x += uiParentEntity->position.x;
    itemSlot->bounds.y += uiParentEntity->position.y;

    if (itemSlot->backgroundTexture != NULL)
    {
        Sprite backgroundSprite;
        Sprite_Initialize(&backgroundSprite);
        backgroundSprite.currentTexture = itemSlot->backgroundTexture;
        backgroundSprite.scale          = itemSlot->scale;
        backgroundSprite.position.x     = itemSlot->position.x;
        backgroundSprite.position.y     = itemSlot->position.y;
        backgroundSprite.parent         = uiParentEntity;
        if (!Utils_AddToArray(uiSpriteArray, backgroundSprite, uiSpriteArraySize, uiSpriteArrayMaxSize))
        {
            LOG_ERR("UI sprite array is full! Cannot add more UI elements.");
        }
    }

    Sprite itemSprite;
    Sprite_Initialize(&itemSprite);
    itemSprite.position.x     = itemSlot->position.x;
    itemSprite.position.y     = itemSlot->position.y;
    itemSprite.currentTexture = itemSlot->itemTexture;
    itemSprite.scale          = itemSlot->scale;
    itemSprite.parent         = uiParentEntity;
    if (!Utils_AddToArray(uiSpriteArray, itemSprite, uiSpriteArraySize, uiSpriteArrayMaxSize))
    {
        LOG_ERR("UI sprite array is full! Cannot add more UI elements.");
    }

    return UI_CheckBounds(itemSlot->bounds);
}
