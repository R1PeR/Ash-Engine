#ifndef LIBS_UTILS_UI_OLD_H
#define LIBS_UTILS_UI_OLD_H
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

struct Pane
{
    Vector2Float position;
    Vector2Float size;
    Rectangle    bounds;
    Color        background;
};

struct Button9Slice
{
    Vector2Float position;
    Vector2Float size;
    TextureData* texture;
    Rectangle    bounds;
    bool         isPressed;
};

void UI_old_Initialize(Drawable* drawableArray, size_t* drawableArraySize, size_t drawableArrayMaxSize);
void UI_old_SetParentEntity(Entity2D entity);

/* Normalized (0-1) to world space */
Vector2Float UI_old_NormalizedToWorld(Vector2Float normalized);

/* Screen anchor points in world space */
Vector2Float UI_old_ScreenTopLeft(void);
Vector2Float UI_old_ScreenTopRight(void);
Vector2Float UI_old_ScreenBottomLeft(void);
Vector2Float UI_old_ScreenBottomRight(void);
Vector2Float UI_old_ScreenCenter(void);
Vector2Float UI_old_ScreenTopCenter(void);
Vector2Float UI_old_ScreenBottomCenter(void);
Vector2Float UI_old_ScreenLeftCenter(void);
Vector2Float UI_old_ScreenRightCenter(void);

/* Centering: offset a position so an element of given size is centered on it */
float        UI_old_CenterH(float worldX, float elementWidth);
float        UI_old_CenterV(float worldY, float elementHeight);
Vector2Float UI_old_CenterOnScreen(Vector2Float elementSize);
Vector2Float UI_old_CenterAt(Vector2Float anchor, Vector2Float elementSize);

/* Layout: return position for the Nth item in a row or column */
Vector2Float UI_old_LayoutRow(Vector2Float origin, float itemWidth, float padding, int index);
Vector2Float UI_old_LayoutColumn(Vector2Float origin, float itemHeight, float padding, int index);

/* Padding: offset a position by dx/dy */
Vector2Float UI_old_Offset(Vector2Float pos, float dx, float dy);

/* Bounds from texture size */
Rectangle UI_old_BoundsFromTexture(Vector2Float pos, TextureData* tex, float scale);

/* Input checks */
bool UI_old_IsHovered(Rectangle bounds);
bool UI_old_IsClicked(Rectangle bounds);

/* Widget constructors */
Button       UI_old_MakeButton(Vector2Float pos, float scale, TextureData* off, TextureData* on);
Text         UI_old_MakeText(Vector2Float pos, float scale, char* buffer, size_t bufferSize);
Checkbox     UI_old_MakeCheckbox(Vector2Float pos, float scale, TextureData* off, TextureData* on, bool checked);
ItemSlot     UI_old_MakeItemSlot(Vector2Float pos, float scale, TextureData* bg, TextureData* item);
Pane         UI_old_MakePane(Vector2Float pos, Vector2Float size, Color bg);
ProgressBar  UI_old_MakeProgressBar(Vector2Float pos, float scale, TextureData* bg, TextureData* progress, float min, float max, float current);
SliderFloat  UI_old_MakeSliderFloat(Vector2Float pos, float scale, TextureData* bg, TextureData* slider, float min, float max, float current);
SliderInt    UI_old_MakeSliderInt(Vector2Float pos, float scale, TextureData* bg, TextureData* slider, int32_t min, int32_t max, int32_t current);

/* State updaters — call each frame before drawing */
void UI_old_UpdateButton(Button* button);
void UI_old_UpdateCheckbox(Checkbox* checkbox);
void UI_old_UpdateSliderFloat(SliderFloat* slider);
void UI_old_UpdateSliderInt(SliderInt* slider);

/* Pane content origin with padding */
Vector2Float UI_old_PaneContent(Pane* pane, float padding);

/* Draw functions */
bool UI_old_TextureButton(Button* uiButton);
bool UI_old_TextButton(TextButton* uiTextButton, TextureData* fontAtlas);
bool UI_old_Text(Text* uiText, TextureData* fontAtlas);
bool UI_old_SliderFloat(SliderFloat* uiSlider);
bool UI_old_SliderInt(SliderInt* uiSlider);
bool UI_old_ProgressBar(ProgressBar* uiProgressBar);
bool UI_old_Checkbox(Checkbox* uiCheckbox);
bool UI_old_ItemSlot(ItemSlot* uiItemSlot);
bool UI_old_Pane(Pane* pane);

/* 9-slice textured button */
Button9Slice UI_old_MakeButton9Slice(Vector2Float pos, Vector2Float size, TextureData* texture);
void         UI_old_UpdateButton9Slice(Button9Slice* button);
bool         UI_old_TextureButton9Slice(Button9Slice* button);

/* Shape batch — push a Shape2D into the drawable array registered with UI_Initialize */
void UI_old_AddShape(Shape2D shape);      /* position is in UI-space (parent transform applied) */

/* Shape constructors — position is in UI-space (local to the current parent entity) */
Shape2D UI_old_MakeRect(Vector2Float position, float width, float height, Color color);
Shape2D UI_old_MakeRectLines(Vector2Float position, float width, float height, float outlineThickness, Color color);
Shape2D UI_old_MakeLine(Vector2Float startPosition, Vector2Float endPosition, float thickness, Color color);
Shape2D UI_old_MakeCircle(Vector2Float centerPosition, float radius, Color color);
Shape2D UI_old_MakeCircleLines(Vector2Float centerPosition, float radius, float outlineThickness, Color color);

#endif  // LIBS_UTILS_UI_OLD_H
