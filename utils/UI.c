#include "UI.h"

#include "ashes/ash_components.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"

#include <assert.h>
#include <raylib.h>
#include <string.h>

UI_State uiState = {};

static Entity2D* uiParent;
static Color     frameBg           = { 30, 30, 45, 200 };
static Color     frameOutline      = { 80, 80, 120, 255 };
static Color     buttonBg          = { 60, 60, 80, 255 };
static Color     buttonHover       = { 80, 80, 110, 255 };
static Color     buttonActive      = { 100, 100, 140, 255 };
static Color     sliderTrack       = { 50, 50, 65, 255 };
static Color     sliderThumb       = { 130, 140, 180, 255 };
static Color     listHover         = { 70, 70, 95, 255 };
static Color     listSelected      = { 50, 80, 120, 255 };
static Color     toggleActive      = { 100, 140, 220, 255 };
static Color     toggleInactive    = { 60, 80, 120, 255 };
static Color     toggleHover       = { 80, 100, 140, 255 };
static Color     toggleActiveHover = { 130, 170, 240, 255 };
static Color     separatorColor    = { 100, 100, 100, 200 };

void UI_Initialize(Drawable* drawableArray, size_t* drawableArraySize, size_t drawableArrayMaxSize)
{
    assert(drawableArray != NULL);
    assert(drawableArraySize != NULL);
    uiState.drawableArray        = drawableArray;
    uiState.drawableArraySize    = drawableArraySize;
    uiState.drawableArrayMaxSize = drawableArrayMaxSize;
    for (uint16_t i = 0; i < UI_MAX_WIDGETS; i++)
        uiState.tileGridSelected[i] = -1;
}

void UI_SetParentEntity(Entity2D* entity)
{
    assert(entity != NULL);
    uiParent = entity;
}

void UI_Begin(Vector4Float bounds)
{
    uiState.bounds          = bounds;
    uiState.itemCount       = 0;
    uiState.widgetIdCounter = 0;
}

void UI_Layout(UI_LayoutType layout)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uiState.item[uiState.itemCount].layout = layout;
    uiState.item[uiState.itemCount].type   = StackType_Layout;
    uiState.itemCount++;
}

void UI_Center(UI_CenterType mode)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uiState.item[uiState.itemCount].centeringMode = mode;
    uiState.item[uiState.itemCount].type          = StackType_Centering;
    uiState.itemCount++;
}

void UI_Padding(Vector4Float padding)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uiState.item[uiState.itemCount].padding = padding;
    uiState.item[uiState.itemCount].type    = StackType_Padding;
    uiState.itemCount++;
}

void UI_Frame()
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uiState.item[uiState.itemCount].type                = StackType_Frame;
    uiState.item[uiState.itemCount].frame.scrollable    = false;
    uiState.item[uiState.itemCount].frame.contentHeight = 0;
    uiState.item[uiState.itemCount].frame.widgetId      = 0;
    uiState.item[uiState.itemCount].frame.size          = 0;
    uiState.itemCount++;
}

void UI_FrameSize(float sizeRatio)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    assert(sizeRatio > 0.0f && sizeRatio <= 1.0f);
    uiState.item[uiState.itemCount].type                = StackType_Frame;
    uiState.item[uiState.itemCount].frame.scrollable    = false;
    uiState.item[uiState.itemCount].frame.contentHeight = 0;
    uiState.item[uiState.itemCount].frame.widgetId      = 0;
    uiState.item[uiState.itemCount].frame.size          = sizeRatio;
    uiState.itemCount++;
}

void UI_ScrollFrame(float contentHeight)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid                                        = uiState.widgetIdCounter++;
    uiState.item[uiState.itemCount].type                = StackType_Frame;
    uiState.item[uiState.itemCount].frame.scrollable    = true;
    uiState.item[uiState.itemCount].frame.contentHeight = contentHeight;
    uiState.item[uiState.itemCount].frame.widgetId      = wid;
    uiState.itemCount++;
}

void UI_Text(const char* text, float scale, TextureData* font)
{
    assert(text != NULL);
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uiState.item[uiState.itemCount].type          = StackType_Text;
    uiState.item[uiState.itemCount].text.text     = (char*)text;
    uiState.item[uiState.itemCount].text.textSize = strlen(text);
    uiState.item[uiState.itemCount].text.scale    = scale;
    uiState.item[uiState.itemCount].text.font     = font;
    uiState.itemCount++;
}

