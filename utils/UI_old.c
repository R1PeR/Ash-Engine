#include "UI_old.h"

#include "ashes/ash_components.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"

#include <stdio.h>
Entity2D  uiParentEntity       = { 0 };
Drawable* uiDrawableArray      = NULL;
size_t*   uiDrawableArraySize  = 0;
size_t    uiDrawableArrayMaxSize = 0;

static void UI_old_PushSprite(Sprite sprite)
{
    Drawable drawable;
    drawable.type   = DRAWABLE_SPRITE;
    drawable.sprite = sprite;
    if (!Utils_AddToArray(uiDrawableArray, drawable, *uiDrawableArraySize, uiDrawableArrayMaxSize))
    {
        LOG_ERR("UI drawable array is full! Cannot add more UI elements. Array size: %zu, Required size: %zu",
                uiDrawableArrayMaxSize, *uiDrawableArraySize + 1);
    }
}

bool UI_old_CheckBounds(Rectangle bounds)
{
    Camera2D*    camera   = Window_GetCamera();
    Vector2Float mousePos = { (float)(Input_GetMouseX()), (float)(Input_GetMouseY()) };
    Vector2Float worldPos = Utils_ScreenToWorld2D(mousePos, *camera);

#if 0
    DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, RED);
#endif

    if (Utils_PointInRectangle(worldPos, bounds))
    {
        return true;
    }
    return false;
}

void UI_old_SetParentEntity(Entity2D entity)
{
    uiParentEntity = entity;
}

Vector2Float UI_old_NormalizedToWorld(Vector2Float normalized)
{
    Camera2D*    camera = Window_GetCamera();
    Vector2Float screen = {
        normalized.x * (float)Window_GetWidth(),
        normalized.y * (float)Window_GetHeight()
    };
    return Utils_ScreenToWorld2D(screen, *camera);
}

Vector2Float UI_old_ScreenTopLeft(void)
{
    return UI_old_NormalizedToWorld((Vector2Float){ 0.0f, 0.0f });
}

Vector2Float UI_old_ScreenTopRight(void)
{
    return UI_old_NormalizedToWorld((Vector2Float){ 1.0f, 0.0f });
}

Vector2Float UI_old_ScreenBottomLeft(void)
{
    return UI_old_NormalizedToWorld((Vector2Float){ 0.0f, 1.0f });
}

Vector2Float UI_old_ScreenBottomRight(void)
{
    return UI_old_NormalizedToWorld((Vector2Float){ 1.0f, 1.0f });
}

Vector2Float UI_old_ScreenCenter(void)
{
    return UI_old_NormalizedToWorld((Vector2Float){ 0.5f, 0.5f });
}

Vector2Float UI_old_ScreenTopCenter(void)
{
    return UI_old_NormalizedToWorld((Vector2Float){ 0.5f, 0.0f });
}

Vector2Float UI_old_ScreenBottomCenter(void)
{
    return UI_old_NormalizedToWorld((Vector2Float){ 0.5f, 1.0f });
}

Vector2Float UI_old_ScreenLeftCenter(void)
{
    return UI_old_NormalizedToWorld((Vector2Float){ 0.0f, 0.5f });
}

Vector2Float UI_old_ScreenRightCenter(void)
{
    return UI_old_NormalizedToWorld((Vector2Float){ 1.0f, 0.5f });
}

float UI_old_CenterH(float worldX, float elementWidth)
{
    return worldX - elementWidth * 0.5f;
}

float UI_old_CenterV(float worldY, float elementHeight)
{
    return worldY - elementHeight * 0.5f;
}

Vector2Float UI_old_CenterOnScreen(Vector2Float elementSize)
{
    /* UI-space origin is at screen center (camera offset = width/2, height/2),
       so the centered position is simply half the element size from center. */
    return (Vector2Float){ -elementSize.x * 0.5f, -elementSize.y * 0.5f };
}

Vector2Float UI_old_CenterAt(Vector2Float anchor, Vector2Float elementSize)
{
    return (Vector2Float){ anchor.x - elementSize.x * 0.5f, anchor.y - elementSize.y * 0.5f };
}

