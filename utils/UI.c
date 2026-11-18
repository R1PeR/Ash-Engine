#include "UI.h"

#include "ashes/ash_components.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_platform.h"

#include <assert.h>
#include <string.h>

void UI_Initialize(UI_State* uiState, RenderStack* renderStack, Window* window, Camera2D* camera)
{
    assert(renderStack != NULL);
    assert(window != NULL);
    uiState->renderStack = renderStack;
    for (uint16_t i = 0; i < UI_MAX_WIDGETS; i++)
    {
        uiState->tileGridSelected[i] = -1;
    }
    uiState->window = window;
    uiState->camera = camera;
    uiState->theme  = {
        { 30, 30, 45, 200 },    /* frameBg             */
        { 80, 80, 120, 255 },   /* frameOutline        */
        { 95, 95, 140, 255 },   /* frameHighlight      */
        { 15, 15, 25, 255 },    /* frameShadow         */
        { 60, 60, 80, 255 },    /* buttonBg            */
        { 80, 80, 110, 255 },   /* buttonHover         */
        { 100, 100, 140, 255 }, /* buttonActive       */
        { 130, 130, 165, 255 }, /* buttonHighlight    */
        { 30, 30, 45, 255 },    /* buttonShadow        */
        { 50, 50, 65, 255 },    /* sliderTrack         */
        { 130, 140, 180, 255 }, /* sliderThumb        */
        { 170, 180, 215, 255 }, /* sliderHighlight    */
        { 70, 75, 100, 255 },   /* sliderShadow        */
        { 70, 70, 95, 255 },    /* listHover           */
        { 50, 80, 120, 255 },   /* listSelected        */
        { 100, 140, 220, 255 }, /* toggleActive       */
        { 60, 80, 120, 255 },   /* toggleInactive      */
        { 80, 100, 140, 255 },  /* toggleHover         */
        { 130, 170, 240, 255 }, /* toggleActiveHover  */
        { 100, 100, 100, 200 }, /* separatorColor     */
        { 200, 200, 200, 255 }, /* graphLineColor     */
        { 100, 150, 250, 255 }, /* graphBarColor      */
        { 250, 100, 100, 255 }, /* graphPointColor    */
        2.0f                    /* bevel               */
    };
}

void UI_SetParentEntity(UI_State* uiState, Entity2D* entity)
{
    assert(entity != NULL);
    uiState->parentEntity = entity;
}

void UI_SetTheme(UI_State* uiState, struct UI_Theme theme)
{
    uiState->theme = theme;
}

void UI_Begin(UI_State* uiState, Vector4Float bounds)
{
    uiState->bounds          = bounds;
    uiState->itemCount       = 0;
    uiState->widgetIdCounter = 0;
    uiState->mouseCaptured   = false;
}

void UI_Layout(UI_State* uiState, UI_LayoutType layout)
{
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uiState->item[uiState->itemCount].layout = layout;
    uiState->item[uiState->itemCount].type   = StackType_Layout;
    uiState->itemCount++;
}

void UI_Center(UI_State* uiState, UI_CenterType mode)
{
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uiState->item[uiState->itemCount].centeringMode = mode;
    uiState->item[uiState->itemCount].type          = StackType_Centering;
    uiState->itemCount++;
}

void UI_Padding(UI_State* uiState, Vector4Float padding)
{
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uiState->item[uiState->itemCount].padding = padding;
    uiState->item[uiState->itemCount].type    = StackType_Padding;
    uiState->itemCount++;
}

void UI_Frame(UI_State* uiState)
{
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uiState->item[uiState->itemCount].type                = StackType_Frame;
    uiState->item[uiState->itemCount].frame.scrollable    = false;
    uiState->item[uiState->itemCount].frame.contentHeight = 0;
    uiState->item[uiState->itemCount].frame.widgetId      = 0;
    uiState->item[uiState->itemCount].frame.size          = 0;
    uiState->itemCount++;
}

void UI_FrameSize(UI_State* uiState, float sizeRatio)
{
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    assert(sizeRatio > 0.0f && sizeRatio <= 1.0f);
    uiState->item[uiState->itemCount].type                = StackType_Frame;
    uiState->item[uiState->itemCount].frame.scrollable    = false;
    uiState->item[uiState->itemCount].frame.contentHeight = 0;
    uiState->item[uiState->itemCount].frame.widgetId      = 0;
    uiState->item[uiState->itemCount].frame.size          = sizeRatio;
    uiState->itemCount++;
}

void UI_ScrollFrame(UI_State* uiState, float contentHeight)
{
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid                                          = uiState->widgetIdCounter++;
    uiState->item[uiState->itemCount].type                = StackType_Frame;
    uiState->item[uiState->itemCount].frame.scrollable    = true;
    uiState->item[uiState->itemCount].frame.contentHeight = contentHeight;
    uiState->item[uiState->itemCount].frame.widgetId      = wid;
    uiState->itemCount++;
}

void UI_Text(UI_State* uiState, const char* text, float scale, TextureData* font)
{
    assert(text != NULL);
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uiState->item[uiState->itemCount].type          = StackType_Text;
    uiState->item[uiState->itemCount].text.text     = (char*)text;
    uiState->item[uiState->itemCount].text.textSize = strlen(text);
    uiState->item[uiState->itemCount].text.scale    = scale;
    uiState->item[uiState->itemCount].text.font     = font;
    uiState->itemCount++;
}

void UI_Sprite(UI_State* uiState, Sprite* sprite)
{
    assert(sprite != NULL);
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uiState->item[uiState->itemCount].type          = StackType_Sprite;
    uiState->item[uiState->itemCount].sprite.sprite = sprite;
    uiState->itemCount++;
}