void UI_Sprite(Sprite* sprite)
{
    assert(sprite != NULL);
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uiState.item[uiState.itemCount].type          = StackType_Sprite;
    uiState.item[uiState.itemCount].sprite.sprite = sprite;
    uiState.itemCount++;
}

bool UI_Button(const char* text, float scale, TextureData* font)
{
    assert(text != NULL);
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid                                    = uiState.widgetIdCounter++;
    uiState.item[uiState.itemCount].type            = StackType_Button;
    uiState.item[uiState.itemCount].button.text     = text;
    uiState.item[uiState.itemCount].button.scale    = scale;
    uiState.item[uiState.itemCount].button.font     = font;
    uiState.item[uiState.itemCount].button.widgetId = wid;
    uiState.itemCount++;

    bool result                = uiState.buttonClicked[wid];
    uiState.buttonClicked[wid] = false;
    return result;
}

float UI_Slider(float value, float min, float max)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid                                    = uiState.widgetIdCounter++;
    uiState.item[uiState.itemCount].type            = StackType_Slider;
    uiState.item[uiState.itemCount].slider.value    = value;
    uiState.item[uiState.itemCount].slider.min      = min;
    uiState.item[uiState.itemCount].slider.max      = max;
    uiState.item[uiState.itemCount].slider.widgetId = wid;
    uiState.itemCount++;

    if (uiState.sliderDragging[wid])
        return uiState.sliderValue[wid];
    return value;
}

int UI_List(const char** items, int itemCount, float textScale, float itemHeight, TextureData* font, bool showScroll)
{
    assert(items != NULL);
    assert(font != NULL);
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid                                    = uiState.widgetIdCounter++;
    uiState.item[uiState.itemCount].type            = StackType_List;
    uiState.item[uiState.itemCount].list.items      = items;
    uiState.item[uiState.itemCount].list.itemCount  = itemCount;
    uiState.item[uiState.itemCount].list.textScale  = textScale;
    uiState.item[uiState.itemCount].list.itemHeight = itemHeight;
    uiState.item[uiState.itemCount].list.font       = font;
    uiState.item[uiState.itemCount].list.showScroll = showScroll;
    uiState.item[uiState.itemCount].list.widgetId   = wid;
    uiState.itemCount++;

    return uiState.listSelected[wid];
}

bool UI_Toggle(const char* text, bool active, float scale, TextureData* font)
{
    assert(text != NULL);
    assert(font != NULL);
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid                                    = uiState.widgetIdCounter++;
    uiState.item[uiState.itemCount].type            = StackType_Toggle;
    uiState.item[uiState.itemCount].button.text     = text;
    uiState.item[uiState.itemCount].button.scale    = scale;
    uiState.item[uiState.itemCount].button.font     = font;
    uiState.item[uiState.itemCount].button.widgetId = wid;
    uiState.toggleState[wid]                        = active;
    uiState.itemCount++;

    bool result                = uiState.buttonClicked[wid];
    uiState.buttonClicked[wid] = false;
    return result;
}

void UI_Separator()
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uiState.item[uiState.itemCount].type = StackType_Separator;
    uiState.itemCount++;
}

int UI_TileGrid(TextureData* textures, int textureCount, int columns, int selectedTile)
{
    assert(textures != NULL);
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid = uiState.widgetIdCounter++;

    uiState.item[uiState.itemCount].type                  = StackType_TileGrid;
    uiState.item[uiState.itemCount].tileGrid.textures     = textures;
    uiState.item[uiState.itemCount].tileGrid.textureCount = textureCount;
    uiState.item[uiState.itemCount].tileGrid.cols         = columns;
    uiState.item[uiState.itemCount].tileGrid.selectedTile = selectedTile;
    uiState.item[uiState.itemCount].tileGrid.widgetId     = wid;
    uiState.itemCount++;

    return uiState.tileGridSelected[wid];
}

static void PushSprite(Sprite sprite)
{
    Drawable d;
    d.type   = DRAWABLE_SPRITE;
    d.sprite = sprite;
    Utils_AddToArray(uiState.drawableArray, d, *uiState.drawableArraySize, uiState.drawableArrayMaxSize);
}

