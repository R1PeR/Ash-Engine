#include "UI.h"

#include "engine/components/Entity2D.h"
#include "engine/components/Sprite.h"
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

    return UI_CheckBounds(button->bounds);
}

bool UI_TextButton(TextButton* textButton)
{
    textButton->bounds.x += uiParentEntity->position.x;
    textButton->bounds.y += uiParentEntity->position.y;

    return UI_CheckBounds(textButton->bounds);
}

bool UI_Text(Text* uiText, const char* fontName)
{

    //TOIDO: fix camera scaling 
    uiText->bounds.x += uiParentEntity->position.x;
    uiText->bounds.y += uiParentEntity->position.y;
    uiText->bounds.width  *= uiParentEntity->scale;
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

    return UI_CheckBounds(slider->bounds);
}

bool UI_SliderInt(SliderInt* slider)
{
    slider->bounds.x += uiParentEntity->position.x;
    slider->bounds.y += uiParentEntity->position.y;

    return UI_CheckBounds(slider->bounds);
}

bool UI_ProgressBar(ProgressBar* progressBar)
{
    Sprite backgroundSprite;
    backgroundSprite.currentTexture = progressBar->background;
    backgroundSprite.scale          = progressBar->scale;
    backgroundSprite.position.x     = progressBar->position.x;
    backgroundSprite.position.y     = progressBar->position.y;
    backgroundSprite.parent         = uiParentEntity;

    Sprite sliderSprite;
    sliderSprite.currentTexture = progressBar->slider;
    sliderSprite.scale          = progressBar->scale;
    sliderSprite.position.x     = progressBar->position.x;
    sliderSprite.position.y     = progressBar->position.y;
    sliderSprite.parent         = uiParentEntity;

    progressBar->bounds.x += uiParentEntity->position.x;
    progressBar->bounds.y += uiParentEntity->position.y;

    Sprite_Add(&backgroundSprite);
    Sprite_Add(&sliderSprite);

    return UI_CheckBounds(progressBar->bounds);
}

bool UI_Checkbox(Checkbox* checkbox)
{
    checkbox->bounds.x += uiParentEntity->position.x;
    checkbox->bounds.y += uiParentEntity->position.y;

    return UI_CheckBounds(checkbox->bounds);
}

bool UI_ItemSlot(ItemSlot* itemSlot)
{
    itemSlot->bounds.x += uiParentEntity->position.x;
    itemSlot->bounds.y += uiParentEntity->position.y;

    return UI_CheckBounds(itemSlot->bounds);
}
