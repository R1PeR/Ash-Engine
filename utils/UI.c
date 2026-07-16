#include "UI.h"

#include "ashes/ash_components.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"


#include <assert.h>
#include <raylib.h>
#include <string.h>

UI_State uiState = {};

static Entity2D     uiParent;
static Color        frameBg      = { 30, 30, 45, 200 };
static Color        frameOutline = { 80, 80, 120, 255 };
static Color        buttonBg     = { 60, 60, 80, 255 };
static Color        buttonHover  = { 80, 80, 110, 255 };
static Color        buttonActive = { 100, 100, 140, 255 };
static Color        sliderTrack  = { 50, 50, 65, 255 };
static Color        sliderThumb  = { 130, 140, 180, 255 };
static Color        listHover        = { 70, 70, 95, 255 };
static Color        listSelected     = { 50, 80, 120, 255 };
static Color        toggleActive     = { 100, 140, 220, 255 };
static Color        toggleInactive   = { 60, 80, 120, 255 };
static Color        toggleHover      = { 80, 100, 140, 255 };
static Color        toggleActiveHover= { 130, 170, 240, 255 };
static Color        separatorColor   = { 100, 100, 100, 200 };

void UI_Initialize(Drawable* drawableArray, size_t* drawableArraySize, size_t drawableArrayMaxSize)
{
    uiState.drawableArray        = drawableArray;
    uiState.drawableArraySize    = drawableArraySize;
    uiState.drawableArrayMaxSize = drawableArrayMaxSize;
    Entity2D_Initialize(&uiParent);
    for (uint16_t i = 0; i < UI_MAX_WIDGETS; i++)
        uiState.tileGridSelected[i] = -1;
}

void UI_SetParentEntity(Entity2D entity)
{
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

void UI_Frame()
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uiState.item[uiState.itemCount].type                 = StackType_Frame;
    uiState.item[uiState.itemCount].frame.scrollable     = false;
    uiState.item[uiState.itemCount].frame.contentHeight  = 0;
    uiState.item[uiState.itemCount].frame.widgetId       = 0;
    uiState.item[uiState.itemCount].frame.size           = 0;
    uiState.itemCount++;
}

void UI_FrameSize(float sizeRatio)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    assert(sizeRatio > 0.0f && sizeRatio <= 1.0f);
    uiState.item[uiState.itemCount].type                 = StackType_Frame;
    uiState.item[uiState.itemCount].frame.scrollable     = false;
    uiState.item[uiState.itemCount].frame.contentHeight  = 0;
    uiState.item[uiState.itemCount].frame.widgetId       = 0;
    uiState.item[uiState.itemCount].frame.size           = sizeRatio;
    uiState.itemCount++;
}

void UI_ScrollFrame(float contentHeight)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid = uiState.widgetIdCounter++;
    uiState.item[uiState.itemCount].type                 = StackType_Frame;
    uiState.item[uiState.itemCount].frame.scrollable     = true;
    uiState.item[uiState.itemCount].frame.contentHeight  = contentHeight;
    uiState.item[uiState.itemCount].frame.widgetId       = wid;
    uiState.itemCount++;
}

void UI_Text(const char* text, float scale, TextureData* font)
{
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
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uiState.item[uiState.itemCount].type         = StackType_Sprite;
    uiState.item[uiState.itemCount].sprite.sprite = sprite;
    uiState.itemCount++;
}

bool UI_Button(const char* text, float scale, TextureData* font)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid = uiState.widgetIdCounter++;
    uiState.item[uiState.itemCount].type          = StackType_Button;
    uiState.item[uiState.itemCount].button.text   = text;
    uiState.item[uiState.itemCount].button.scale  = scale;
    uiState.item[uiState.itemCount].button.font   = font;
    uiState.item[uiState.itemCount].button.widgetId = wid;
    uiState.itemCount++;

    bool result = uiState.buttonClicked[wid];
    uiState.buttonClicked[wid] = false;
    return result;
}