static void PushShape(Shape2D shape)
{
    Drawable d;
    d.type  = DRAWABLE_SHAPE;
    d.shape = shape;
    Utils_AddToArray(uiState.drawableArray, d, *uiState.drawableArraySize, uiState.drawableArrayMaxSize);
}

static Vector2Float GetMouseWorldPos()
{
    Camera2D*    cam = Window_GetCamera();
    Vector2Float s   = { (float)Input_GetMouseX(), (float)Input_GetMouseY() };
    return Utils_ScreenToWorld2D(s, *cam);
}

static bool PointInRect(Vector2Float point, Rectangle rectangle)
{
    return point.x >= rectangle.x && point.x <= rectangle.x + rectangle.width && point.y >= rectangle.y
           && point.y <= rectangle.y + rectangle.height;
}

static void DrawFrameShape(Vector4Float rectangle, Color background)
{
    Shape2D s;
    Shape2D_Initialize(&s);
    s.type             = SHAPE2D_RECTANGLE;
    s.position.x       = rectangle.x;
    s.position.y       = rectangle.y;
    s.rectangle.width  = rectangle.w;
    s.rectangle.height = rectangle.h;
    s.color            = background;
    s.parent           = uiParent;
    PushShape(s);
}

static void DrawFrameOutline(Vector4Float rectangle, Color color)
{
    Shape2D s;
    Shape2D_Initialize(&s);
    s.type                       = SHAPE2D_RECTANGLE_LINES;
    s.position.x                 = rectangle.x;
    s.position.y                 = rectangle.y;
    s.rectangle.width            = rectangle.w;
    s.rectangle.height           = rectangle.h;
    s.rectangle.outlineThickness = 1.0f;
    s.color                      = color;
    s.parent                     = uiParent;
    PushShape(s);
}

static void DrawTextItem(int childId, Vector4Float childBounds, UI_CenterType childCenter, float scrollY)
{
    const char*  text       = uiState.item[childId].text.text;
    size_t       textLength = uiState.item[childId].text.textSize;
    float        scale      = uiState.item[childId].text.scale;
    TextureData* fontAtlas  = uiState.item[childId].text.font;

    float charWidth  = (fontAtlas ? (float)fontAtlas[0].size.x : 8.0f) * scale;
    float charHeight = (fontAtlas ? (float)fontAtlas[0].size.y : 8.0f) * scale;
    float textWidth  = textLength * charWidth;

    float dx = childBounds.x;
    float dy = childBounds.y;
    if (childCenter == CenterHorizontal || childCenter == CenterBoth)
        dx = childBounds.x + (childBounds.w - textWidth) * 0.5f;
    if (childCenter == CenterVertical || childCenter == CenterBoth)
        dy = childBounds.y + (childBounds.h - charHeight) * 0.5f;

    for (size_t k = 0; k < textLength; k++)
    {
        Sprite s;
        Sprite_Initialize(&s);
        unsigned char ch = (unsigned char)text[k];
        if (fontAtlas)
            s.currentTexture = &fontAtlas[ch];
        s.scale      = scale;
        s.position.x = dx + charWidth * k;
        s.position.y = dy;
        s.parent     = uiParent;
        s.isVisible  = true;
        PushSprite(s);
    }
}

static void DrawSpriteItem(int childId, Vector4Float childBounds, UI_CenterType childCenter, float scrollY)
{
    Sprite* src = uiState.item[childId].sprite.sprite;
    if (!src || !src->currentTexture)
        return;

    float spriteWidth  = (float)src->currentTexture->size.x * src->scale;
    float spriteHeight = (float)src->currentTexture->size.y * src->scale;

    float dx = childBounds.x;
    float dy = childBounds.y;
    if (childCenter == CenterHorizontal || childCenter == CenterBoth)
        dx = childBounds.x + (childBounds.w - spriteWidth) * 0.5f;
    if (childCenter == CenterVertical || childCenter == CenterBoth)
        dy = childBounds.y + (childBounds.h - spriteHeight) * 0.5f;

    Sprite s;
    Sprite_Initialize(&s);
    s.currentTexture = src->currentTexture;
    s.scale          = src->scale;
    s.position.x     = dx;
    s.position.y     = dy;
    s.parent         = uiParent;
    s.isVisible      = true;
    s.tint           = src->tint;
    PushSprite(s);
}

