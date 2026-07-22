#ifndef LIBS_UTILS_UI_H
#define LIBS_UTILS_UI_H
#include "ashes/ash_components.h"
#include "ashes/ash_misc.h"

#include <cstdint>
#define UI_MAX_STACK_DEPTH 32
#define UI_MAX_WIDGETS     64

enum UI_LayoutType
{
    LayoutHorizontal,
    LayoutVertical
};

enum UI_CenterType
{
    CenterNone,
    CenterHorizontal,
    CenterVertical,
    CenterBoth
};

typedef Vector4Float UI_PaddingData;

struct UI_TextData
{
    char*        text;
    size_t       textSize;
    float        scale;
    TextureData* font;
};

struct UI_SpriteData
{
    Sprite* sprite;
};

struct UI_FrameData
{
    bool     scrollable;
    float    contentHeight;
    uint16_t widgetId;
    float    size;
};

struct UI_TileGridData
{
    TextureData* textures;
    int          textureCount;
    int          cols;
    int          selectedTile;
    uint16_t     widgetId;
};

struct UI_ButtonData
{
    const char*  text;
    float        scale;
    TextureData* font;
    uint16_t     widgetId;
};

struct UI_SliderData
{
    float    value;
    float    min;
    float    max;
    uint16_t widgetId;
};

struct UI_ListData
{
    const char** items;
    int          itemCount;
    float        textScale;
    float        itemHeight;
    TextureData* font;
    uint16_t     widgetId;
    bool         showScroll;
};

enum UI_StackType
{
    StackType_Layout,
    StackType_Centering,
    StackType_Padding,
    StackType_Frame,
    StackType_Text,
    StackType_Sprite,
    StackType_Button,
    StackType_Slider,
    StackType_List,
    StackType_Toggle,
    StackType_Separator,
    StackType_TileGrid
};

struct UI_Stack
{
    UI_StackType type;
    union
    {
        UI_LayoutType   layout;
        UI_CenterType   centeringMode;
        UI_PaddingData  padding;
        UI_FrameData    frame;
        UI_TextData     text;
        UI_SpriteData   sprite;
        UI_ButtonData   button;
        UI_SliderData   slider;
        UI_ListData     list;
        UI_TileGridData tileGrid;
    };
};

struct UI_State
{
    UI_Stack item[UI_MAX_STACK_DEPTH];
    uint16_t itemCount;

    uint16_t widgetIdCounter;

    bool  buttonClicked[UI_MAX_WIDGETS];
    bool  toggleState[UI_MAX_WIDGETS];
    float sliderValue[UI_MAX_WIDGETS];
    bool  sliderDragging[UI_MAX_WIDGETS];
    int   listSelected[UI_MAX_WIDGETS];
    int   tileGridSelected[UI_MAX_WIDGETS];
    float scrollOffset[UI_MAX_WIDGETS];

    Drawable* drawableArray;
    size_t*   drawableArraySize;
    size_t    drawableArrayMaxSize;

    Vector4Float bounds;
};

void UI_Initialize(Drawable* drawableArray, size_t* drawableArraySize, size_t drawableArrayMaxSize);
void UI_SetParentEntity(Entity2D* entity);

void UI_Begin(Vector4Float bounds);

void UI_Layout(UI_LayoutType layout);
void UI_Center(UI_CenterType mode);
void UI_Padding(Vector4Float padding);

void UI_Frame();
void UI_ScrollFrame(float contentHeight);
void UI_Text(const char* text, float scale, TextureData* font);
void UI_Sprite(Sprite* sprite);

bool  UI_Button(const char* text, float scale, TextureData* font);
float UI_Slider(float value, float min, float max);
int   UI_List(const char** items, int itemCount, float textScale, float itemHeight, TextureData* font, bool showScroll);

void UI_FrameSize(float sizeRatio);
bool UI_Toggle(const char* text, bool active, float scale, TextureData* font);
void UI_Separator();
int  UI_TileGrid(TextureData* textures, int textureCount, int cols, int selectedTile);
bool UI_IsMouseOverBounds(Vector4Float bounds);

void UI_End();

#endif