Vector2Float UI_old_LayoutRow(Vector2Float origin, float itemWidth, float padding, int index)
{
    return (Vector2Float){ origin.x + (float)index * (itemWidth + padding), origin.y };
}

Vector2Float UI_old_LayoutColumn(Vector2Float origin, float itemHeight, float padding, int index)
{
    return (Vector2Float){ origin.x, origin.y + (float)index * (itemHeight + padding) };
}

Vector2Float UI_old_Offset(Vector2Float pos, float dx, float dy)
{
    return (Vector2Float){ pos.x + dx, pos.y + dy };
}

Rectangle UI_old_BoundsFromTexture(Vector2Float pos, TextureData* tex, float scale)
{
    return (Rectangle){ pos.x, pos.y, (float)tex->size.x * scale, (float)tex->size.y * scale };
}

bool UI_old_IsHovered(Rectangle bounds)
{
    return UI_old_CheckBounds(bounds);
}

bool UI_old_IsClicked(Rectangle bounds)
{
    return UI_old_CheckBounds(bounds) && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT);
}

Button UI_old_MakeButton(Vector2Float pos, float scale, TextureData* off, TextureData* on)
{
    Button b    = { 0 };
    b.position  = pos;
    b.scale     = scale;
    b.offTexture = off;
    b.onTexture  = on;
    b.bounds    = UI_old_BoundsFromTexture(pos, off, scale);
    return b;
}

Text UI_old_MakeText(Vector2Float pos, float scale, char* buffer, size_t bufferSize)
{
    Text t      = { 0 };
    t.position  = pos;
    t.scale     = scale;
    t.buffer    = buffer;
    t.bufferSize = bufferSize;
    return t;
}

Checkbox UI_old_MakeCheckbox(Vector2Float pos, float scale, TextureData* off, TextureData* on, bool checked)
{
    Checkbox c   = { 0 };
    c.position   = pos;
    c.scale      = scale;
    c.offTexture = off;
    c.onTexture  = on;
    c.isChecked  = checked;
    c.bounds     = UI_old_BoundsFromTexture(pos, off, scale);
    return c;
}

ItemSlot UI_old_MakeItemSlot(Vector2Float pos, float scale, TextureData* bg, TextureData* item)
{
    ItemSlot s          = { 0 };
    s.position          = pos;
    s.scale             = scale;
    s.backgroundTexture = bg;
    s.itemTexture       = item;
    s.bounds            = UI_old_BoundsFromTexture(pos, bg, scale);
    return s;
}

Pane UI_old_MakePane(Vector2Float pos, Vector2Float size, Color bg)
{
    Pane p       = { 0 };
    p.position   = pos;
    p.size       = size;
    p.background = bg;
    p.bounds     = (Rectangle){ pos.x, pos.y, size.x, size.y };
    return p;
}

ProgressBar UI_old_MakeProgressBar(Vector2Float pos, float scale, TextureData* bg, TextureData* progress,
                               float min, float max, float current)
{
    ProgressBar pb       = { 0 };
    pb.position          = pos;
    pb.scale             = scale;
    pb.backgroundTexture = bg;
    pb.progressTexture   = progress;
    pb.minValue          = min;
    pb.maxValue          = max;
    pb.currentValue      = current;
    pb.bounds            = UI_old_BoundsFromTexture(pos, bg, scale);
    return pb;
}

SliderFloat UI_old_MakeSliderFloat(Vector2Float pos, float scale, TextureData* bg, TextureData* slider,
                               float min, float max, float current)
{
    SliderFloat s         = { 0 };
    s.position            = pos;
    s.scale               = scale;
    s.backgroundSprite    = bg;
    s.sliderTexture       = slider;
    s.minValue            = min;
    s.maxValue            = max;
    s.currentValue        = current;
    s.bounds              = UI_old_BoundsFromTexture(pos, bg, scale);
    return s;
}

SliderInt UI_old_MakeSliderInt(Vector2Float pos, float scale, TextureData* bg, TextureData* slider,
                           int32_t min, int32_t max, int32_t current)
{
    SliderInt s           = { 0 };
    s.position            = pos;
    s.scale               = scale;
    s.backgroundTexture   = bg;
    s.sliderTexture       = slider;
    s.minValue            = min;
    s.maxValue            = max;
    s.currentValue        = current;
    s.bounds              = UI_old_BoundsFromTexture(pos, bg, scale);
    return s;
}