static void DrawButtonItem(int childId, Vector4Float childBounds, UI_CenterType childCenter, float scrollY)
{
    const char*  text      = uiState.item[childId].button.text;
    float        scale     = uiState.item[childId].button.scale;
    uint16_t     wid       = uiState.item[childId].button.widgetId;
    TextureData* fontAtlas = uiState.item[childId].button.font;

    float  charWidth  = (fontAtlas ? (float)fontAtlas[0].size.x : 8.0f) * scale;
    float  charHeight = (fontAtlas ? (float)fontAtlas[0].size.y : 8.0f) * scale;
    size_t textLength = strlen(text);
    float  textWidth  = textLength * charWidth;
    float  textHeight = charHeight;

    float padX = childBounds.w * 0.1f;
    float padY = childBounds.h * 0.15f;
    float btnW = childBounds.w - padX * 2;
    float btnH = childBounds.h - padY * 2;
    float invZ = 1.0f / Window_GetCamera()->zoom;

    if (btnW < textWidth + 8.0f * invZ)
        btnW = textWidth + 8.0f * invZ;
    if (btnH < textHeight + 4.0f * invZ)
        btnH = textHeight + 4.0f * invZ;

    float bx = childBounds.x + (childBounds.w - btnW) * 0.5f;
    float by = childBounds.y + (childBounds.h - btnH) * 0.5f;

    Rectangle    rect    = { bx, by, btnW, btnH };
    Vector2Float mp      = GetMouseWorldPos();
    bool         hovered = PointInRect(mp, rect);
    bool         pressed = Input_IsMouseButtonDown(INPUT_MOUSE_BUTTON_LEFT) && hovered;

    Color col = pressed ? buttonActive : (hovered ? buttonHover : buttonBg);

    DrawFrameShape((Vector4Float){ rect.x, rect.y, rect.width, rect.height }, col);
    DrawFrameOutline((Vector4Float){ rect.x, rect.y, rect.width, rect.height }, pressed ? WHITE : frameOutline);

    float dx = bx + (btnW - textWidth) * 0.5f;
    float dy = by + (btnH - textHeight) * 0.5f;

    for (size_t k = 0; k < textLength; k++)
    {
        Sprite s;
        Sprite_Initialize(&s);
        unsigned char c = (unsigned char)text[k];
        if (fontAtlas)
            s.currentTexture = &fontAtlas[c];
        s.scale      = scale;
        s.position.x = dx + charWidth * k;
        s.position.y = dy;
        s.parent     = uiParent;
        s.isVisible  = true;
        PushSprite(s);
    }

    if (hovered && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
        uiState.buttonClicked[wid] = true;
}

static void DrawSliderItem(int childId, Vector4Float childBounds, UI_CenterType childCenter, float scrollY)
{
    uint16_t wid = uiState.item[childId].slider.widgetId;
    float    val = uiState.sliderDragging[wid] ? uiState.sliderValue[wid] : uiState.item[childId].slider.value;
    float    min = uiState.item[childId].slider.min;
    float    max = uiState.item[childId].slider.max;

    float trackH = childBounds.h * 0.3f;
    float trackY = childBounds.y + (childBounds.h - trackH) * 0.5f;
    float padX   = childBounds.w * 0.15f;
    float trackW = childBounds.w - padX * 2;
    float trackX = childBounds.x + padX;

    Rectangle trackRect = { trackX, trackY, trackW, trackH };
    DrawFrameShape((Vector4Float){ trackRect.x, trackRect.y, trackRect.width, trackRect.height }, sliderTrack);

    float t      = (max > min) ? (val - min) / (max - min) : 0;
    float thumbW = trackH * 1.5f;
    float thumbH = trackH * 2.0f;
    float thumbX = trackX + t * (trackW - thumbW);
    float thumbY = trackY - (thumbH - trackH) * 0.5f;

    Rectangle thumbRect = { thumbX, thumbY, thumbW, thumbH };
    DrawFrameShape((Vector4Float){ thumbRect.x, thumbRect.y, thumbRect.width, thumbRect.height }, sliderThumb);

    Vector2Float mousePod = GetMouseWorldPos();
    bool         hovered  = PointInRect(mousePod, trackRect) || PointInRect(mousePod, thumbRect);

    if (Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT) && hovered)
        uiState.sliderDragging[wid] = true;

    if (uiState.sliderDragging[wid])
    {
        if (Input_IsMouseButtonReleased(INPUT_MOUSE_BUTTON_LEFT))
        {
            uiState.sliderDragging[wid] = false;
        }
        else
        {
            float nt = (mousePod.x - trackX) / trackW;
            if (nt < 0)
                nt = 0;
            if (nt > 1)
                nt = 1;
            uiState.sliderValue[wid] = min + nt * (max - min);
        }
    }
}