bool UI_Button(UI_State* uiState, const char* text, float scale, TextureData* font)
{
    assert(text != NULL);
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid                                      = uiState->widgetIdCounter++;
    uiState->item[uiState->itemCount].type            = StackType_Button;
    uiState->item[uiState->itemCount].button.text     = text;
    uiState->item[uiState->itemCount].button.scale    = scale;
    uiState->item[uiState->itemCount].button.font     = font;
    uiState->item[uiState->itemCount].button.widgetId = wid;
    uiState->itemCount++;

    bool result                 = uiState->buttonClicked[wid];
    uiState->buttonClicked[wid] = false;
    return result;
}

float UI_Slider(UI_State* uiState, float value, float min, float max)
{
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid                                      = uiState->widgetIdCounter++;
    uiState->item[uiState->itemCount].type            = StackType_Slider;
    uiState->item[uiState->itemCount].slider.value    = value;
    uiState->item[uiState->itemCount].slider.min      = min;
    uiState->item[uiState->itemCount].slider.max      = max;
    uiState->item[uiState->itemCount].slider.widgetId = wid;
    uiState->itemCount++;

    if (uiState->sliderDragging[wid])
        return uiState->sliderValue[wid];
    return value;
}

int UI_List(UI_State* uiState, const char** items, int itemCount, float textScale, float itemHeight, TextureData* font,
            bool showScroll)
{
    assert(items != NULL);
    assert(font != NULL);
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid                                      = uiState->widgetIdCounter++;
    uiState->item[uiState->itemCount].type            = StackType_List;
    uiState->item[uiState->itemCount].list.items      = items;
    uiState->item[uiState->itemCount].list.itemCount  = itemCount;
    uiState->item[uiState->itemCount].list.textScale  = textScale;
    uiState->item[uiState->itemCount].list.itemHeight = itemHeight;
    uiState->item[uiState->itemCount].list.font       = font;
    uiState->item[uiState->itemCount].list.showScroll = showScroll;
    uiState->item[uiState->itemCount].list.widgetId   = wid;
    uiState->itemCount++;

    return uiState->listSelected[wid];
}

bool UI_Toggle(UI_State* uiState, const char* text, bool active, float scale, TextureData* font)
{
    assert(text != NULL);
    assert(font != NULL);
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid                                      = uiState->widgetIdCounter++;
    uiState->item[uiState->itemCount].type            = StackType_Toggle;
    uiState->item[uiState->itemCount].button.text     = text;
    uiState->item[uiState->itemCount].button.scale    = scale;
    uiState->item[uiState->itemCount].button.font     = font;
    uiState->item[uiState->itemCount].button.widgetId = wid;
    uiState->toggleState[wid]                         = active;
    uiState->itemCount++;

    bool result                 = uiState->buttonClicked[wid];
    uiState->buttonClicked[wid] = false;
    return result;
}

void UI_Separator(UI_State* uiState)
{
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uiState->item[uiState->itemCount].type = StackType_Separator;
    uiState->itemCount++;
}

int UI_TileGrid(UI_State* uiState, TextureData* textures, int textureCount, int columns, int selectedTile)
{
    assert(textures != NULL);
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uint16_t wid = uiState->widgetIdCounter++;

    uiState->item[uiState->itemCount].type                  = StackType_TileGrid;
    uiState->item[uiState->itemCount].tileGrid.textures     = textures;
    uiState->item[uiState->itemCount].tileGrid.textureCount = textureCount;
    uiState->item[uiState->itemCount].tileGrid.cols         = columns;
    uiState->item[uiState->itemCount].tileGrid.selectedTile = selectedTile;
    uiState->item[uiState->itemCount].tileGrid.widgetId     = wid;
    uiState->itemCount++;

    return uiState->tileGridSelected[wid];
}

void UI_Graph1D(UI_State* uiState, float* values, int valueCount, UI_GraphType1d graphType)
{
    assert(values != NULL);
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uiState->item[uiState->itemCount].type               = StackType_Graph1D;
    uiState->item[uiState->itemCount].graph1d.values     = values;
    uiState->item[uiState->itemCount].graph1d.valueCount = valueCount;
    uiState->item[uiState->itemCount].graph1d.type       = graphType;
    uiState->itemCount++;
}

void UI_Graph2D(UI_State* uiState, Vector2Float* values, int valueCount, UI_GraphType2d graphType)
{
    assert(values != NULL);
    assert(uiState->itemCount < UI_MAX_STACK_DEPTH);
    uiState->item[uiState->itemCount].type               = StackType_Graph2D;
    uiState->item[uiState->itemCount].graph2d.values     = values;
    uiState->item[uiState->itemCount].graph2d.valueCount = valueCount;
    uiState->item[uiState->itemCount].graph2d.type       = graphType;
    uiState->itemCount++;
}

static void PushSprite(UI_State* uiState, Sprite sprite)
{
    RenderStackItem d;
    d.type   = RENDERCOMMAND_DRAW_SPRITE;
    d.sprite = sprite;
    RenderStack_PushSprite(uiState->renderStack, sprite);
}

static void PushShape(UI_State* uiState, Shape2D shape)
{
    RenderStackItem d;
    d.type  = RENDERCOMMAND_DRAW_SHAPE;
    d.shape = shape;
    RenderStack_PushShape(uiState->renderStack, shape);
}

static Vector2Float GetMouseWorldPos(UI_State* uiState)
{
    return Utils_ScreenToWorld2D({ (float)Input_GetMouseX(), (float)Input_GetMouseY() }, *uiState->camera);
}

static bool PointInRect(UI_State* uiState, Vector2Float point, Rectangle rectangle)
{
    if (uiState->parentEntity != NULL)
    {
        point.x -= uiState->parentEntity->position.x;
        point.y -= uiState->parentEntity->position.y;
        point.x /= uiState->parentEntity->scale;
        point.y /= uiState->parentEntity->scale;
    }
    return point.x >= rectangle.x && point.x <= rectangle.x + rectangle.width && point.y >= rectangle.y
           && point.y <= rectangle.y + rectangle.height;
}