void UI_old_UpdateButton(Button* button)
{
    button->isPressed = UI_old_CheckBounds(button->bounds) && Input_IsMouseButtonDown(INPUT_MOUSE_BUTTON_LEFT);
}

void UI_old_UpdateCheckbox(Checkbox* checkbox)
{
    if (UI_old_CheckBounds(checkbox->bounds) && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
    {
        checkbox->isChecked = !checkbox->isChecked;
    }
}

void UI_old_UpdateSliderFloat(SliderFloat* slider)
{
    if (UI_old_CheckBounds(slider->bounds) && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
    {
        slider->isDragging = true;
    }
    if (Input_IsMouseButtonReleased(INPUT_MOUSE_BUTTON_LEFT))
    {
        slider->isDragging = false;
    }
    if (slider->isDragging)
    {
        Camera2D*    camera   = Window_GetCamera();
        Vector2Float mousePos = { (float)Input_GetMouseX(), (float)Input_GetMouseY() };
        Vector2Float worldPos = Utils_ScreenToWorld2D(mousePos, *camera);
        float        t        = (worldPos.x - slider->bounds.x) / slider->bounds.width;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        slider->currentValue = slider->minValue + t * (slider->maxValue - slider->minValue);
    }
}

void UI_old_UpdateSliderInt(SliderInt* slider)
{
    if (UI_old_CheckBounds(slider->bounds) && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
    {
        slider->isDragging = true;
    }
    if (Input_IsMouseButtonReleased(INPUT_MOUSE_BUTTON_LEFT))
    {
        slider->isDragging = false;
    }
    if (slider->isDragging)
    {
        Camera2D*    camera   = Window_GetCamera();
        Vector2Float mousePos = { (float)Input_GetMouseX(), (float)Input_GetMouseY() };
        Vector2Float worldPos = Utils_ScreenToWorld2D(mousePos, *camera);
        float        t        = (worldPos.x - slider->bounds.x) / slider->bounds.width;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        slider->currentValue = (int32_t)(slider->minValue
                               + t * (float)(slider->maxValue - slider->minValue) + 0.5f);
    }
}

Vector2Float UI_old_PaneContent(Pane* pane, float padding)
{
    return (Vector2Float){ pane->position.x + padding, pane->position.y + padding };
}

void UI_old_Initialize(Drawable* drawableArray, size_t* drawableArraySize, size_t drawableArrayMaxSize)
{
    uiDrawableArray       = drawableArray;
    uiDrawableArraySize   = drawableArraySize;
    uiDrawableArrayMaxSize = drawableArrayMaxSize;
}

void UI_old_AddShape(Shape2D shape)
{
    shape.parent = &uiParentEntity;
    Drawable drawable;
    drawable.type  = DRAWABLE_SHAPE;
    drawable.shape = shape;
    if (!Utils_AddToArray(uiDrawableArray, drawable, *uiDrawableArraySize, uiDrawableArrayMaxSize))
    {
        LOG_ERR("UI drawable array is full! Cannot add more UI shapes. Array size: %zu, Required size: %zu",
                uiDrawableArrayMaxSize, *uiDrawableArraySize + 1);
    }
}

Shape2D UI_old_MakeRect(Vector2Float position, float width, float height, Color color)
{
    Shape2D shape;
    Shape2D_Initialize(&shape);
    shape.type                = SHAPE2D_RECTANGLE;
    shape.position            = position;
    shape.color               = color;
    shape.rectangle.width     = width;
    shape.rectangle.height    = height;
    return shape;
}

Shape2D UI_old_MakeRectLines(Vector2Float position, float width, float height, float outlineThickness, Color color)
{
    Shape2D shape;
    Shape2D_Initialize(&shape);
    shape.type                         = SHAPE2D_RECTANGLE_LINES;
    shape.position                     = position;
    shape.color                        = color;
    shape.rectangle.width              = width;
    shape.rectangle.height             = height;
    shape.rectangle.outlineThickness   = outlineThickness;
    return shape;
}

Shape2D UI_old_MakeLine(Vector2Float startPosition, Vector2Float endPosition, float thickness, Color color)
{
    Shape2D shape;
    Shape2D_Initialize(&shape);
    shape.type                = SHAPE2D_LINE;
    shape.position            = startPosition;
    shape.color               = color;
    shape.line.endPosition    = endPosition;
    shape.line.thickness      = thickness;
    return shape;
}

Shape2D UI_old_MakeCircle(Vector2Float centerPosition, float radius, Color color)
{
    Shape2D shape;
    Shape2D_Initialize(&shape);
    shape.type          = SHAPE2D_CIRCLE;
    shape.position      = centerPosition;
    shape.color         = color;
    shape.circle.radius = radius;
    return shape;
}

Shape2D UI_old_MakeCircleLines(Vector2Float centerPosition, float radius, float outlineThickness, Color color)
{
    Shape2D shape;
    Shape2D_Initialize(&shape);
    shape.type                     = SHAPE2D_CIRCLE_LINES;
    shape.position                 = centerPosition;
    shape.color                    = color;
    shape.circle.radius            = radius;
    shape.circle.outlineThickness  = outlineThickness;
    return shape;
}

bool UI_old_TextureButton(Button* button)
{
    button->bounds.x *= uiParentEntity.scale;
    button->bounds.y *= uiParentEntity.scale;
    button->bounds.width *= uiParentEntity.scale;
    button->bounds.height *= uiParentEntity.scale;

    button->bounds.x += uiParentEntity.position.x;
    button->bounds.y += uiParentEntity.position.y;

    Sprite buttonSprite;
    Sprite_Initialize(&buttonSprite);
    buttonSprite.scale      = button->scale;
    buttonSprite.position.x = button->position.x;
    buttonSprite.position.y = button->position.y;
    buttonSprite.parent     = &uiParentEntity;
    if (button->isPressed)
    {
        buttonSprite.currentTexture = button->onTexture;
    }
    else
    {
        buttonSprite.currentTexture = button->offTexture;
    }
    UI_old_PushSprite(buttonSprite);

    return UI_old_CheckBounds(button->bounds);
}

bool UI_old_TextButton(TextButton* textButton, TextureData* fontAtlas)
{
    textButton->bounds.x *= uiParentEntity.scale;
    textButton->bounds.y *= uiParentEntity.scale;
    textButton->bounds.width *= uiParentEntity.scale;
    textButton->bounds.height *= uiParentEntity.scale;

    textButton->bounds.x += uiParentEntity.position.x;
    textButton->bounds.y += uiParentEntity.position.y;

    Sprite buttonSprite;
    Sprite_Initialize(&buttonSprite);
    buttonSprite.scale      = textButton->scale;
    buttonSprite.position.x = textButton->position.x;
    buttonSprite.position.y = textButton->position.y;
    buttonSprite.parent     = &uiParentEntity;
    if (textButton->isPressed)
    {
        buttonSprite.currentTexture = textButton->onTexture;
    }
    else
    {
        buttonSprite.currentTexture = textButton->offTexture;
    }
    UI_old_PushSprite(buttonSprite);

    UI_old_Text(&textButton->text, fontAtlas);

    return UI_old_CheckBounds(textButton->bounds);
}

bool UI_old_Text(Text* uiText, TextureData* fontAtlas)
{
    uiText->bounds.x *= uiParentEntity.scale;
    uiText->bounds.y *= uiParentEntity.scale;
    uiText->bounds.width *= uiParentEntity.scale;
    uiText->bounds.height *= uiParentEntity.scale;

    uiText->bounds.x += uiParentEntity.position.x;
    uiText->bounds.y += uiParentEntity.position.y;

    for (size_t i = 0; i < uiText->bufferSize; i++)
    {
        Sprite textSprite;
        Sprite_Initialize(&textSprite);
        char         c            = uiText->buffer[i];
        TextureData* charTexture  = fontAtlas + c;
        textSprite.currentTexture = charTexture;
        textSprite.scale          = uiText->scale;
        textSprite.position.x     = uiText->position.x + (charTexture->size.x * uiText->scale * i);
        textSprite.position.y     = uiText->position.y;
        textSprite.parent         = &uiParentEntity;
        UI_old_PushSprite(textSprite);
    }

    return UI_old_CheckBounds(uiText->bounds);
}

bool UI_old_SliderFloat(SliderFloat* slider)
{
    slider->bounds.x *= uiParentEntity.scale;
    slider->bounds.y *= uiParentEntity.scale;
    slider->bounds.width *= uiParentEntity.scale;
    slider->bounds.height *= uiParentEntity.scale;

    slider->bounds.x += uiParentEntity.position.x;
    slider->bounds.y += uiParentEntity.position.y;

    if (slider->backgroundSprite != NULL)
    {
        Sprite backgroundSprite;
        Sprite_Initialize(&backgroundSprite);
        backgroundSprite.currentTexture = slider->backgroundSprite;
        backgroundSprite.scale          = slider->scale;
        backgroundSprite.position.x     = slider->position.x;
        backgroundSprite.position.y     = slider->position.y;
        backgroundSprite.parent         = &uiParentEntity;
        UI_old_PushSprite(backgroundSprite);
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
    sliderSprite.parent     = &uiParentEntity;
    UI_old_PushSprite(sliderSprite);

    return UI_old_CheckBounds(slider->bounds);
}

bool UI_old_SliderInt(SliderInt* slider)
{
    slider->bounds.x *= uiParentEntity.scale;
    slider->bounds.y *= uiParentEntity.scale;
    slider->bounds.width *= uiParentEntity.scale;
    slider->bounds.height *= uiParentEntity.scale;

    slider->bounds.x += uiParentEntity.position.x;
    slider->bounds.y += uiParentEntity.position.y;

    if (slider->backgroundTexture != NULL)
    {
        Sprite backgroundSprite;
        Sprite_Initialize(&backgroundSprite);
        backgroundSprite.currentTexture = slider->backgroundTexture;
        backgroundSprite.scale          = slider->scale;
        backgroundSprite.position.x     = slider->position.x;
        backgroundSprite.position.y     = slider->position.y;
        backgroundSprite.parent         = &uiParentEntity;
        UI_old_PushSprite(backgroundSprite);
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
    sliderSprite.parent     = &uiParentEntity;
    UI_old_PushSprite(sliderSprite);

    return UI_old_CheckBounds(slider->bounds);
}

bool UI_old_ProgressBar(ProgressBar* progressBar)
{

    progressBar->bounds.x *= uiParentEntity.scale;
    progressBar->bounds.y *= uiParentEntity.scale;
    progressBar->bounds.width *= uiParentEntity.scale;
    progressBar->bounds.height *= uiParentEntity.scale;

    progressBar->bounds.x += uiParentEntity.position.x;
    progressBar->bounds.y += uiParentEntity.position.y;

    if (progressBar->backgroundTexture != NULL)
    {
        Sprite backgroundSprite;
        Sprite_Initialize(&backgroundSprite);
        backgroundSprite.currentTexture = progressBar->backgroundTexture;
        backgroundSprite.scale          = progressBar->scale;
        backgroundSprite.position.x     = progressBar->position.x;
        backgroundSprite.position.y     = progressBar->position.y;
        backgroundSprite.parent         = &uiParentEntity;
        UI_old_PushSprite(backgroundSprite);
    }

    Sprite sliderSprite;
    Sprite_Initialize(&sliderSprite);
    sliderSprite.currentTexture    = progressBar->progressTexture;
    sliderSprite.scale             = progressBar->scale;
    sliderSprite.position.x        = progressBar->position.x;
    sliderSprite.position.y        = progressBar->position.y;
    sliderSprite.parent            = &uiParentEntity;
    sliderSprite.drawPortion       = true;
    sliderSprite.portionRect.x     = 0;
    sliderSprite.portionRect.y     = 0;
    sliderSprite.portionRect.width = (progressBar->currentValue - progressBar->minValue)
                                     / (progressBar->maxValue - progressBar->minValue)
                                     * progressBar->progressTexture->size.x;
    sliderSprite.portionRect.height = progressBar->progressTexture->size.y;

    UI_old_PushSprite(sliderSprite);

    return UI_old_CheckBounds(progressBar->bounds);
}

bool UI_old_Checkbox(Checkbox* checkbox)
{
    checkbox->bounds.x *= uiParentEntity.scale;
    checkbox->bounds.y *= uiParentEntity.scale;
    checkbox->bounds.width *= uiParentEntity.scale;
    checkbox->bounds.height *= uiParentEntity.scale;

    checkbox->bounds.x += uiParentEntity.position.x;
    checkbox->bounds.y += uiParentEntity.position.y;

    Sprite checkboxSprite;
    Sprite_Initialize(&checkboxSprite);
    checkboxSprite.scale      = checkbox->scale;
    checkboxSprite.position.x = checkbox->position.x;
    checkboxSprite.position.y = checkbox->position.y;
    checkboxSprite.parent     = &uiParentEntity;
    if (checkbox->isChecked)
    {
        checkboxSprite.currentTexture = checkbox->onTexture;
    }
    else
    {
        checkboxSprite.currentTexture = checkbox->offTexture;
    }
    UI_old_PushSprite(checkboxSprite);

    return UI_old_CheckBounds(checkbox->bounds);
}

bool UI_old_ItemSlot(ItemSlot* itemSlot)
{
    itemSlot->bounds.x *= uiParentEntity.scale;
    itemSlot->bounds.y *= uiParentEntity.scale;
    itemSlot->bounds.width *= uiParentEntity.scale;
    itemSlot->bounds.height *= uiParentEntity.scale;

    itemSlot->bounds.x += uiParentEntity.position.x;
    itemSlot->bounds.y += uiParentEntity.position.y;

    if (itemSlot->backgroundTexture != NULL)
    {
        Sprite backgroundSprite;
        Sprite_Initialize(&backgroundSprite);
        backgroundSprite.currentTexture = itemSlot->backgroundTexture;
        backgroundSprite.scale          = itemSlot->scale;
        backgroundSprite.position.x     = itemSlot->position.x;
        backgroundSprite.position.y     = itemSlot->position.y;
        backgroundSprite.parent         = &uiParentEntity;
        UI_old_PushSprite(backgroundSprite);
    }

    Sprite itemSprite;
    Sprite_Initialize(&itemSprite);
    itemSprite.position.x     = itemSlot->position.x;
    itemSprite.position.y     = itemSlot->position.y;
    itemSprite.currentTexture = itemSlot->itemTexture;
    itemSprite.scale          = itemSlot->scale;
    itemSprite.parent         = &uiParentEntity;
    UI_old_PushSprite(itemSprite);

    return UI_old_CheckBounds(itemSlot->bounds);
}

Button9Slice UI_old_MakeButton9Slice(Vector2Float pos, Vector2Float size, TextureData* texture)
{
    Button9Slice b = { 0 };
    b.position     = pos;
    b.size         = size;
    b.texture      = texture;
    b.bounds       = (Rectangle){ pos.x, pos.y, size.x, size.y };
    return b;
}

void UI_old_UpdateButton9Slice(Button9Slice* button)
{
    button->isPressed = UI_old_CheckBounds(button->bounds) && Input_IsMouseButtonDown(INPUT_MOUSE_BUTTON_LEFT);
}

bool UI_old_TextureButton9Slice(Button9Slice* button)
{
    button->bounds.x *= uiParentEntity.scale;
    button->bounds.y *= uiParentEntity.scale;
    button->bounds.width *= uiParentEntity.scale;
    button->bounds.height *= uiParentEntity.scale;

    button->bounds.x += uiParentEntity.position.x;
    button->bounds.y += uiParentEntity.position.y;

    if (button->texture == NULL)
    {
        return UI_old_CheckBounds(button->bounds) && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT);
    }

    float texW = (float)button->texture->size.x;
    float texH = (float)button->texture->size.y;
    float tileW = texW / 3.0f;
    float tileH = texH / 3.0f;

    float posX = button->position.x * uiParentEntity.scale + uiParentEntity.position.x;
    float posY = button->position.y * uiParentEntity.scale + uiParentEntity.position.y;
    float scale = uiParentEntity.scale;
    float btnW = button->size.x * scale;
    float btnH = button->size.y * scale;
    float crnW = tileW * scale;
    float crnH = tileH * scale;
    float midW = btnW - 2.0f * crnW;
    float midH = btnH - 2.0f * crnH;

    if (midW < 0.0f) { midW = 0.0f; }
    if (midH < 0.0f) { midH = 0.0f; }

    Texture2D tex = button->texture->texture;
    Color tint = button->isPressed ? (Color){ 180, 180, 180, 255 } : WHITE;

    /* Top row */
    DrawTexturePro(tex, (Rectangle){ 0.0f, 0.0f, tileW, tileH },
                   (Rectangle){ posX, posY, crnW, crnH }, (Vector2){ 0, 0 }, 0.0f, tint);
    DrawTexturePro(tex, (Rectangle){ tileW, 0.0f, tileW, tileH },
                   (Rectangle){ posX + crnW, posY, midW, crnH }, (Vector2){ 0, 0 }, 0.0f, tint);
    DrawTexturePro(tex, (Rectangle){ 2.0f * tileW, 0.0f, tileW, tileH },
                   (Rectangle){ posX + crnW + midW, posY, crnW, crnH }, (Vector2){ 0, 0 }, 0.0f, tint);

    /* Middle row */
    DrawTexturePro(tex, (Rectangle){ 0.0f, tileH, tileW, tileH },
                   (Rectangle){ posX, posY + crnH, crnW, midH }, (Vector2){ 0, 0 }, 0.0f, tint);
    DrawTexturePro(tex, (Rectangle){ tileW, tileH, tileW, tileH },
                   (Rectangle){ posX + crnW, posY + crnH, midW, midH }, (Vector2){ 0, 0 }, 0.0f, tint);
    DrawTexturePro(tex, (Rectangle){ 2.0f * tileW, tileH, tileW, tileH },
                   (Rectangle){ posX + crnW + midW, posY + crnH, crnW, midH }, (Vector2){ 0, 0 }, 0.0f, tint);

    /* Bottom row */
    DrawTexturePro(tex, (Rectangle){ 0.0f, 2.0f * tileH, tileW, tileH },
                   (Rectangle){ posX, posY + crnH + midH, crnW, crnH }, (Vector2){ 0, 0 }, 0.0f, tint);
    DrawTexturePro(tex, (Rectangle){ tileW, 2.0f * tileH, tileW, tileH },
                   (Rectangle){ posX + crnW, posY + crnH + midH, midW, crnH }, (Vector2){ 0, 0 }, 0.0f, tint);
    DrawTexturePro(tex, (Rectangle){ 2.0f * tileW, 2.0f * tileH, tileW, tileH },
                   (Rectangle){ posX + crnW + midW, posY + crnH + midH, crnW, crnH }, (Vector2){ 0, 0 }, 0.0f, tint);

    return UI_old_CheckBounds(button->bounds) && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT);
}

bool UI_old_Pane(Pane* pane)
{
    pane->bounds.x *= uiParentEntity.scale;
    pane->bounds.y *= uiParentEntity.scale;
    pane->bounds.width *= uiParentEntity.scale;
    pane->bounds.height *= uiParentEntity.scale;

    pane->bounds.x += uiParentEntity.position.x;
    pane->bounds.y += uiParentEntity.position.y;

    pane->position.x *= uiParentEntity.scale;
    pane->position.y *= uiParentEntity.scale;

    pane->position.x += uiParentEntity.position.x;
    pane->position.y += uiParentEntity.position.y;

    pane->size.x *= uiParentEntity.scale;
    pane->size.y *= uiParentEntity.scale;

    Shape2D paneShape;
    Shape2D_Initialize(&paneShape);
    paneShape.type             = SHAPE2D_RECTANGLE;
    paneShape.parent           = NULL;  /* position is already in world space */
    paneShape.position         = pane->position;
    paneShape.rectangle.width  = pane->size.x;
    paneShape.rectangle.height = pane->size.y;
    paneShape.color            = pane->background;
    Drawable paneDrawable;
    paneDrawable.type  = DRAWABLE_SHAPE;
    paneDrawable.shape = paneShape;
    if (!Utils_AddToArray(uiDrawableArray, paneDrawable, *uiDrawableArraySize, uiDrawableArrayMaxSize))
    {
        LOG_ERR("UI drawable array is full! Cannot add pane background.");
    }

    return UI_old_CheckBounds(pane->bounds);
}
