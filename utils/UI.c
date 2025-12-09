#include "UI.h"

#include "engine/components/Entity2D.h"
#include "engine/components/Sprite.h"
#include "engine/components/Texture.h"
#include "engine/io/Input.h"
#include "engine/io/Window.h"
#include "engine/misc/Logger.h"
#include "engine/misc/Utils.h"

#include <stdio.h>
Entity2D* uiParentEntity = NULL;

bool UI_CheckBounds(Rectangle bounds)
{
    Camera2D* camera   = Window_GetCamera();
    Vector2   mousePos = { (float)(Input_GetMouseX()), (float)(Input_GetMouseY()) };
    Vector2   worldPos = GetScreenToWorld2D(mousePos, *camera);

#if 1
    DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, RED);
#endif

    if (Input_IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (Utils_PointInRectangle(worldPos, bounds))
        {
            return true;
        }
    }
    return false;
}
void UI_Init(Entity2D* parentEntity)
{
    uiParentEntity = parentEntity;
}

bool UI_TextureButton(Button* button)
{
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
    Sprite_Add(&buttonSprite);

    return UI_CheckBounds(button->bounds);
}

bool UI_TextButton(TextButton* textButton, const char* fontName)
{
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
    Sprite_Add(&buttonSprite);

    UI_Text(&textButton->text, fontName);

    return UI_CheckBounds(textButton->bounds);
}

bool UI_Text(Text* uiText, const char* fontName)
{

    // TOIDO: fix camera scaling
    uiText->bounds.x += uiParentEntity->position.x;
    uiText->bounds.y += uiParentEntity->position.y;
    uiText->bounds.width *= uiParentEntity->scale;
    uiText->bounds.height *= uiParentEntity->scale;

    for (size_t i = 0; i < uiText->bufferSize; i++)
    {
        Sprite textSprite;
        Sprite_Initialize(&textSprite);
        char c = uiText->buffer[i];
        char textureName[32];
        sprintf(textureName, "%s_%d", fontName, c);
        Texture2D* charTexture    = Texture_GetTextureByName(textureName);
        textSprite.currentTexture = charTexture;
        textSprite.scale          = uiText->scale;
        textSprite.position.x     = uiText->position.x + (charTexture->width * uiText->scale * i);
        textSprite.position.y     = uiText->position.y;
        textSprite.parent         = uiParentEntity;
        Sprite_Add(&textSprite);
    }

    return UI_CheckBounds(uiText->bounds);
}

bool UI_SliderFloat(SliderFloat* slider)
{
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
        Sprite_Add(&backgroundSprite);
    }

    Sprite sliderSprite;
    Sprite_Initialize(&sliderSprite);
    sliderSprite.currentTexture = slider->sliderTexture;
    sliderSprite.scale          = slider->scale;
    sliderSprite.position.x     = slider->position.x
                              + ((slider->currentValue - slider->minValue) / (slider->maxValue - slider->minValue))
                                    * slider->backgroundSprite->width
                              - (slider->sliderTexture->width * 0.5f * slider->scale);
    sliderSprite.position.y = slider->position.y;
    sliderSprite.parent     = uiParentEntity;
    Sprite_Add(&sliderSprite);

    return UI_CheckBounds(slider->bounds);
}

bool UI_SliderInt(SliderInt* slider)
{
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
        Sprite_Add(&backgroundSprite);
    }

    Sprite sliderSprite;
    Sprite_Initialize(&sliderSprite);
    sliderSprite.currentTexture = slider->sliderTexture;
    sliderSprite.scale          = slider->scale;
    sliderSprite.position.x =
        slider->position.x
        + ((float)(slider->currentValue - slider->minValue) / (float)(slider->maxValue - slider->minValue))
              * slider->backgroundTexture->width
        - (slider->sliderTexture->width * 0.5f * slider->scale);
    sliderSprite.position.y = slider->position.y;
    sliderSprite.parent     = uiParentEntity;
    Sprite_Add(&sliderSprite);

    return UI_CheckBounds(slider->bounds);
}

bool UI_ProgressBar(ProgressBar* progressBar)
{
    if (progressBar->backgroundTexture != NULL)
    {
        Sprite backgroundSprite;
        Sprite_Initialize(&backgroundSprite);
        backgroundSprite.currentTexture = progressBar->backgroundTexture;
        backgroundSprite.scale          = progressBar->scale;
        backgroundSprite.position.x     = progressBar->position.x;
        backgroundSprite.position.y     = progressBar->position.y;
        backgroundSprite.parent         = uiParentEntity;
        Sprite_Add(&backgroundSprite);
    }

    Sprite sliderSprite;
    Sprite_Initialize(&sliderSprite);
    sliderSprite.currentTexture   = progressBar->progressTexture;
    sliderSprite.scale            = progressBar->scale;
    sliderSprite.position.x       = progressBar->position.x;
    sliderSprite.position.y       = progressBar->position.y;
    sliderSprite.parent           = uiParentEntity;
    sliderSprite.extendedDraw     = true;
    sliderSprite.portionRect.x     = 0;
    sliderSprite.portionRect.y     = 0;
    sliderSprite.portionRect.width = (progressBar->currentValue - progressBar->minValue)
                                    / (progressBar->maxValue - progressBar->minValue)
                                    * progressBar->progressTexture->width;
    sliderSprite.portionRect.height = progressBar->progressTexture->height;

    progressBar->bounds.x += uiParentEntity->position.x;
    progressBar->bounds.y += uiParentEntity->position.y;

    Sprite_Add(&sliderSprite);

    return UI_CheckBounds(progressBar->bounds);
}

bool UI_Checkbox(Checkbox* checkbox)
{
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
    Sprite_Add(&checkboxSprite);

    return UI_CheckBounds(checkbox->bounds);
}

bool UI_ItemSlot(ItemSlot* itemSlot)
{
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
        Sprite_Add(&backgroundSprite);
    }

    Sprite itemSprite;
    Sprite_Initialize(&itemSprite);
    itemSprite.position.x     = itemSlot->position.x;
    itemSprite.position.y     = itemSlot->position.y;
    itemSprite.currentTexture = itemSlot->itemTexture;
    itemSprite.scale          = itemSlot->scale;
    itemSprite.parent         = uiParentEntity;
    Sprite_Add(&itemSprite);

    return UI_CheckBounds(itemSlot->bounds);
}