static void PushLocalClip(UI_State* uiState, Vector4Float localBounds)
{
    Rectangle worldRect = { localBounds.x, localBounds.y, localBounds.w, localBounds.h };
    if (uiState->parentEntity != NULL)
    {
        worldRect.x = uiState->parentEntity->position.x + localBounds.x * uiState->parentEntity->scale;
        worldRect.y = uiState->parentEntity->position.y + localBounds.y * uiState->parentEntity->scale;
        worldRect.width *= uiState->parentEntity->scale;
        worldRect.height *= uiState->parentEntity->scale;
    }
    RenderStack_PushClip(uiState->renderStack, worldRect);
}

static void DrawFrameShape(UI_State* uiState, Vector4Float rectangle, Color background)
{
    Shape2D s;
    Shape2D_Initialize(&s);
    s.type             = SHAPE2D_RECTANGLE;
    s.position.x       = rectangle.x;
    s.position.y       = rectangle.y;
    s.rectangle.width  = rectangle.w;
    s.rectangle.height = rectangle.h;
    s.color            = background;
    s.parent           = uiState->parentEntity;
    PushShape(uiState, s);
}

static void DrawLine(UI_State* uiState, Vector2Float a, Vector2Float b, float thickness, Color color)
{
    Shape2D s;
    Shape2D_Initialize(&s);
    s.type               = SHAPE2D_LINE;
    s.position.x         = a.x;
    s.position.y         = a.y;
    s.line.endPosition.x = b.x;
    s.line.endPosition.y = b.y;
    s.line.thickness     = thickness;
    s.color              = color;
    s.parent             = uiState->parentEntity;
    PushShape(uiState, s);
}

/* Draws a raised (true) or pressed/inset (false) 3D bevel around a filled rectangle. */
static void DrawBevel(UI_State* uiState, Vector4Float rect, Color base, bool raised)
{
    DrawFrameShape(uiState, rect, base);

    Color hi = raised ? uiState->theme.buttonHighlight : uiState->theme.buttonShadow;
    Color lo = raised ? uiState->theme.buttonShadow : uiState->theme.buttonHighlight;

    float b  = uiState->theme.bevel;
    float x0 = rect.x;
    float y0 = rect.y;
    float x1 = rect.x + rect.w;
    float y1 = rect.y + rect.h;

    DrawLine(uiState, (Vector2Float){ x0, y0 }, (Vector2Float){ x1, y0 }, b, hi);
    DrawLine(uiState, (Vector2Float){ x0, y0 }, (Vector2Float){ x0, y1 }, b, hi);
    DrawLine(uiState, (Vector2Float){ x0, y1 }, (Vector2Float){ x1, y1 }, b, lo);
    DrawLine(uiState, (Vector2Float){ x1, y0 }, (Vector2Float){ x1, y1 }, b, lo);
}

static void DrawTextItem(UI_State* uiState, int childId, Vector4Float childBounds, UI_CenterType childCenter,
                         float scrollY)
{
    (void)scrollY;

    const char*  text       = uiState->item[childId].text.text;
    size_t       textLength = uiState->item[childId].text.textSize;
    float        scale      = uiState->item[childId].text.scale;
    TextureData* fontAtlas  = uiState->item[childId].text.font;

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
        s.parent     = uiState->parentEntity;
        s.isVisible  = true;
        PushSprite(uiState, s);
    }
}

static void DrawSpriteItem(UI_State* uiState, int childId, Vector4Float childBounds, UI_CenterType childCenter,
                           float scrollY)
{
    (void)scrollY;

    Sprite* src = uiState->item[childId].sprite.sprite;
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

    Sprite sprite;
    Sprite_Initialize(&sprite);
    sprite.currentTexture = src->currentTexture;
    sprite.scale          = src->scale;
    sprite.position.x     = dx;
    sprite.position.y     = dy;
    sprite.parent         = uiState->parentEntity;
    sprite.isVisible      = true;
    sprite.tint           = src->tint;
    PushSprite(uiState, sprite);
}

static void DrawButtonItem(UI_State* uiState, int childId, Vector4Float childBounds, UI_CenterType childCenter,
                           float scrollY)
{
    (void)scrollY;
    (void)childCenter;

    const char*  text      = uiState->item[childId].button.text;
    float        scale     = uiState->item[childId].button.scale;
    uint16_t     wid       = uiState->item[childId].button.widgetId;
    TextureData* fontAtlas = uiState->item[childId].button.font;

    float  charWidth  = (fontAtlas ? (float)fontAtlas[0].size.x : 8.0f) * scale;
    float  charHeight = (fontAtlas ? (float)fontAtlas[0].size.y : 8.0f) * scale;
    size_t textLength = strlen(text);
    float  textWidth  = textLength * charWidth;
    float  textHeight = charHeight;

    float btnW = childBounds.w;
    float btnH = childBounds.h;

    float bx = childBounds.x + (childBounds.w - btnW) * 0.5f;
    float by = childBounds.y + (childBounds.h - btnH) * 0.5f;

    Rectangle    rect    = { bx, by, btnW, btnH };
    Vector2Float mp      = GetMouseWorldPos(uiState);
    bool         hovered = PointInRect(uiState, mp, rect);
    bool         pressed = Input_IsMouseButtonDown(INPUT_MOUSE_BUTTON_LEFT) && hovered;

    Color col =
        pressed ? uiState->theme.buttonActive : (hovered ? uiState->theme.buttonHover : uiState->theme.buttonBg);

    /* pressed -> inset (shadow on top/left), otherwise raised bevel */
    DrawBevel(uiState, (Vector4Float){ rect.x, rect.y, rect.width, rect.height }, col, !pressed);

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
        s.parent     = uiState->parentEntity;
        s.isVisible  = true;
        PushSprite(uiState, s);
    }

    if (hovered && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
        uiState->buttonClicked[wid] = true;
}