float UI_Slider(float value, float min, float max)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid = uiState.widgetIdCounter++;
    uiState.item[uiState.itemCount].type           = StackType_Slider;
    uiState.item[uiState.itemCount].slider.value   = value;
    uiState.item[uiState.itemCount].slider.min     = min;
    uiState.item[uiState.itemCount].slider.max     = max;
    uiState.item[uiState.itemCount].slider.widgetId = wid;
    uiState.itemCount++;

    if (uiState.sliderDragging[wid])
        return uiState.sliderValue[wid];
    return value;
}

int UI_List(const char** items, int itemCount, float textScale, float itemHeight, TextureData* font, bool showScroll)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid = uiState.widgetIdCounter++;
    uiState.item[uiState.itemCount].type             = StackType_List;
    uiState.item[uiState.itemCount].list.items       = items;
    uiState.item[uiState.itemCount].list.itemCount   = itemCount;
    uiState.item[uiState.itemCount].list.textScale   = textScale;
    uiState.item[uiState.itemCount].list.itemHeight  = itemHeight;
    uiState.item[uiState.itemCount].list.font        = font;
    uiState.item[uiState.itemCount].list.showScroll  = showScroll;
    uiState.item[uiState.itemCount].list.widgetId    = wid;
    uiState.itemCount++;

    return uiState.listSelected[wid];
}

bool UI_Toggle(const char* text, bool active, float scale, TextureData* font)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid = uiState.widgetIdCounter++;
    uiState.item[uiState.itemCount].type            = StackType_Toggle;
    uiState.item[uiState.itemCount].button.text     = text;
    uiState.item[uiState.itemCount].button.scale    = scale;
    uiState.item[uiState.itemCount].button.font     = font;
    uiState.item[uiState.itemCount].button.widgetId = wid;
    uiState.toggleState[wid] = active;
    uiState.itemCount++;

    bool result = uiState.buttonClicked[wid];
    uiState.buttonClicked[wid] = false;
    return result;
}

void UI_Separator()
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uiState.item[uiState.itemCount].type = StackType_Separator;
    uiState.itemCount++;
}