static void DrawListItem(int childId, Vector4Float childBounds, UI_CenterType childCenter, float scrollY)
{
    const char** items      = uiState.item[childId].list.items;
    int          itemCount  = uiState.item[childId].list.itemCount;
    float        textScale  = uiState.item[childId].list.textScale;
    float        itemH      = uiState.item[childId].list.itemHeight;
    uint16_t     wid        = uiState.item[childId].list.widgetId;
    TextureData* fontAtlas  = uiState.item[childId].list.font;
    bool         showScroll = uiState.item[childId].list.showScroll;
    float        charWidth  = (fontAtlas ? (float)fontAtlas[0].size.x : 8.0f) * textScale;

    float listScroll  = uiState.scrollOffset[wid];
    float totalHeight = itemCount * itemH;
    float maxScroll   = totalHeight - childBounds.h;
    if (maxScroll < 0)
        maxScroll = 0;

    float wheel = GetMouseWheelMove();
    listScroll -= wheel * 30.0f;
    if (listScroll < 0)
        listScroll = 0;
    if (listScroll > maxScroll)
        listScroll = maxScroll;
    uiState.scrollOffset[wid] = listScroll;

    float itemWidth      = childBounds.w;
    float scrollbarWidth = 8;
    if (showScroll && maxScroll > 0)
        itemWidth = childBounds.w - scrollbarWidth;

    Vector2Float mp = GetMouseWorldPos();

    for (int i = 0; i < itemCount; i++)
    {
        float iy = childBounds.y + i * itemH - listScroll;
        if (iy + itemH < childBounds.y || iy > childBounds.y + childBounds.h)
            continue;

        Rectangle itemRect = { childBounds.x, iy, itemWidth, itemH };
        bool      hover    = PointInRect(mp, itemRect);

        Color bgCol = (i == uiState.listSelected[wid]) ? listSelected : (Color){ 0, 0, 0, 0 };
        if (hover && i != uiState.listSelected[wid])
            bgCol = listHover;

        if (bgCol.a > 0)
            DrawFrameShape((Vector4Float){ itemRect.x, itemRect.y, itemRect.width, itemRect.height }, bgCol);

        const char* text       = items[i];
        size_t      textLength = strlen(text);
        float       dx         = childBounds.x + 4;
        float       dy         = iy + (itemH - charWidth) * 0.5f;

        for (size_t k = 0; k < textLength; k++)
        {
            Sprite s;
            Sprite_Initialize(&s);
            unsigned char c = (unsigned char)text[k];
            if (fontAtlas)
                s.currentTexture = &fontAtlas[c];
            s.scale      = textScale;
            s.position.x = dx + charWidth * k;
            s.position.y = dy;
            s.parent     = uiParent;
            s.isVisible  = true;
            PushSprite(s);
        }

        if (hover && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
            uiState.listSelected[wid] = i;
    }

    if (showScroll && maxScroll > 0)
    {
        float sbx = childBounds.x + itemWidth;
        float sby = childBounds.y;
        DrawFrameShape((Vector4Float){ sbx, sby, scrollbarWidth, childBounds.h }, sliderTrack);

        float thumbH = (childBounds.h / totalHeight) * childBounds.h;
        float thumbY = sby + (listScroll / totalHeight) * childBounds.h;
        DrawFrameShape((Vector4Float){ sbx, thumbY, scrollbarWidth, thumbH }, sliderThumb);
    }
}

static void DrawToggleItem(int childId, Vector4Float childBounds, UI_CenterType childCenter, float scrollY)
{
    const char*  text      = uiState.item[childId].button.text;
    float        scale     = uiState.item[childId].button.scale;
    uint16_t     wid       = uiState.item[childId].button.widgetId;
    TextureData* fontAtlas = uiState.item[childId].button.font;

    float  charWidth   = (fontAtlas ? (float)fontAtlas[0].size.x : 8.0f) * scale;
    float  ch   = (fontAtlas ? (float)fontAtlas[0].size.y : 8.0f) * scale;
    size_t textLength = strlen(text);
    float  tw   = textLength * charWidth;
    float  th   = ch;

    float padX = childBounds.w * 0.1f;
    float padY = childBounds.h * 0.15f;
    float btnW = childBounds.w - padX * 2;
    float btnH = childBounds.h - padY * 2;
    float invZ = 1.0f / Window_GetCamera()->zoom;
    if (btnW < tw + 8.0f * invZ)
        btnW = tw + 8.0f * invZ;
    if (btnH < th + 4.0f * invZ)
        btnH = th + 4.0f * invZ;

    float bx = childBounds.x + (childBounds.w - btnW) * 0.5f;
    float by = childBounds.y + (childBounds.h - btnH) * 0.5f;

    Rectangle    rect    = { bx, by, btnW, btnH };
    Vector2Float mp      = GetMouseWorldPos();
    bool         hovered = PointInRect(mp, rect);
    bool         pressed = Input_IsMouseButtonDown(INPUT_MOUSE_BUTTON_LEFT) && hovered;

    bool  active = uiState.toggleState[wid];
    Color col;
    if (active)
        col = hovered ? toggleActiveHover : toggleActive;
    else
        col = hovered ? toggleHover : toggleInactive;
    if (pressed)
        col = toggleActive;

    DrawFrameShape((Vector4Float){ rect.x, rect.y, rect.width, rect.height }, col);
    DrawFrameOutline((Vector4Float){ rect.x, rect.y, rect.width, rect.height },
                     active ? (Color){ 180, 200, 255, 255 } : frameOutline);

    float dx = bx + (btnW - tw) * 0.5f;
    float dy = by + (btnH - th) * 0.5f;

    for (size_t k = 0; k < textLength; k++)
    {
        Sprite s;
        Sprite_Initialize(&s);
        unsigned char c = (unsigned char)text[k];
        if (fontAtlas)
            s.currentTexture = &fontAtlas[c];
        s.scale      = scale;
        s.position.x = dx + charWidth * k;
        s.position.y = dy;
        s.parent     = uiParent;
        s.isVisible  = true;
        PushSprite(s);
    }

    if (hovered && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
        uiState.buttonClicked[wid] = true;
}

static void DrawSeparatorItem(int childId, Vector4Float childBounds, UI_CenterType childCenter, float scrollY)
{
    (void)childId;

    float midY = childBounds.y + childBounds.h * 0.5f;

    Shape2D line;
    Shape2D_Initialize(&line);
    line.type               = SHAPE2D_LINE;
    line.position.x         = childBounds.x;
    line.position.y         = midY;
    line.line.endPosition.x = childBounds.x + childBounds.w;
    line.line.endPosition.y = midY;
    line.line.thickness     = 1.0f / Window_GetCamera()->zoom;
    line.color              = separatorColor;
    line.parent             = uiParent;
    PushShape(line);
}

static void DrawTileGridItem(int childId, Vector4Float childBounds, UI_CenterType childCenter, float scrollY)
{
    (void)scrollY;

    TextureData* textures     = uiState.item[childId].tileGrid.textures;
    int          textureCount = uiState.item[childId].tileGrid.textureCount;
    int          cols         = uiState.item[childId].tileGrid.cols;
    uint16_t     wid          = uiState.item[childId].tileGrid.widgetId;
    int          selectedTile = uiState.tileGridSelected[wid];

    float cellSize = childBounds.w / cols;
    int   rows     = (textureCount + cols - 1) / cols;
    float totalHeight   = rows * cellSize;

    float gridScroll = uiState.scrollOffset[wid];

    Vector2Float mp        = GetMouseWorldPos();
    Rectangle    frameRect = { childBounds.x, childBounds.y, childBounds.w, childBounds.h };
    bool         hovered   = PointInRect(mp, frameRect);

    float wheel = GetMouseWheelMove();
    if (hovered && wheel != 0.0f)
    {
        gridScroll -= wheel * cellSize * 2.0f;
        float maxScroll = totalHeight - childBounds.h;
        if (maxScroll < 0)
            maxScroll = 0;
        if (gridScroll < 0)
            gridScroll = 0;
        if (gridScroll > maxScroll)
            gridScroll = maxScroll;
        uiState.scrollOffset[wid] = gridScroll;
    }

    for (int i = 0; i < textureCount; i++)
    {
        int row = i / cols;
        int col = i % cols;

        float bx = childBounds.x + col * cellSize;
        float by = childBounds.y + row * cellSize - gridScroll;

        if (by + cellSize < childBounds.y || by > childBounds.y + childBounds.h)
            continue;

        Sprite sprite;
        Sprite_Initialize(&sprite);
        sprite.currentTexture = &textures[i];
        sprite.scale          = (cellSize - 1.0f / Window_GetCamera()->zoom) / 16.0f;
        sprite.position.x     = bx;
        sprite.position.y     = by;
        sprite.isVisible      = true;

        if (i == selectedTile)
            sprite.tint = (Color){ 120, 200, 255, 255 };

        PushSprite(sprite);

        if (hovered && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
        {
            Rectangle cellRect = { bx, by, cellSize, cellSize };
            if (PointInRect(mp, cellRect))
            {
                if (i == selectedTile)
                    uiState.tileGridSelected[wid] = -1;
                else
                    uiState.tileGridSelected[wid] = i;
            }
        }
    }
}

static bool IsChildRenderable(UI_StackType type)
{
    return type == StackType_Text || type == StackType_Sprite || type == StackType_Button || type == StackType_Slider
           || type == StackType_List || type == StackType_Toggle || type == StackType_TileGrid
           || type == StackType_Separator;
}

static void DispatchChild(int childId, Vector4Float childBounds, UI_CenterType childCenter, float scrollY)
{
    switch (uiState.item[childId].type)
    {
        case StackType_Text:
            DrawTextItem(childId, childBounds, childCenter, scrollY);
            break;
        case StackType_Sprite:
            DrawSpriteItem(childId, childBounds, childCenter, scrollY);
            break;
        case StackType_Button:
            DrawButtonItem(childId, childBounds, childCenter, scrollY);
            break;
        case StackType_Slider:
            DrawSliderItem(childId, childBounds, childCenter, scrollY);
            break;
        case StackType_List:
            DrawListItem(childId, childBounds, childCenter, scrollY);
            break;
        case StackType_Toggle:
            DrawToggleItem(childId, childBounds, childCenter, scrollY);
            break;
        case StackType_Separator:
            DrawSeparatorItem(childId, childBounds, childCenter, scrollY);
            break;
        case StackType_TileGrid:
            DrawTileGridItem(childId, childBounds, childCenter, scrollY);
            break;
        default:
            return;
    }
}

bool UI_IsMouseOverBounds(Vector4Float bounds)
{
    Vector2Float mp = GetMouseWorldPos();
    return PointInRect(mp, (Rectangle){ bounds.x, bounds.y, bounds.w, bounds.h });
}

void UI_End()
{
    if (uiState.itemCount == 0)
        return;

    uint16_t containerIdx[UI_MAX_STACK_DEPTH];
    int      containerCount = 0;
    for (uint16_t i = 0; i < uiState.itemCount; i++)
    {
        if (uiState.item[i].type == StackType_Frame)
            containerIdx[containerCount++] = i;
    }

    if (containerCount == 0)
    {
        uiState.itemCount = 0;
        return;
    }

    UI_LayoutType rootLayout = LayoutVertical;
    for (int i = 0; i < containerIdx[0]; i++)
    {
        if (uiState.item[i].type == StackType_Layout)
            rootLayout = uiState.item[i].layout;
    }

    Vector4Float root = uiState.bounds;

    float totalAuto  = 0;
    int   autoCount  = 0;
    float fixedTotal = 0;
    for (int f = 0; f < containerCount; f++)
    {
        int   cur = containerIdx[f];
        float s   = uiState.item[cur].frame.size;
        if (s > 0)
            fixedTotal += s;
        else
        {
            totalAuto += 1.0f;
            autoCount++;
        }
    }

    float remainingRatio = 1.0f - fixedTotal;
    float autoUnit       = autoCount > 0 ? remainingRatio / autoCount : 0;

    float cursor = rootLayout == LayoutVertical ? root.y : root.x;
    float span   = rootLayout == LayoutVertical ? root.h : root.w;

    Vector2Float mousePos = GetMouseWorldPos();

    for (int f = 0; f < containerCount; f++)
    {
        int current = containerIdx[f];
        int next    = (f + 1 < containerCount) ? containerIdx[f + 1] : uiState.itemCount;

        float frameSize = uiState.item[current].frame.size > 0 ? uiState.item[current].frame.size : autoUnit;
        float frameSpan = frameSize * span;

        Vector4Float frameBounds;
        if (rootLayout == LayoutVertical)
            frameBounds = (Vector4Float){ root.x, cursor, root.w, frameSpan };
        else
            frameBounds = (Vector4Float){ cursor, root.y, frameSpan, root.h };
        cursor += frameSpan;

        DrawFrameShape(frameBounds, frameBg);
        DrawFrameOutline(frameBounds, frameOutline);

        float scrollY = 0;
        if (uiState.item[current].frame.scrollable)
        {
            uint16_t wid = uiState.item[current].frame.widgetId;
            scrollY      = uiState.scrollOffset[wid];

            bool mouseOverFrame =
                PointInRect(mousePos, (Rectangle){ frameBounds.x, frameBounds.y, frameBounds.w, frameBounds.h });
            if (mouseOverFrame)
            {
                float wheel = GetMouseWheelMove();
                scrollY -= wheel * 30.0f;
                float maxScroll = uiState.item[current].frame.contentHeight - frameBounds.h;
                if (maxScroll < 0)
                    maxScroll = 0;
                if (scrollY < 0)
                    scrollY = 0;
                if (scrollY > maxScroll)
                    scrollY = maxScroll;
            }
            uiState.scrollOffset[wid] = scrollY;
        }

        int childStart = current + 1;
        int childEnd   = next;

        UI_LayoutType childLayout = LayoutVertical;
        UI_CenterType childCenter = CenterNone;
        int           childCount  = 0;
        for (int j = childStart; j < childEnd; j++)
        {
            if (uiState.item[j].type == StackType_Layout)
            {
                childLayout = uiState.item[j].layout;
            }
            else if (uiState.item[j].type == StackType_Centering)
            {
                childCenter = uiState.item[j].centeringMode;
            }
            else if (uiState.item[j].type == StackType_Padding)
            {
                continue;
            }
            else
            {
                childCount++;
            }
        }

        if (childCount == 0)
            continue;

        float slotW      = (childLayout == LayoutHorizontal) ? frameBounds.w / childCount : frameBounds.w;
        float slotH      = (childLayout == LayoutVertical) ? frameBounds.h / childCount : frameBounds.h;
        int   childIndex = 0;

        for (int childId = childStart; childId < childEnd; childId++)
        {
            if (!IsChildRenderable(uiState.item[childId].type))
                continue;

            Vector4Float childPadding = { 0, 0, 0, 0 };
            for (int k = childId; k > childStart; k--)
            {
                if (uiState.item[k].type == StackType_Padding)
                {
                    childPadding = uiState.item[k].padding;
                    break;
                }
            }

            Vector4Float childBounds = { frameBounds.x, frameBounds.y, frameBounds.w, frameBounds.h };

            if (childLayout == LayoutVertical)
            {
                childBounds.x = frameBounds.x;
                childBounds.y = frameBounds.y + (childIndex * slotH);
                childBounds.w = slotW;
                childBounds.h = slotH;
            }
            else
            {
                childBounds.x = frameBounds.x + (childIndex * slotW);
                childBounds.y = frameBounds.y;
                childBounds.w = slotW;
                childBounds.h = slotH;
            }

            childBounds.x += childPadding.x;
            childBounds.y += childPadding.y;
            childBounds.w -= childPadding.w;
            childBounds.h -= childPadding.h;

            DispatchChild(childId, childBounds, childCenter, scrollY);
            childIndex++;
        }
    }
    uiState.itemCount = 0;
}