static void DrawSliderItem(UI_State* uiState, int childId, Vector4Float childBounds, UI_CenterType childCenter,
                           float scrollY)
{
    (void)scrollY;
    (void)childCenter;

    uint16_t wid = uiState->item[childId].slider.widgetId;
    float    val = uiState->sliderDragging[wid] ? uiState->sliderValue[wid] : uiState->item[childId].slider.value;
    float    min = uiState->item[childId].slider.min;
    float    max = uiState->item[childId].slider.max;

    float trackH = childBounds.h * 0.3f;
    float trackY = childBounds.y + (childBounds.h - trackH) * 0.5f;
    float trackW = childBounds.w;
    float trackX = childBounds.x;

    Rectangle trackRect = { trackX, trackY, trackW, trackH };
    DrawBevel(uiState, (Vector4Float){ trackRect.x, trackRect.y, trackRect.width, trackRect.height },
              uiState->theme.sliderTrack, false);

    float t      = (max > min) ? (val - min) / (max - min) : 0;
    float thumbW = trackH * 1.5f;
    float thumbH = trackH * 2.0f;
    float thumbX = trackX + t * (trackW - thumbW);
    float thumbY = trackY - (thumbH - trackH) * 0.5f;

    Rectangle thumbRect = { thumbX, thumbY, thumbW, thumbH };
    DrawBevel(uiState, (Vector4Float){ thumbRect.x, thumbRect.y, thumbRect.width, thumbRect.height },
              uiState->theme.sliderThumb, true);

    Vector2Float mousePos = GetMouseWorldPos(uiState);
    bool         hovered  = PointInRect(uiState, mousePos, trackRect) || PointInRect(uiState, mousePos, thumbRect);

    if (Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT) && hovered)
        uiState->sliderDragging[wid] = true;

    if (uiState->sliderDragging[wid])
    {
        if (Input_IsMouseButtonReleased(INPUT_MOUSE_BUTTON_LEFT))
        {
            uiState->sliderDragging[wid] = false;
        }
        else
        {
            float nt = (mousePos.x - trackX) / trackW;
            if (nt < 0)
                nt = 0;
            if (nt > 1)
                nt = 1;
            uiState->sliderValue[wid] = min + nt * (max - min);
        }
    }
}

static void DrawListItem(UI_State* uiState, int childId, Vector4Float childBounds, UI_CenterType childCenter,
                         float scrollY)
{
    (void)scrollY;
    (void)childCenter;

    const char** items      = uiState->item[childId].list.items;
    int          itemCount  = uiState->item[childId].list.itemCount;
    float        textScale  = uiState->item[childId].list.textScale;
    float        itemH      = uiState->item[childId].list.itemHeight;
    uint16_t     wid        = uiState->item[childId].list.widgetId;
    TextureData* fontAtlas  = uiState->item[childId].list.font;
    bool         showScroll = uiState->item[childId].list.showScroll;
    float        charWidth  = (fontAtlas ? (float)fontAtlas[0].size.x : 8.0f) * textScale;
    float        charHeight = (fontAtlas ? (float)fontAtlas[0].size.y : 8.0f) * textScale;

    Vector2Float mousePos = GetMouseWorldPos(uiState);
    bool mouseOverList =
        PointInRect(uiState, mousePos, (Rectangle){ childBounds.x, childBounds.y, childBounds.w, childBounds.h });

    float listScroll  = uiState->scrollOffset[wid];
    float totalHeight = itemCount * itemH;
    float maxScroll   = totalHeight - childBounds.h;
    if (maxScroll < 0)
        maxScroll = 0;

    if (mouseOverList)
        listScroll -= Platform_GetMouseWheelMove() * 30.0f;
    if (listScroll < 0)
        listScroll = 0;
    if (listScroll > maxScroll)
        listScroll = maxScroll;
    uiState->scrollOffset[wid] = listScroll;

    float itemWidth      = childBounds.w;
    float scrollbarWidth = 8;
    if (showScroll && maxScroll > 0)
        itemWidth = childBounds.w - scrollbarWidth;

    for (int i = 0; i < itemCount; i++)
    {
        float iy = childBounds.y + i * itemH - listScroll;
        if (iy + itemH <= childBounds.y || iy >= childBounds.y + childBounds.h)
            continue;

        Rectangle itemRect = { childBounds.x, iy, itemWidth, itemH };
        bool      hover    = mouseOverList && PointInRect(uiState, mousePos, itemRect);

        Color bgCol = (i == uiState->listSelected[wid]) ? uiState->theme.listSelected : (Color){ 0, 0, 0, 0 };
        if (hover && i != uiState->listSelected[wid])
            bgCol = uiState->theme.listHover;

        if (bgCol.a > 0)
            DrawFrameShape(uiState, (Vector4Float){ itemRect.x, itemRect.y, itemRect.width, itemRect.height }, bgCol);

        const char* text       = items[i];
        size_t      textLength = strlen(text);
        float       dx         = childBounds.x + 4;
        float       dy         = iy + (itemH - charHeight) * 0.5f;

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
            s.parent     = uiState->parentEntity;
            s.isVisible  = true;
            PushSprite(uiState, s);
        }

        if (hover && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
            uiState->listSelected[wid] = i;
    }

    if (showScroll && maxScroll > 0)
    {
        float sbx = childBounds.x + itemWidth;
        float sby = childBounds.y;
        DrawFrameShape(uiState, (Vector4Float){ sbx, sby, scrollbarWidth, childBounds.h }, uiState->theme.sliderTrack);

        float thumbH = (childBounds.h / totalHeight) * childBounds.h;
        float thumbY = sby + (listScroll / totalHeight) * childBounds.h;
        DrawBevel(uiState, (Vector4Float){ sbx, thumbY, scrollbarWidth, thumbH }, uiState->theme.sliderThumb, true);
    }
}

