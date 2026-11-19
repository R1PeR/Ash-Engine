#ifndef LIBS_UTILS_UI_H
#define LIBS_UTILS_UI_H
#include "ashes/ash_components.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"

#include <stdint.h>
#define UI_MAX_STACK_DEPTH 256
#define UI_MAX_WIDGETS     128

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

enum UI_AnchorType
{
    AnchorTopLeft,
    AnchorTopCenter,
    AnchorTopRight,
    AnchorMiddleLeft,
    AnchorMiddleCenter,
    AnchorMiddleRight,
    AnchorBottomLeft,
    AnchorBottomCenter,
    AnchorBottomRight
};

enum UI_GraphType1d
{
    Graph1dHorizontal,
    Graph1dVertical,
};

enum UI_GraphType2d
{
    Graph2dBar,
    Graph2dLine,
    Graph2dPoint
};

typedef Vector4Float UI_PaddingData;
typedef float        UI_SizeData;

struct UI_Theme
{
    Color frameBg;
    Color frameOutline;
    Color frameHighlight;
    Color frameShadow;
    Color buttonBg;
    Color buttonHover;
    Color buttonActive;
    Color buttonHighlight;
    Color buttonShadow;
    Color sliderTrack;
    Color sliderThumb;
    Color sliderHighlight;
    Color sliderShadow;
    Color listHover;
    Color listSelected;
    Color toggleActive;
    Color toggleInactive;
    Color toggleHover;
    Color toggleActiveHover;
    Color separatorColor;
    Color graphLineColor;
    Color graphBarColor;
    Color graphPointColor;
    float bevel;
};

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
    // float    size;
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

struct UI_Graph1DData
{
    float*         values;
    int            valueCount;
    UI_GraphType1d type;
};

struct UI_Graph2DData
{
    Vector2Float*  values;
    int            valueCount;
    UI_GraphType2d type;
};

enum UI_StackType
{
    StackType_Layout,
    StackType_Centering,
    StackType_Padding,
    StackType_Size,
    StackType_Frame,
    StackType_Text,
    StackType_Sprite,
    StackType_Button,
    StackType_Slider,
    StackType_List,
    StackType_Toggle,
    StackType_Separator,
    StackType_TileGrid,
    StackType_Graph1D,
    StackType_Graph2D
};

struct UI_Stack
{
    UI_StackType type;
    union
    {
        UI_LayoutType   layout;
        UI_CenterType   centeringMode;
        UI_PaddingData  padding;
        UI_SizeData     size;
        UI_FrameData    frame;
        UI_TextData     text;
        UI_SpriteData   sprite;
        UI_ButtonData   button;
        UI_SliderData   slider;
        UI_ListData     list;
        UI_TileGridData tileGrid;
        UI_Graph1DData  graph1d;
        UI_Graph2DData  graph2d;
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

    RenderStack* renderStack;

    Vector4Float bounds;

    bool      mouseCaptured;
    Window*   window;
    Entity2D* parentEntity;
    Camera2D* camera;
    UI_Theme  theme;
};

void UI_Initialize(UI_State* uiState, RenderStack* renderStack, Window* window, Camera2D* camera);
void UI_SetParentEntity(UI_State* uiState, Entity2D* entity);
void UI_SetTheme(UI_State* uiState, struct UI_Theme theme);

void UI_Begin(UI_State* uiState, Vector4Float bounds);

void UI_Layout(UI_State* uiState, UI_LayoutType layout);
void UI_Center(UI_State* uiState, UI_CenterType mode);
void UI_Padding(UI_State* uiState, Vector4Float padding);
void UI_Size(UI_State* uiState, float sizeRatio);

void UI_Frame(UI_State* uiState);
void UI_ScrollFrame(UI_State* uiState, float contentHeight);
void UI_Text(UI_State* uiState, const char* text, float scale, TextureData* font);
void UI_Sprite(UI_State* uiState, Sprite* sprite);

bool  UI_Button(UI_State* uiState, const char* text, float scale, TextureData* font);
float UI_Slider(UI_State* uiState, float value, float min, float max);
int UI_List(UI_State* uiState, const char** items, int itemCount, float textScale, float itemHeight, TextureData* font,
            bool showScroll);

bool UI_Toggle(UI_State* uiState, const char* text, bool active, float scale, TextureData* font);
void UI_Separator(UI_State* uiState);
int  UI_TileGrid(UI_State* uiState, TextureData* textures, int textureCount, int cols, int selectedTile);
void UI_Graph1D(UI_State* uiState, float* values, int valueCount, UI_GraphType1d graphType);
void UI_Graph2D(UI_State* uiState, Vector2Float* values, int valueCount, UI_GraphType2d graphType);

bool UI_IsMouseOverBounds(UI_State* uiState, Vector4Float bounds);

void UI_End(UI_State* uiState);

Vector4Float UI_GetBounds(UI_State* uiState, UI_AnchorType anchor, Vector4Float position);
Vector4Float UI_GetSize(UI_State* uiState, Vector4Float position);
bool         UI_GetMouseCaptured(UI_State* uiState);

#endif