int UI_TileGrid(TextureData* textures, int textureCount, int cols, int selectedTile)
{
    assert(uiState.itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid = uiState.widgetIdCounter++;

    uiState.item[uiState.itemCount].type                 = StackType_TileGrid;
    uiState.item[uiState.itemCount].tileGrid.textures       = textures;
    uiState.item[uiState.itemCount].tileGrid.textureCount   = textureCount;
    uiState.item[uiState.itemCount].tileGrid.cols           = cols;
    uiState.item[uiState.itemCount].tileGrid.selectedTile   = selectedTile;
    uiState.item[uiState.itemCount].tileGrid.widgetId       = wid;
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
    Camera2D* cam = Window_GetCamera();
    Vector2Float s = { (float)Input_GetMouseX(), (float)Input_GetMouseY() };
    return Utils_ScreenToWorld2D(s, *cam);
}

static bool PointInRect(Vector2Float p, Rectangle r)
{
    return p.x >= r.x && p.x <= r.x + r.width &&
           p.y >= r.y && p.y <= r.y + r.height;
}

static void GetSlotPosition(Vector4Float fb, UI_LayoutType layout, float slotW, float slotH, int ci, float scrollY, float* sx, float* sy)
{
    if (layout == LayoutVertical)
    {
        *sx = fb.x;
        *sy = fb.y + slotH * ci - scrollY;
    }
    else
    {
        *sx = fb.x + slotW * ci;
        *sy = fb.y - scrollY;
    }
}

static void DrawFrameShape(Vector4Float r, Color bg)
{
    Shape2D s;
    Shape2D_Initialize(&s);
    s.type             = SHAPE2D_RECTANGLE;
    s.position.x       = r.x;
    s.position.y       = r.y;
    s.rectangle.width  = r.w;
    s.rectangle.height = r.h;
    s.color            = bg;
    PushShape(s);
}

static void DrawFrameOutline(Vector4Float r, Color c)
{
    Shape2D s;
    Shape2D_Initialize(&s);
    s.type                        = SHAPE2D_RECTANGLE_LINES;
    s.position.x                  = r.x;
    s.position.y                  = r.y;
    s.rectangle.width             = r.w;
    s.rectangle.height            = r.h;
    s.rectangle.outlineThickness  = 1.0f / Window_GetCamera()->zoom;
    s.color                       = c;
    PushShape(s);
}

static void DrawTextItem(int idx, Vector4Float fb, UI_LayoutType layout, UI_CenterType center, float slotW, float slotH, int ci, float scrollY)
{
    const char* text    = uiState.item[idx].text.text;
    size_t      tlen    = uiState.item[idx].text.textSize;
    float       scale   = uiState.item[idx].text.scale;
    TextureData* fontAtlas = uiState.item[idx].text.font;

    float sx, sy;
    GetSlotPosition(fb, layout, slotW, slotH, ci, scrollY, &sx, &sy);

    float cw = (fontAtlas ? (float)fontAtlas[0].size.x : 8.0f) * scale;
    float ch = (fontAtlas ? (float)fontAtlas[0].size.y : 8.0f) * scale;
    float tw = tlen * cw;

    float dx = sx;
    float dy = sy;
    if (center == CenterHorizontal || center == CenterBoth)
        dx = sx + (slotW - tw) * 0.5f;
    if (center == CenterVertical || center == CenterBoth)
        dy = sy + (slotH - ch) * 0.5f;

    for (size_t k = 0; k < tlen; k++)
    {
        Sprite s;
        Sprite_Initialize(&s);
        unsigned char ch = (unsigned char)text[k];
        if (fontAtlas)
            s.currentTexture = &fontAtlas[ch];
        s.scale      = scale;
        s.position.x = dx + cw * k;
        s.position.y = dy;
        s.parent     = &uiParent;
        s.isVisible  = true;
        PushSprite(s);
    }
}

static void DrawSpriteItem(int idx, Vector4Float fb, UI_LayoutType layout, UI_CenterType center, float slotW, float slotH, int ci, float scrollY)
{
    Sprite* src = uiState.item[idx].sprite.sprite;
    if (!src || !src->currentTexture)
        return;

    float sx, sy;
    GetSlotPosition(fb, layout, slotW, slotH, ci, scrollY, &sx, &sy);

    float sw = (float)src->currentTexture->size.x * src->scale;
    float sh = (float)src->currentTexture->size.y * src->scale;

    float dx = sx;
    float dy = sy;
    if (center == CenterHorizontal || center == CenterBoth)
        dx = sx + (slotW - sw) * 0.5f;
    if (center == CenterVertical || center == CenterBoth)
        dy = sy + (slotH - sh) * 0.5f;

    Sprite s;
    Sprite_Initialize(&s);
    s.currentTexture = src->currentTexture;
    s.scale          = src->scale;
    s.position.x     = dx;
    s.position.y     = dy;
    s.parent         = &uiParent;
    s.isVisible      = true;
    s.tint           = src->tint;
    PushSprite(s);
}

static void DrawButtonItem(int idx, Vector4Float fb, UI_LayoutType layout, UI_CenterType center, float slotW, float slotH, int ci, float scrollY)
{
    const char* text      = uiState.item[idx].button.text;
    float       scale     = uiState.item[idx].button.scale;
    uint16_t    wid       = uiState.item[idx].button.widgetId;
    TextureData* fontAtlas = uiState.item[idx].button.font;

    float sx, sy;
    GetSlotPosition(fb, layout, slotW, slotH, ci, scrollY, &sx, &sy);

    float cw = (fontAtlas ? (float)fontAtlas[0].size.x : 8.0f) * scale;
    float ch = (fontAtlas ? (float)fontAtlas[0].size.y : 8.0f) * scale;
    size_t tlen = strlen(text);
    float tw = tlen * cw;
    float th = ch;

    float padX = slotW * 0.1f;
    float padY = slotH * 0.15f;
    float btnW = slotW - padX * 2;
    float btnH = slotH - padY * 2;
    float invZ = 1.0f / Window_GetCamera()->zoom;
    if (btnW < tw + 8.0f * invZ) btnW = tw + 8.0f * invZ;
    if (btnH < th + 4.0f * invZ) btnH = th + 4.0f * invZ;

    float bx = sx + (slotW - btnW) * 0.5f;
    float by = sy + (slotH - btnH) * 0.5f;

    Rectangle rect = { bx, by, btnW, btnH };
    Vector2Float mp = GetMouseWorldPos();
    bool hovered = PointInRect(mp, rect);
    bool pressed = Input_IsMouseButtonDown(INPUT_MOUSE_BUTTON_LEFT) && hovered;

    Color col = pressed ? buttonActive : (hovered ? buttonHover : buttonBg);

    DrawFrameShape((Vector4Float){ rect.x, rect.y, rect.width, rect.height }, col);
    DrawFrameOutline((Vector4Float){ rect.x, rect.y, rect.width, rect.height },
                     pressed ? WHITE : frameOutline);

    float dx = bx + (btnW - tw) * 0.5f;
    float dy = by + (btnH - th) * 0.5f;

    for (size_t k = 0; k < tlen; k++)
    {
        Sprite s;
        Sprite_Initialize(&s);
        unsigned char c = (unsigned char)text[k];
        if (fontAtlas)
            s.currentTexture = &fontAtlas[c];
        s.scale      = scale;
        s.position.x = dx + cw * k;
        s.position.y = dy;
        s.parent     = &uiParent;
        s.isVisible  = true;
        PushSprite(s);
    }

    if (hovered && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
        uiState.buttonClicked[wid] = true;
}

static void DrawSliderItem(int idx, Vector4Float fb, UI_LayoutType layout, UI_CenterType center, float slotW, float slotH, int ci, float scrollY)
{
    uint16_t wid = uiState.item[idx].slider.widgetId;
    float    val = uiState.sliderDragging[wid] ? uiState.sliderValue[wid] : uiState.item[idx].slider.value;
    float    min = uiState.item[idx].slider.min;
    float    max = uiState.item[idx].slider.max;

    float sx, sy;
    GetSlotPosition(fb, layout, slotW, slotH, ci, scrollY, &sx, &sy);

    float trackH = slotH * 0.3f;
    float trackY = sy + (slotH - trackH) * 0.5f;
    float padX   = slotW * 0.15f;
    float trackW = slotW - padX * 2;
    float trackX = sx + padX;

    Rectangle trackRect = { trackX, trackY, trackW, trackH };
    DrawFrameShape((Vector4Float){ trackRect.x, trackRect.y, trackRect.width, trackRect.height }, sliderTrack);

    float t = (max > min) ? (val - min) / (max - min) : 0;
    float thumbW = trackH * 1.5f;
    float thumbH = trackH * 2.0f;
    float thumbX = trackX + t * (trackW - thumbW);
    float thumbY = trackY - (thumbH - trackH) * 0.5f;

    Rectangle thumbRect = { thumbX, thumbY, thumbW, thumbH };
    DrawFrameShape((Vector4Float){ thumbRect.x, thumbRect.y, thumbRect.width, thumbRect.height }, sliderThumb);

    Vector2Float mp = GetMouseWorldPos();
    bool hovered = PointInRect(mp, trackRect) || PointInRect(mp, thumbRect);

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
            float nt = (mp.x - trackX) / trackW;
            if (nt < 0) nt = 0;
            if (nt > 1) nt = 1;
            uiState.sliderValue[wid] = min + nt * (max - min);
        }
    }
}