static void DrawToggleItem(UI_State* uiState, int childId, Vector4Float childBounds, UI_CenterType childCenter,
                           float scrollY)
{
    (void)scrollY;
    (void)childCenter;

    const char*  text      = uiState->item[childId].button.text;
    float        scale     = uiState->item[childId].button.scale;
    uint16_t     wid       = uiState->item[childId].button.widgetId;
    TextureData* fontAtlas = uiState->item[childId].button.font;

    float  charWidth  = (fontAtlas ? (float)fontAtlas[0].size.x : 8.0f) * scale;
    float  charHeight = (fontAtlas ? (float)fontAtlas[0].size.y : 8.0f) * scale;
    size_t textLength = strlen(text);
    float  textWidth  = textLength * charWidth;
    float  textHeight = charHeight;

    float btnW = childBounds.w;
    float btnH = childBounds.h;

    float bx = childBounds.x + (childBounds.w - btnW) * 0.5f;
    float by = childBounds.y + (childBounds.h - btnH) * 0.5f;

    Rectangle    rect     = { bx, by, btnW, btnH };
    Vector2Float mousePos = GetMouseWorldPos(uiState);
    bool         hovered  = PointInRect(uiState, mousePos, rect);
    bool         pressed  = Input_IsMouseButtonDown(INPUT_MOUSE_BUTTON_LEFT) && hovered;

    bool  active = uiState->toggleState[wid];
    Color col;
    if (active)
        col = hovered ? uiState->theme.toggleActiveHover : uiState->theme.toggleActive;
    else
        col = hovered ? uiState->theme.toggleHover : uiState->theme.toggleInactive;
    if (pressed)
        col = uiState->theme.toggleActive;

    DrawBevel(uiState, (Vector4Float){ rect.x, rect.y, rect.width, rect.height }, col, !active);

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
        s.parent     = uiState->parentEntity;
        s.isVisible  = true;
        PushSprite(uiState, s);
    }

    if (hovered && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
        uiState->buttonClicked[wid] = true;
}

static void DrawSeparatorItem(UI_State* uiState, int childId, Vector4Float childBounds, UI_CenterType childCenter,
                              float scrollY)
{
    (void)scrollY;
    (void)childId;
    (void)childCenter;

    float midY = childBounds.y + childBounds.h * 0.5f;

    Shape2D line;
    Shape2D_Initialize(&line);
    line.type               = SHAPE2D_LINE;
    line.position.x         = childBounds.x;
    line.position.y         = midY;
    line.line.endPosition.x = childBounds.x + childBounds.w;
    line.line.endPosition.y = midY;
    line.line.thickness     = 1.0f;
    line.color              = uiState->theme.separatorColor;
    line.parent             = uiState->parentEntity;
    PushShape(uiState, line);
}

static void DrawTileGridItem(UI_State* uiState, int childId, Vector4Float childBounds, UI_CenterType childCenter,
                             float scrollY)
{
    (void)scrollY;
    (void)childCenter;

    TextureData* textures     = uiState->item[childId].tileGrid.textures;
    int          textureCount = uiState->item[childId].tileGrid.textureCount;
    int          cols         = uiState->item[childId].tileGrid.cols;
    uint16_t     wid          = uiState->item[childId].tileGrid.widgetId;
    int          selectedTile = uiState->tileGridSelected[wid];

    float cellSize    = childBounds.w / cols;
    int   rows        = (textureCount + cols - 1) / cols;
    float totalHeight = rows * cellSize;

    float gridScroll = uiState->scrollOffset[wid];

    Vector2Float mp        = GetMouseWorldPos(uiState);
    Rectangle    frameRect = { childBounds.x, childBounds.y, childBounds.w, childBounds.h };
    bool         hovered   = PointInRect(uiState, mp, frameRect);

    float wheel = Platform_GetMouseWheelMove();
    if (hovered && wheel != 0.0f)
    {
        gridScroll -= wheel * cellSize;
        float maxScroll = totalHeight - childBounds.h;
        if (maxScroll < 0)
            maxScroll = 0;
        if (gridScroll < 0)
            gridScroll = 0;
        if (gridScroll > maxScroll)
            gridScroll = maxScroll;
        uiState->scrollOffset[wid] = gridScroll;
    }

    for (int i = 0; i < textureCount; i++)
    {
        int row = i / cols;
        int col = i % cols;

        float bx = childBounds.x + col * cellSize;
        float by = childBounds.y + row * cellSize - gridScroll;

        if (by < childBounds.y || by > childBounds.y + childBounds.h)
        {
            continue;
        }

        Sprite sprite;
        Sprite_Initialize(&sprite);
        sprite.currentTexture = &textures[i];
        sprite.scale          = cellSize / sprite.currentTexture->size.x;
        sprite.position.x     = bx;
        sprite.position.y     = by;
        sprite.parent         = uiState->parentEntity;
        sprite.isVisible      = true;

        if (i == selectedTile)
        {
            sprite.tint = (Color){ 120, 200, 255, 255 };
        }

        PushSprite(uiState, sprite);

        if (hovered && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
        {
            Rectangle cellRect = { bx, by, cellSize, cellSize };
            if (PointInRect(uiState, mp, cellRect))
            {
                if (i == selectedTile)
                    uiState->tileGridSelected[wid] = -1;
                else
                    uiState->tileGridSelected[wid] = i;
            }
        }
    }
}

static void DrawGraph1DItem(UI_State* uiState, int childId, Vector4Float childBounds, UI_CenterType childCenter,
                            float scrollY)
{
    (void)scrollY;
    (void)childCenter;

    Color colors[12] = { COLOR_GREEN,   COLOR_BLUE, COLOR_RED,   COLOR_YELLOW, COLOR_ORANGE,    COLOR_PURPLE,
                         COLOR_SKYBLUE, COLOR_PINK, COLOR_BROWN, COLOR_GRAY,   COLOR_DARKGREEN, COLOR_DARKBLUE };

    UI_Graph1DData* graph    = &uiState->item[childId].graph1d;
    float           maxValue = graph->values[0];
    float           minValue = graph->values[0];
    for (int i = 0; i < graph->valueCount - 1; i++)
    {
        if (graph->values[i] > maxValue)
            maxValue = graph->values[i];
        if (graph->values[i] < minValue)
            minValue = graph->values[i];
    }

    if (graph->type == Graph1dHorizontal)
    {
        // Draw horizontal bars for each value make it percent of maxValue
        for (int i = 0; i < graph->valueCount - 1; i++)
        {
            float x0 = childBounds.x;
            float y0 = childBounds.y + ((float)i / (graph->valueCount - 1)) * childBounds.h;
            float x1 = childBounds.x + (graph->values[i] / maxValue) * childBounds.w;
            float y1 = childBounds.y + ((float)(i + 1) / (graph->valueCount - 1)) * childBounds.h;
            // float x0 = childBounds.x + ((float)i / (graph->valueCount - 1)) * childBounds.w;
            // float y0 = childBounds.y + childBounds.h - (graph->values[i] / maxValue) * childBounds.h;
            // float x1 = childBounds.x + ((float)(i + 1) / (graph->valueCount - 1)) * childBounds.w;
            // float y1 = childBounds.y + childBounds.h - (graph->values[i + 1] / maxValue) * childBounds.h;

            Shape2D rect;
            Shape2D_Initialize(&rect);
            rect.type             = SHAPE2D_RECTANGLE;
            rect.position.x       = x0;
            rect.position.y       = y0;
            rect.rectangle.width  = x1 - x0;
            rect.rectangle.height = y1 - y0;
            rect.color            = colors[i % 12];
            rect.parent           = uiState->parentEntity;
            PushShape(uiState, rect);
        }
    }
    else if (graph->type == Graph1dVertical)
    {
        // Draw vertical bars for each value make it percent of maxValue
        for (int i = 0; i < graph->valueCount - 1; i++)
        {
            float x0 = childBounds.x + ((float)i / (graph->valueCount - 1)) * childBounds.w;
            float y0 = childBounds.y + childBounds.h - (graph->values[i] / maxValue) * childBounds.h;
            float x1 = childBounds.x + ((float)(i + 1) / (graph->valueCount - 1)) * childBounds.w;
            float y1 = childBounds.y + childBounds.h;

            Shape2D rect;
            Shape2D_Initialize(&rect);
            rect.type             = SHAPE2D_RECTANGLE;
            rect.position.x       = x0;
            rect.position.y       = y0;
            rect.rectangle.width  = x1 - x0;
            rect.rectangle.height = y1 - y0;
            rect.color            = colors[i % 12];
            rect.parent           = uiState->parentEntity;
            PushShape(uiState, rect);
        }
    }
}

static void DrawGraph2DItem(UI_State* uiState, int childId, Vector4Float childBounds, UI_CenterType childCenter,
                            float scrollY)
{
    (void)scrollY;
    (void)childCenter;

    UI_Graph2DData* graph     = &uiState->item[childId].graph2d;
    float           minValueX = graph->values[0].x;
    float           maxValueX = graph->values[0].x;
    float           minValueY = graph->values[0].y;
    float           maxValueY = graph->values[0].y;
    for (int i = 0; i < graph->valueCount; i++)
    {
        if (graph->values[i].x < minValueX)
            minValueX = graph->values[i].x;
        if (graph->values[i].x > maxValueX)
            maxValueX = graph->values[i].x;
        if (graph->values[i].y < minValueY)
            minValueY = graph->values[i].y;
        if (graph->values[i].y > maxValueY)
            maxValueY = graph->values[i].y;
    }
    if (graph->type == Graph2dBar)
    {
        for (int i = 0; i < graph->valueCount; i++)
        {
            float x0 = childBounds.x + ((graph->values[i].x - minValueX) / (maxValueX - minValueX)) * childBounds.w;
            float y0 = childBounds.y + childBounds.h
                       - ((graph->values[i].y - minValueY) / (maxValueY - minValueY)) * childBounds.h;
            float x1 = childBounds.x + ((graph->values[i].x - minValueX) / (maxValueX - minValueX)) * childBounds.w;
            float y1 = childBounds.y + childBounds.h;

            Shape2D rect;
            Shape2D_Initialize(&rect);
            rect.type             = SHAPE2D_RECTANGLE;
            rect.position.x       = x0;
            rect.position.y       = y0;
            rect.rectangle.width  = x1 - x0;
            rect.rectangle.height = y1 - y0;
            rect.color            = uiState->theme.graphBarColor;
            rect.parent           = uiState->parentEntity;
            PushShape(uiState, rect);
        }
    }
    else if (graph->type == Graph2dLine)
    {
        for (int i = 0; i < graph->valueCount - 1; i++)
        {
            float x0 = childBounds.x + ((graph->values[i].x - minValueX) / (maxValueX - minValueX)) * childBounds.w;
            float y0 = childBounds.y + childBounds.h
                       - ((graph->values[i].y - minValueY) / (maxValueY - minValueY)) * childBounds.h;
            float x1 = childBounds.x + ((graph->values[i + 1].x - minValueX) / (maxValueX - minValueX)) * childBounds.w;
            float y1 = childBounds.y + childBounds.h
                       - ((graph->values[i + 1].y - minValueY) / (maxValueY - minValueY)) * childBounds.h;

            Shape2D line;
            Shape2D_Initialize(&line);
            line.type               = SHAPE2D_LINE;
            line.position.x         = x0;
            line.position.y         = y0;
            line.line.endPosition.x = x1;
            line.line.endPosition.y = y1;
            line.line.thickness     = 2.0f;
            line.color              = uiState->theme.graphLineColor;
            line.parent             = uiState->parentEntity;
            PushShape(uiState, line);
        }
    }
    else if (graph->type == Graph2dPoint)
    {
        for (int i = 0; i < graph->valueCount; i++)
        {
            float x = childBounds.x + ((graph->values[i].x - minValueX) / (maxValueX - minValueX)) * childBounds.w;
            float y = childBounds.y + childBounds.h
                      - ((graph->values[i].y - minValueY) / (maxValueY - minValueY)) * childBounds.h;

            Shape2D point;
            Shape2D_Initialize(&point);
            point.type          = SHAPE2D_CIRCLE;
            point.position.x    = x;
            point.position.y    = y;
            point.circle.radius = 3.0f;
            point.color         = uiState->theme.graphPointColor;
            point.parent        = uiState->parentEntity;
            PushShape(uiState, point);
        }
    }
}

static bool IsChildRenderable(UI_StackType type)
{
    return type == StackType_Text || type == StackType_Sprite || type == StackType_Button || type == StackType_Slider
           || type == StackType_List || type == StackType_Toggle || type == StackType_TileGrid
           || type == StackType_Separator || type == StackType_Graph1D || type == StackType_Graph2D;
}

static void DispatchChild(UI_State* uiState, int childId, Vector4Float childBounds, UI_CenterType childCenter,
                          float scrollY, Vector4Float frameBounds)
{
    switch (uiState->item[childId].type)
    {
        case StackType_Text:
            DrawTextItem(uiState, childId, childBounds, childCenter, scrollY);
            break;
        case StackType_Sprite:
            DrawSpriteItem(uiState, childId, childBounds, childCenter, scrollY);
            break;
        case StackType_Button:
            DrawButtonItem(uiState, childId, childBounds, childCenter, scrollY);
            break;
        case StackType_Slider:
            DrawSliderItem(uiState, childId, childBounds, childCenter, scrollY);
            break;
        case StackType_List:
        {
            float right = Utils_MinFloat(childBounds.x + childBounds.w, frameBounds.x + frameBounds.w);
            float bottom = Utils_MinFloat(childBounds.y + childBounds.h, frameBounds.y + frameBounds.h);
            Vector4Float clipBounds;
            clipBounds.x = Utils_MaxFloat(childBounds.x, frameBounds.x);
            clipBounds.y = Utils_MaxFloat(childBounds.y, frameBounds.y);
            clipBounds.w = Utils_MaxFloat(0.0f, right - clipBounds.x);
            clipBounds.h = Utils_MaxFloat(0.0f, bottom - clipBounds.y);
            PushLocalClip(uiState, clipBounds);
            DrawListItem(uiState, childId, childBounds, childCenter, scrollY);
            RenderStack_PopClip(uiState->renderStack);
            break;
        }
        case StackType_Toggle:
            DrawToggleItem(uiState, childId, childBounds, childCenter, scrollY);
            break;
        case StackType_Separator:
            DrawSeparatorItem(uiState, childId, childBounds, childCenter, scrollY);
            break;
        case StackType_TileGrid:
            DrawTileGridItem(uiState, childId, childBounds, childCenter, scrollY);
            break;
        case StackType_Graph1D:
            DrawGraph1DItem(uiState, childId, childBounds, childCenter, scrollY);
            break;
        case StackType_Graph2D:
            DrawGraph2DItem(uiState, childId, childBounds, childCenter, scrollY);
            break;
        default:
            return;
    }
}

bool UI_IsMouseOverBounds(UI_State* uiState, Vector4Float bounds)
{
    Vector2Float mp = GetMouseWorldPos(uiState);
    return PointInRect(uiState, mp, (Rectangle){ bounds.x, bounds.y, bounds.w, bounds.h });
}

void UI_End(UI_State* uiState)
{
    if (uiState->itemCount == 0)
        return;

    uint16_t containerIdx[UI_MAX_STACK_DEPTH];
    int      containerCount = 0;
    for (uint16_t i = 0; i < uiState->itemCount; i++)
    {
        if (uiState->item[i].type == StackType_Frame)
            containerIdx[containerCount++] = i;
    }

    if (containerCount == 0)
    {
        uiState->itemCount = 0;
        return;
    }

    UI_LayoutType rootLayout = LayoutVertical;
    for (int i = 0; i < containerIdx[0]; i++)
    {
        if (uiState->item[i].type == StackType_Layout)
            rootLayout = uiState->item[i].layout;
    }

    Vector4Float root = uiState->bounds;

    int   autoCount  = 0;
    float fixedTotal = 0;
    for (int f = 0; f < containerCount; f++)
    {
        int   cur = containerIdx[f];
        float s   = uiState->item[cur].frame.size;
        if (s > 0)
            fixedTotal += s;
        else
        {
            autoCount++;
        }
    }

    float remainingRatio = 1.0f - fixedTotal;
    float autoUnit       = autoCount > 0 ? remainingRatio / autoCount : 0;

    float cursor = rootLayout == LayoutVertical ? root.y : root.x;
    float span   = rootLayout == LayoutVertical ? root.h : root.w;

    Vector2Float mousePos = GetMouseWorldPos(uiState);

    for (int f = 0; f < containerCount; f++)
    {
        int current = containerIdx[f];
        int next    = (f + 1 < containerCount) ? containerIdx[f + 1] : uiState->itemCount;

        float frameSize = uiState->item[current].frame.size > 0 ? uiState->item[current].frame.size : autoUnit;
        float frameSpan = frameSize * span;

        Vector4Float frameBounds;
        if (rootLayout == LayoutVertical)
            frameBounds = (Vector4Float){ root.x, cursor, root.w, frameSpan };
        else
            frameBounds = (Vector4Float){ cursor, root.y, frameSpan, root.h };
        cursor += frameSpan;

        PushLocalClip(uiState, frameBounds);
        DrawBevel(uiState, frameBounds, uiState->theme.frameBg, true);

        bool mouseOverFrame =
            PointInRect(uiState, mousePos, (Rectangle){ frameBounds.x, frameBounds.y, frameBounds.w, frameBounds.h });
        uiState->mouseCaptured |= mouseOverFrame;

        float scrollY = 0;
        if (uiState->item[current].frame.scrollable)
        {
            uint16_t wid = uiState->item[current].frame.widgetId;
            scrollY      = uiState->scrollOffset[wid];

            if (mouseOverFrame)
            {
                float wheel = Platform_GetMouseWheelMove();
                scrollY -= wheel * 30.0f;
                float maxScroll = uiState->item[current].frame.contentHeight - frameBounds.h;
                if (maxScroll < 0)
                    maxScroll = 0;
                if (scrollY < 0)
                    scrollY = 0;
                if (scrollY > maxScroll)
                    scrollY = maxScroll;
            }
            uiState->scrollOffset[wid] = scrollY;
        }

        int childStart = current + 1;
        int childEnd   = next;

        UI_LayoutType childLayout = LayoutVertical;
        UI_CenterType childCenter = CenterNone;
        int           childCount  = 0;
        for (int j = childStart; j < childEnd; j++)
        {
            if (uiState->item[j].type == StackType_Layout)
            {
                childLayout = uiState->item[j].layout;
            }
            else if (uiState->item[j].type == StackType_Centering)
            {
                childCenter = uiState->item[j].centeringMode;
            }
            else if (uiState->item[j].type == StackType_Padding)
            {
                continue;
            }
            else
            {
                childCount++;
            }
        }

        if (childCount == 0)
        {
            RenderStack_PopClip(uiState->renderStack);
            continue;
        }

        float slotW      = (childLayout == LayoutHorizontal) ? frameBounds.w / childCount : frameBounds.w;
        float slotH      = (childLayout == LayoutVertical) ? frameBounds.h / childCount : frameBounds.h;
        int   childIndex = 0;

        for (int childId = childStart; childId < childEnd; childId++)
        {
            if (!IsChildRenderable(uiState->item[childId].type))
                continue;

            Vector4Float childPadding = { 0, 0, 0, 0 };
            for (int k = childId; k > childStart; k--)
            {
                if (uiState->item[k].type == StackType_Padding)
                {
                    childPadding = uiState->item[k].padding;
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
            childBounds.w -= childPadding.w + childPadding.x;
            childBounds.h -= childPadding.h + childPadding.h;

            DispatchChild(uiState, childId, childBounds, childCenter, scrollY, frameBounds);
            childIndex++;
        }
        RenderStack_PopClip(uiState->renderStack);
    }
    uiState->itemCount = 0;
}

Vector4Float UI_GetBounds(UI_State* uiState, UI_AnchorType anchor, Vector4Float position)
{
    float aspect = uiState->window->height;
    if (uiState->window->width < uiState->window->height)
    {
        aspect = uiState->window->width;
    }
    switch (anchor)
    {
        case AnchorTopLeft:
        {
            Vector4Float bounds;
            bounds.x = (uiState->window->width * -0.5f) + (aspect * position.x);
            bounds.y = (uiState->window->height * -0.5f) + (aspect * position.y);
            bounds.w = (aspect * position.w);
            bounds.h = (aspect * position.h);
            return bounds;
        }
        case AnchorTopCenter:
        {
            Vector4Float bounds;
            bounds.x = -(aspect * position.x);
            bounds.y = (uiState->window->height * -0.5f) + (aspect * position.y);
            bounds.w = (aspect * position.w);
            bounds.h = (aspect * position.h);
            return bounds;
        }
        case AnchorTopRight:
        {
            Vector4Float bounds;
            bounds.x = (uiState->window->width * 0.5f) - (aspect * position.w) - (aspect * position.x);
            bounds.y = (uiState->window->height * -0.5f) + (aspect * position.y);
            bounds.w = (aspect * position.w);
            bounds.h = (aspect * position.h);
            return bounds;
        }
        case AnchorMiddleLeft:
        {
            Vector4Float bounds;
            bounds.x = (uiState->window->width * -0.5f) + (aspect * position.x);
            bounds.y = -(aspect * position.y);
            bounds.w = (aspect * position.w);
            bounds.h = (aspect * position.h);
            return bounds;
        }
        case AnchorMiddleCenter:
        {
            Vector4Float bounds;
            bounds.x = -(aspect * position.x);
            bounds.y = -(aspect * position.y);
            bounds.w = (aspect * position.w);
            bounds.h = (aspect * position.h);
            return bounds;
        }
        case AnchorMiddleRight:
        {
            Vector4Float bounds;
            bounds.x = (uiState->window->width * 0.5f) - (aspect * position.w) - (aspect * position.x);
            bounds.y = -(aspect * position.y);
            bounds.w = (aspect * position.w);
            bounds.h = (aspect * position.h);
            return bounds;
        }
        case AnchorBottomLeft:
        {
            Vector4Float bounds;
            bounds.x = (uiState->window->width * -0.5f) + (aspect * position.x);
            bounds.y = (uiState->window->height * 0.5f) - (aspect * position.h) - (aspect * position.y);
            bounds.w = (aspect * position.w);
            bounds.h = (aspect * position.h);
            return bounds;
        }
        case AnchorBottomCenter:
        {
            Vector4Float bounds;
            bounds.x = -(aspect * position.x);
            bounds.y = (uiState->window->height * 0.5f) - (aspect * position.h) - (aspect * position.y);
            bounds.w = (aspect * position.w);
            bounds.h = (aspect * position.h);
            return bounds;
        }
        case AnchorBottomRight:
        {
            Vector4Float bounds;
            bounds.x = (uiState->window->width * 0.5f) - (aspect * position.w) - (aspect * position.x);
            bounds.y = (uiState->window->height * 0.5f) - (aspect * position.h) - (aspect * position.y);
            bounds.w = (aspect * position.w);
            bounds.h = (aspect * position.h);
            return bounds;
        }
    }
    return (Vector4Float){ 0, 0, 0, 0 };
}

Vector4Float UI_GetSize(UI_State* uiState, Vector4Float position)
{
    float aspect = uiState->window->height;
    if (uiState->window->width < uiState->window->height)
    {
        aspect = uiState->window->width;
    }
    Vector4Float bounds;
    bounds.x = aspect * position.x;
    bounds.y = aspect * position.y;
    bounds.w = aspect * position.w;
    bounds.h = aspect * position.h;
    return bounds;
}
bool UI_GetMouseCaptured(UI_State* uiState)
{
    return uiState->mouseCaptured;
}