static void DrawListItem(int idx, Vector4Float fb, UI_LayoutType layout, UI_CenterType center, float slotW, float slotH, int ci, float scrollY)
{
    const char** items      = uiState.item[idx].list.items;
    int          itemCount  = uiState.item[idx].list.itemCount;
    float        textScale  = uiState.item[idx].list.textScale;
    float        itemH      = uiState.item[idx].list.itemHeight;
    uint16_t     wid        = uiState.item[idx].list.widgetId;
    TextureData* fontAtlas  = uiState.item[idx].list.font;
    bool         showScroll = uiState.item[idx].list.showScroll;
    float        cw         = (fontAtlas ? (float)fontAtlas[0].size.x : 8.0f) * textScale;

    float sx, sy;
    GetSlotPosition(fb, layout, slotW, slotH, ci, scrollY, &sx, &sy);

    float listScroll = uiState.scrollOffset[wid];
    float totalH = itemCount * itemH;
    float maxScroll = totalH - slotH;
    if (maxScroll < 0) maxScroll = 0;

    float wheel = GetMouseWheelMove();
    listScroll -= wheel * 30.0f;
    if (listScroll < 0) listScroll = 0;
    if (listScroll > maxScroll) listScroll = maxScroll;
    uiState.scrollOffset[wid] = listScroll;

    float itemWidth = slotW;
    float scrollbarWidth = 8;
    if (showScroll && maxScroll > 0)
        itemWidth = slotW - scrollbarWidth;

    Vector2Float mp = GetMouseWorldPos();

    for (int i = 0; i < itemCount; i++)
    {
        float iy = sy + i * itemH - listScroll;
        if (iy + itemH < sy || iy > sy + slotH)
            continue;

        Rectangle itemRect = { sx, iy, itemWidth, itemH };
        bool hover = PointInRect(mp, itemRect);

        Color bgCol = (i == uiState.listSelected[wid]) ? listSelected : (Color){ 0, 0, 0, 0 };
        if (hover && i != uiState.listSelected[wid])
            bgCol = listHover;

        if (bgCol.a > 0)
            DrawFrameShape((Vector4Float){ itemRect.x, itemRect.y, itemRect.width, itemRect.height }, bgCol);

        const char* text = items[i];
        size_t tlen = strlen(text);
        float dx = sx + 4;
        float dy = iy + (itemH - cw) * 0.5f;

        for (size_t k = 0; k < tlen; k++)
        {
            Sprite s;
            Sprite_Initialize(&s);
            unsigned char c = (unsigned char)text[k];
            if (fontAtlas)
                s.currentTexture = &fontAtlas[c];
            s.scale      = textScale;
            s.position.x = dx + cw * k;
            s.position.y = dy;
            s.parent     = &uiParent;
            s.isVisible  = true;
            PushSprite(s);
        }

        if (hover && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
            uiState.listSelected[wid] = i;
    }

    if (showScroll && maxScroll > 0)
    {
        float sbx = sx + itemWidth;
        float sby = sy;
        DrawFrameShape((Vector4Float){ sbx, sby, scrollbarWidth, slotH }, sliderTrack);

        float thumbH = (slotH / totalH) * slotH;
        float thumbY = sby + (listScroll / totalH) * slotH;
        DrawFrameShape((Vector4Float){ sbx, thumbY, scrollbarWidth, thumbH }, sliderThumb);
    }
}

static void DrawToggleItem(int idx, Vector4Float fb, UI_LayoutType layout, UI_CenterType center, float slotW, float slotH, int ci, float scrollY)
{
    const char* text      = uiState.item[idx].button.text;
    float       scale     = uiState.item[idx].button.scale;
    uint16_t    wid       = uiState.item[idx].button.widgetId;
    TextureData* fontAtlas = uiState.item[idx].button.font;

    float sx, sy;
    GetSlotPosition(fb, layout, slotW, slotH, ci, scrollY, &sx, &sy);

    float cw = (fontAtlas ? (float)fontAtlas[0].size.x : 8.0f) * scale;
    float ch = (fontAtlas ? (float)fontAtlas[0].size.y : 8.0f) * scale;
    size_t tlen = strlen(text);
    float tw = tlen * cw;
    float th = ch;

    float padX = slotW * 0.1f;
    float padY = slotH * 0.15f;
    float btnW = slotW - padX * 2;
    float btnH = slotH - padY * 2;
    float invZ = 1.0f / Window_GetCamera()->zoom;
    if (btnW < tw + 8.0f * invZ) btnW = tw + 8.0f * invZ;
    if (btnH < th + 4.0f * invZ) btnH = th + 4.0f * invZ;

    float bx = sx + (slotW - btnW) * 0.5f;
    float by = sy + (slotH - btnH) * 0.5f;

    Rectangle rect = { bx, by, btnW, btnH };
    Vector2Float mp = GetMouseWorldPos();
    bool hovered = PointInRect(mp, rect);
    bool pressed = Input_IsMouseButtonDown(INPUT_MOUSE_BUTTON_LEFT) && hovered;

    bool active = uiState.toggleState[wid];
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

    for (size_t k = 0; k < tlen; k++)
    {
        Sprite s;
        Sprite_Initialize(&s);
        unsigned char c = (unsigned char)text[k];
        if (fontAtlas)
            s.currentTexture = &fontAtlas[c];
        s.scale      = scale;
        s.position.x = dx + cw * k;
        s.position.y = dy;
        s.parent     = &uiParent;
        s.isVisible  = true;
        PushSprite(s);
    }

    if (hovered && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
        uiState.buttonClicked[wid] = true;
}

static void DrawSeparatorItem(int idx, Vector4Float fb, UI_LayoutType layout, UI_CenterType center, float slotW, float slotH, int ci, float scrollY)
{
    (void)idx;
    (void)center;
    (void)ci;

    float sx, sy;
    GetSlotPosition(fb, layout, slotW, slotH, ci, scrollY, &sx, &sy);

    float midY = sy + slotH * 0.5f;

    Shape2D line;
    Shape2D_Initialize(&line);
    line.type = SHAPE2D_LINE;
    line.position.x = sx;
    line.position.y = midY;
    line.line.endPosition.x = sx + slotW;
    line.line.endPosition.y = midY;
    line.line.thickness = 1.0f / Window_GetCamera()->zoom;
    line.color = separatorColor;
    PushShape(line);
}

static void DrawTileGridItem(int idx, Vector4Float fb, UI_LayoutType layout, UI_CenterType center, float slotW, float slotH, int ci, float scrollY)
{
    (void)layout;
    (void)center;
    (void)ci;
    (void)scrollY;

    TextureData* textures       = uiState.item[idx].tileGrid.textures;
    int          textureCount   = uiState.item[idx].tileGrid.textureCount;
    int          cols           = uiState.item[idx].tileGrid.cols;
    uint16_t     wid            = uiState.item[idx].tileGrid.widgetId;
    int          selectedTile   = uiState.tileGridSelected[wid];

    float cellSize = slotW / cols;
    int rows = (textureCount + cols - 1) / cols;
    float totalH = rows * cellSize;

    float gridScroll = uiState.scrollOffset[wid];

    Vector2Float mp = GetMouseWorldPos();
    Rectangle frameRect = { fb.x, fb.y, slotW, slotH };
    bool hovered = PointInRect(mp, frameRect);

    float wheel = GetMouseWheelMove();
    if (hovered && wheel != 0.0f)
    {
        gridScroll -= wheel * cellSize * 2.0f;
        float maxScroll = totalH - slotH;
        if (maxScroll < 0) maxScroll = 0;
        if (gridScroll < 0) gridScroll = 0;
        if (gridScroll > maxScroll) gridScroll = maxScroll;
        uiState.scrollOffset[wid] = gridScroll;
    }

    for (int i = 0; i < textureCount; i++)
    {
        int row = i / cols;
        int col = i % cols;

        float bx = fb.x + col * cellSize;
        float by = fb.y + row * cellSize - gridScroll;

        if (by + cellSize < fb.y || by > fb.y + slotH)
            continue;

        Sprite sp;
        Sprite_Initialize(&sp);
        sp.currentTexture = &textures[i];
        sp.scale = (cellSize - 1.0f / Window_GetCamera()->zoom) / 16.0f;
        sp.position.x = bx;
        sp.position.y = by;
        sp.isVisible = true;

        if (i == selectedTile)
            sp.tint = (Color){ 120, 200, 255, 255 };

        PushSprite(sp);

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

static bool IsChildRenderable(UI_StackType t)
{
    return t == StackType_Text || t == StackType_Sprite || t == StackType_Button
        || t == StackType_Slider || t == StackType_List
        || t == StackType_Toggle || t == StackType_TileGrid
        || t == StackType_Separator;
}

static void DispatchChild(int idx, Vector4Float fb, UI_LayoutType layout, UI_CenterType center,
                          float slotW, float slotH, int* ci, float scrollY)
{
    switch (uiState.item[idx].type)
    {
    case StackType_Text:      DrawTextItem(idx, fb, layout, center, slotW, slotH, *ci, scrollY); break;
    case StackType_Sprite:    DrawSpriteItem(idx, fb, layout, center, slotW, slotH, *ci, scrollY); break;
    case StackType_Button:    DrawButtonItem(idx, fb, layout, center, slotW, slotH, *ci, scrollY); break;
    case StackType_Slider:    DrawSliderItem(idx, fb, layout, center, slotW, slotH, *ci, scrollY); break;
    case StackType_List:      DrawListItem(idx, fb, layout, center, slotW, slotH, *ci, scrollY); break;
    case StackType_Toggle:    DrawToggleItem(idx, fb, layout, center, slotW, slotH, *ci, scrollY); break;
    case StackType_Separator: DrawSeparatorItem(idx, fb, layout, center, slotW, slotH, *ci, scrollY); break;
    case StackType_TileGrid:  DrawTileGridItem(idx, fb, layout, center, slotW, slotH, *ci, scrollY); break;
    default: return;
    }
    (*ci)++;
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

    float totalAuto = 0;
    int autoCount = 0;
    float fixedTotal = 0;
    for (int f = 0; f < containerCount; f++)
    {
        int cur = containerIdx[f];
        float s = uiState.item[cur].frame.size;
        if (s > 0)
            fixedTotal += s;
        else {
            totalAuto += 1.0f;
            autoCount++;
        }
    }

    float remainingRatio = 1.0f - fixedTotal;
    float autoUnit = autoCount > 0 ? remainingRatio / autoCount : 0;

    float cursor = rootLayout == LayoutVertical ? root.y : root.x;
    float span = rootLayout == LayoutVertical ? root.h : root.w;

    Vector2Float mp = GetMouseWorldPos();

    for (int f = 0; f < containerCount; f++)
    {
        int cur = containerIdx[f];
        int nxt = (f + 1 < containerCount) ? containerIdx[f + 1] : uiState.itemCount;

        float frameSize = uiState.item[cur].frame.size > 0 ? uiState.item[cur].frame.size : autoUnit;
        float frameSpan = frameSize * span;

        Vector4Float fb;
        if (rootLayout == LayoutVertical)
            fb = (Vector4Float){ root.x, cursor, root.w, frameSpan };
        else
            fb = (Vector4Float){ cursor, root.y, frameSpan, root.h };
        cursor += frameSpan;

        DrawFrameShape(fb, frameBg);
        DrawFrameOutline(fb, frameOutline);

        float scrollY = 0;
        if (uiState.item[cur].frame.scrollable)
        {
            uint16_t wid = uiState.item[cur].frame.widgetId;
            scrollY = uiState.scrollOffset[wid];

            bool mouseOverFrame = PointInRect(mp, (Rectangle){ fb.x, fb.y, fb.w, fb.h });
            if (mouseOverFrame)
            {
                float wheel = GetMouseWheelMove();
                scrollY -= wheel * 30.0f;
                float maxScroll = uiState.item[cur].frame.contentHeight - fb.h;
                if (maxScroll < 0) maxScroll = 0;
                if (scrollY < 0) scrollY = 0;
                if (scrollY > maxScroll) scrollY = maxScroll;
            }
            uiState.scrollOffset[wid] = scrollY;
        }

        int childStart = cur + 1;
        int childEnd   = nxt;

        UI_LayoutType childLayout = LayoutVertical;
        UI_CenterType childCenter = CenterNone;
        for (int j = childStart; j < childEnd; j++)
        {
            if (uiState.item[j].type == StackType_Layout)
                childLayout = uiState.item[j].layout;
            else if (uiState.item[j].type == StackType_Centering)
                childCenter = uiState.item[j].centeringMode;
        }

        int childCount = 0;
        for (int j = childStart; j < childEnd; j++)
        {
            if (IsChildRenderable(uiState.item[j].type))
                childCount++;
        }
        if (childCount == 0)
            continue;

        float slotW = (childLayout == LayoutHorizontal) ? fb.w / childCount : fb.w;
        float slotH = (childLayout == LayoutVertical) ? fb.h / childCount : fb.h;

        int ci = 0;
        for (int j = childStart; j < childEnd; j++)
            DispatchChild(j, fb, childLayout, childCenter, slotW, slotH, &ci, scrollY);
    }

    uiState.itemCount = 0;
}
