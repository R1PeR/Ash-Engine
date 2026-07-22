#include "MapEditorMode.h"

#include "MainMode.h"
#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"
#include "utils/UI.h"

#include <raylib.h>
#include <stdio.h>
#include <string.h>

#define TILESET_ATLAS_COLS MAP_TILESET_COLS
#define TILESET_ATLAS_ROWS MAP_TILESET_ROWS
#define TILESET_COUNT      MAP_TILESET_COUNT
#define FONT_ATLAS_COLS    16
#define FONT_ATLAS_ROWS    16
#define FONT_GLYPH_COUNT   (FONT_ATLAS_COLS * FONT_ATLAS_ROWS)
#define DRAWABLE_MAX       4096
#define TILE_DRAW_SCALE    2.0f
#define PANE_TILE_PX       18.0f
#define PANE_TILE_COLS     16
#define TEX_PANE_W         300.0f
#define INFO_PANE_W        215.0f
#define INFO_PANE_H        147.0f
#define ZOOM_MIN           0.1f
#define ZOOM_MAX           10.0f
#define ZOOM_STEP          0.15f
#define MAP_SAVE_FILE      "map.txt"
#define GRID_COLOR         ((Color){ 45, 45, 45, 255 })

Mode              mapEditorMode       = MODE_FROM_CLASSNAME(MapEditorMode);
EditorTestMapData g_editorTestMapData = {};

static Drawable drawables[DRAWABLE_MAX];
static size_t   drawableCount = 0;

static TextureData tileTextures[TILESET_COUNT];
static TextureData tileAtlasBase;

static TextureData fontTextures[FONT_GLYPH_COUNT];
static TextureData fontAtlasBase;

static Entity2D cameraEntity;

struct EditorData
{
    MapData  mapData;
    uint8_t  activeLayer  = 0;
    int32_t  selectedTile = -1;
    TileType tileType     = TILE_TYPE_SOLID;
    bool     showTypes    = false;
    bool     showGrid     = true;
    bool     isErasing    = false;
} data;

static Vector4Float g_texturePaneBounds = { 0, 0, 0, 0 };
static Vector4Float g_infoPaneBounds    = { 0, 0, 0, 0 };

void HandleCameraInput();
void DrawGrid();
void DrawWorldTiles();
void HandleTilePlacement();
void HandleKeyboardShortcuts();
void DrawTexturePane();
void DrawInfoPane();
void EraseTileAt(Vector3Int gridPos);
void PlaceTileAt(Vector3Int gridPos);
void SaveMap(const char* filename);
void LoadMap(const char* filename);
void TestInMainMode();

void EraseTileAt(Vector3Int gridPos)
{
    TileLayer* layer = &data.mapData.layers[data.activeLayer];
    for (uint16_t i = 0; i < layer->tileCount; i++)
    {
        if (layer->tiles[i].position.x == gridPos.x && layer->tiles[i].position.y == gridPos.y)
        {
            layer->tiles[i] = layer->tiles[--layer->tileCount];
            return;
        }
    }
}

void PlaceTileAt(Vector3Int gridPos)
{
    if (data.selectedTile < 0)
        return;
    TileLayer* layer = &data.mapData.layers[data.activeLayer];
    for (uint16_t i = 0; i < layer->tileCount; i++)
    {
        if (layer->tiles[i].position.x == gridPos.x && layer->tiles[i].position.y == gridPos.y)
        {
            layer->tiles[i].textureId = (uint16_t)data.selectedTile;
            layer->tiles[i].type      = data.tileType;
            return;
        }
    }
    if (layer->tileCount < MAP_MAX_TILES)
    {
        Tile* tile       = &layer->tiles[layer->tileCount++];
        tile->position.x = gridPos.x;
        tile->position.y = gridPos.y;
        tile->textureId  = (uint16_t)data.selectedTile;
        tile->type       = data.tileType;
    }
    else
    {
        LOG_ERR("MapEditor: layer %d is full", data.activeLayer);
    }
}


void HandleCameraInput()
{
    float wheel = GetMouseWheelMove();

    if (wheel != 0.0f)
    {
        bool mouseOverUI = UI_IsMouseOverBounds(g_texturePaneBounds) || UI_IsMouseOverBounds(g_infoPaneBounds);
        if (!mouseOverUI)
        {
            Vector2Float mw = Utils_ScreenToWorld2D(
                (Vector2Float){ (float)Input_GetMouseX(), (float)Input_GetMouseY() }, *Window_GetCamera());
            float oldZoom = Window_GetCamera()->zoom;
            Window_GetCamera()->zoom += wheel * ZOOM_STEP;
            if (Window_GetCamera()->zoom < ZOOM_MIN)
                Window_GetCamera()->zoom = ZOOM_MIN;
            if (Window_GetCamera()->zoom > ZOOM_MAX)
                Window_GetCamera()->zoom = ZOOM_MAX;
            float ratio                  = Window_GetCamera()->zoom / oldZoom;
            Window_GetCamera()->target.x = mw.x - (mw.x - Window_GetCamera()->target.x) / ratio;
            Window_GetCamera()->target.y = mw.y - (mw.y - Window_GetCamera()->target.y) / ratio;
        }
    }

    if (Input_IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        float dx = (float)Input_GetMouseDeltaX() / Window_GetCamera()->zoom;
        float dy = (float)Input_GetMouseDeltaY() / Window_GetCamera()->zoom;
        Window_GetCamera()->target.x -= dx;
        Window_GetCamera()->target.y -= dy;
    }
}

void DrawGrid()
{
    if (!data.showGrid)
        return;
    Camera2D*    cam      = Window_GetCamera();
    Vector2Float startPos = Utils_ScreenToWorld2D((Vector2Float){ 0.0f, 0.0f }, *cam);
    Vector2Float endPos =
        Utils_ScreenToWorld2D((Vector2Float){ (float)Window_GetWidth(), (float)Window_GetHeight() }, *cam);
    int   startX    = ((int)(startPos.x / TILE_SIZE) - 1) * TILE_SIZE;
    int   endX      = ((int)(endPos.x / TILE_SIZE) + 1) * TILE_SIZE;
    int   startY    = ((int)(startPos.y / TILE_SIZE) - 1) * TILE_SIZE;
    int   endY      = ((int)(endPos.y / TILE_SIZE) + 1) * TILE_SIZE;
    float thickness = 1.0f / cam->zoom;
    for (int x = startX; x <= endX; x += TILE_SIZE)
        DrawLineEx((Vector2){ (float)x, (float)startY }, (Vector2){ (float)x, (float)endY }, thickness, GRID_COLOR);
    for (int y = startY; y <= endY; y += TILE_SIZE)
        DrawLineEx((Vector2){ (float)startX, (float)y }, (Vector2){ (float)endX, (float)y }, thickness, GRID_COLOR);
}

void DrawWorldTiles()
{
    // Camera2D camera = *Window_GetCamera();

    for (uint8_t l = 0; l < MAP_MAX_LAYERS; l++)
    {
        TileLayer* layer = &data.mapData.layers[l];
        for (uint16_t i = 0; i < layer->tileCount; i++)
        {
            Tile*   tile                  = &layer->tiles[i];
            Sprite* sprite                = &drawables[drawableCount].sprite;
            drawables[drawableCount].type = DRAWABLE_SPRITE;
            drawableCount++;
            Sprite_Initialize(sprite);
            sprite->scale          = TILE_DRAW_SCALE;
            sprite->position.x     = (float)(tile->position.x * TILE_SIZE);
            sprite->position.y     = (float)(tile->position.y * TILE_SIZE);
            sprite->currentTexture = &tileTextures[tile->textureId];
            sprite->zOrder         = l;

            if (l != data.activeLayer)
                sprite->tint.a = 80;

            if (data.showTypes && l == data.activeLayer)
            {
                switch (tile->type)
                {
                    case TILE_TYPE_SOLID:
                        sprite->tint = (Color){ 255, 150, 150, 255 };
                        break;
                    case TILE_TYPE_JUMP_PLATFORM:
                        sprite->tint = (Color){ 150, 255, 150, 255 };
                        break;
                    case TILE_TYPE_PLAYER_SPAWN:
                        sprite->tint = (Color){ 150, 150, 255, 255 };
                        break;
                    case TILE_TYPE_ENEMY_SPAWN:
                        sprite->tint = (Color){ 255, 255, 150, 255 };
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void HandleTilePlacement()
{
    if (UI_IsMouseOverBounds(g_texturePaneBounds) || UI_IsMouseOverBounds(g_infoPaneBounds))
        return;

    Camera2D     camera = *Window_GetCamera();
    Vector2Float mouseWorldPos =
        Utils_ScreenToWorld2D((Vector2Float){ (float)Input_GetMouseX(), (float)Input_GetMouseY() }, camera);
    Vector3Int gridPos = Utils_WorldToGrid(mouseWorldPos, TILE_SIZE);

    /* Ghost preview */
    if (data.selectedTile >= 0 && !data.isErasing)
    {
        Sprite* ghost                 = &drawables[drawableCount].sprite;
        drawables[drawableCount].type = DRAWABLE_SPRITE;
        drawableCount++;
        Sprite_Initialize(ghost);
        ghost->scale          = TILE_DRAW_SCALE;
        ghost->position.x     = (float)(gridPos.x * TILE_SIZE);
        ghost->position.y     = (float)(gridPos.y * TILE_SIZE);
        ghost->currentTexture = &tileTextures[data.selectedTile];
        ghost->tint           = (Color){ 255, 255, 255, 130 };
    }

    if (Input_IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (data.isErasing)
            EraseTileAt(gridPos);
        else if (data.selectedTile >= 0)
            PlaceTileAt(gridPos);
    }
}

void DrawTexturePane()
{
    Camera2D* cam = Window_GetCamera();
    float     sw  = (float)Window_GetWidth();
    float     sh  = (float)Window_GetHeight();
    float     hw  = sw * 0.5f;
    float     hh  = sh * 0.5f;
    float     s   = 1.0f / cam->zoom;
    float     tx  = cam->target.x;
    float     ty  = cam->target.y;

    float px = tx + (hw - TEX_PANE_W) * s;
    float py = ty - hh * s;
    float pw = TEX_PANE_W * s;
    float ph = sh * s;

    g_texturePaneBounds = (Vector4Float){ px, py, pw, ph };

    static const char* typeLabels[] = { "EMPT", "SOLI", "JUMP", "PLYR", "ENMY" };

    UI_Begin(g_texturePaneBounds);
    {
        UI_FrameSize(0.08f);
        UI_Layout(LayoutHorizontal);
        for (int l = 0; l < MAP_MAX_LAYERS; l++)
        {
            char label[8];
            snprintf(label, sizeof(label), "L%d", l);
            if (UI_Toggle(label, l == data.activeLayer, s, fontTextures))
                data.activeLayer = (uint8_t)l;
        }

        UI_FrameSize(0.08f);
        UI_Layout(LayoutHorizontal);
        for (int t = 0; t < 5; t++)
        {
            if (UI_Toggle(typeLabels[t], data.tileType == (TileType)t, s, fontTextures))
                data.tileType = (TileType)t;
        }

        UI_FrameSize(0.02f);
        UI_Separator();

        UI_Frame();
        data.selectedTile = UI_TileGrid(tileTextures, TILESET_COUNT, PANE_TILE_COLS, data.selectedTile);
    }
    UI_End();
}

void DrawInfoPane()
{
    Camera2D* cam = Window_GetCamera();
    float     hw  = (float)Window_GetWidth() * 0.5f;
    float     hh  = (float)Window_GetHeight() * 0.5f;
    float     s   = 1.0f / cam->zoom;
    float     tx  = cam->target.x;
    float     ty  = cam->target.y;

    float px = tx - hw * s;
    float py = ty - hh * s;
    float pw = INFO_PANE_W * s;
    float ph = INFO_PANE_H * s;

    g_infoPaneBounds = (Vector4Float){ px, py, pw, ph };

    static const char* typeNames[] = { "EMPTY", "SOLID", "JUMP", "PSPAWN", "ESPAWN" };

    char buf[64];
    UI_Begin(g_infoPaneBounds);
    UI_Frame();
    UI_Layout(LayoutVertical);
    {
        snprintf(buf, sizeof(buf), "LAYER: %d  (PGUP/PGDN)", data.activeLayer);
        UI_Text(buf, s, fontTextures);
        snprintf(buf, sizeof(buf), "TYPE:  %s  (1-5)", typeNames[data.tileType]);
        UI_Text(buf, s, fontTextures);
        snprintf(buf, sizeof(buf), "TILE:  %d", data.selectedTile);
        UI_Text(buf, s, fontTextures);
        snprintf(buf, sizeof(buf), "ZOOM:  %.2fx  (WHEEL)", cam->zoom);
        UI_Text(buf, s, fontTextures);
        UI_Text(data.isErasing ? "MODE:  ERASE  (E)" : "MODE:  DRAW   (E)", s, fontTextures);
        UI_Text(data.showTypes ? "TYPES: ON  (T)" : "TYPES: OFF (T)", s, fontTextures);
        UI_Text(data.showGrid ? "GRID:  ON  (G)" : "GRID:  OFF (G)", s, fontTextures);
        UI_Text("F2:SAVE  F3:LOAD  F9:CLR", s, fontTextures);
        UI_Text("F5:TEST  (PAN:RMB)", s, fontTextures);
    }
    UI_End();
}

void HandleKeyboardShortcuts()
{
    if (Input_IsKeyPressed(KEY_T))
        data.showTypes = !data.showTypes;
    if (Input_IsKeyPressed(KEY_G))
        data.showGrid = !data.showGrid;
    if (Input_IsKeyPressed(KEY_E))
        data.isErasing = !data.isErasing;

    if (Input_IsKeyPressed(KEY_ONE))
        data.tileType = TILE_TYPE_EMPTY;
    if (Input_IsKeyPressed(KEY_TWO))
        data.tileType = TILE_TYPE_SOLID;
    if (Input_IsKeyPressed(KEY_THREE))
        data.tileType = TILE_TYPE_JUMP_PLATFORM;
    if (Input_IsKeyPressed(KEY_FOUR))
        data.tileType = TILE_TYPE_PLAYER_SPAWN;
    if (Input_IsKeyPressed(KEY_FIVE))
        data.tileType = TILE_TYPE_ENEMY_SPAWN;

    if (Input_IsKeyPressed(KEY_PAGE_UP) && data.activeLayer + 1 < MAP_MAX_LAYERS)
        data.activeLayer++;
    if (Input_IsKeyPressed(KEY_PAGE_DOWN) && data.activeLayer > 0)
        data.activeLayer--;

    if (Input_IsKeyPressed(KEY_F2))
        SaveMap(MAP_SAVE_FILE);
    if (Input_IsKeyPressed(KEY_F3))
        LoadMap(MAP_SAVE_FILE);
    if (Input_IsKeyPressed(KEY_F5))
        TestInMainMode();
    if (Input_IsKeyPressed(KEY_F9))
    {
        for (int l = 0; l < MAP_MAX_LAYERS; l++)
            data.mapData.layers[l].tileCount = 0;
    }
}

void SaveMap(const char* filename)
{
    FILE* f = fopen(filename, "w");
    if (!f)
    {
        LOG_ERR("MapEditor: cannot open '%s' for writing", filename);
        return;
    }
    fprintf(f, "VER 1\n");
    fprintf(f, "TILESIZE %d\n", TILE_SIZE);
    fprintf(f, "LAYERS %d\n", MAP_MAX_LAYERS);
    for (int l = 0; l < MAP_MAX_LAYERS; l++)
    {
        TileLayer* layer = &data.mapData.layers[l];
        fprintf(f, "LAYER %d\n", l);
        fprintf(f, "COUNT %d\n", (int)layer->tileCount);
        for (int i = 0; i < (int)layer->tileCount; i++)
        {
            Tile* tile = &layer->tiles[i];
            fprintf(f, "%d %d %d %d\n", tile->position.x, tile->position.y, (int)tile->textureId, (int)tile->type);
        }
    }
    fclose(f);
    LOG_INF("MapEditor: saved to '%s'", filename);
}

void LoadMap(const char* filename)
{
    FILE* f = fopen(filename, "r");
    if (!f)
    {
        LOG_ERR("MapEditor: cannot open '%s' for reading", filename);
        return;
    }
    for (int l = 0; l < MAP_MAX_LAYERS; l++)
        data.mapData.layers[l].tileCount = 0;

    char line[128];
    int  currentLayer = -1;
    while (fgets(line, sizeof(line), f))
    {
        int layerIdx;
        if (sscanf(line, "LAYER %d", &layerIdx) == 1)
        {
            currentLayer = layerIdx;
            continue;
        }
        if (currentLayer >= 0 && currentLayer < MAP_MAX_LAYERS)
        {
            int x, y, texId, type;
            if (sscanf(line, "%d %d %d %d", &x, &y, &texId, &type) == 4)
            {
                TileLayer* layer = &data.mapData.layers[currentLayer];
                if (layer->tileCount < MAP_MAX_TILES)
                {
                    Tile* tile       = &layer->tiles[layer->tileCount++];
                    tile->position.x = x;
                    tile->position.y = y;
                    tile->textureId  = (uint16_t)texId;
                    tile->type       = (TileType)type;
                }
            }
        }
    }
    fclose(f);
    LOG_INF("MapEditor: loaded from '%s'", filename);
}

void TestInMainMode()
{
    g_editorTestMapData.isValid        = true;
    g_editorTestMapData.hasPlayerSpawn = false;
    g_editorTestMapData.mapData        = data.mapData;

    for (int l = 0; l < MAP_MAX_LAYERS && !g_editorTestMapData.hasPlayerSpawn; l++)
    {
        TileLayer* layer = &g_editorTestMapData.mapData.layers[l];
        for (int i = 0; i < (int)layer->tileCount; i++)
        {
            if (layer->tiles[i].type == TILE_TYPE_PLAYER_SPAWN)
            {
                g_editorTestMapData.playerSpawn    = layer->tiles[i].position;
                g_editorTestMapData.hasPlayerSpawn = true;
                break;
            }
        }
    }
    Context_SetMode(&mainMode);
}

void MapEditorMode_OnStart()
{
    tileAtlasBase = Texture_LoadTexture("resources/sprites/tileset.png");
    LOG_INF("MapEditor: tile atlas %dx%d", tileAtlasBase.size.x, tileAtlasBase.size.y);
    if (!Texture_CreateTextureAtlas(tileAtlasBase, TILESET_ATLAS_COLS, TILESET_ATLAS_ROWS, tileTextures))
        LOG_ERR("MapEditor: failed to create tile atlas");

    fontAtlasBase = Texture_LoadTexture("resources/sprites/Anikki_square_8x8.png");
    if (!Texture_CreateTextureAtlas(fontAtlasBase, FONT_ATLAS_COLS, FONT_ATLAS_ROWS, fontTextures))
        LOG_ERR("MapEditor: failed to create font atlas");

    Entity2D_Initialize(&cameraEntity);

    Camera2D* camera = Window_GetCamera();
    camera->zoom     = 1.0f;
    camera->target   = (Vector2){ 0.0f, 0.0f };

    cameraEntity.position.x = camera->target.x;
    cameraEntity.position.y = camera->target.y;
    cameraEntity.scale      = 1.0f / camera->zoom;

    UI_Initialize(drawables, (size_t*)&drawableCount, DRAWABLE_MAX);
    UI_SetParentEntity(&cameraEntity);
}

void MapEditorMode_OnPause()
{
}

void DrawTest()
{
    Vector4Float bounds = { -200, -200, 400, 200 };
    UI_Begin(bounds);
    {
        UI_Frame();
        UI_Layout(LayoutVertical);
        UI_Center(CenterVertical);
        UI_Padding({ 20, 20, 20, 20 });
        UI_Text("Hello, World!", 1.0f, fontTextures);
        UI_Padding({ 10, 10, 10, 10 });
        UI_Text("This is a test.", 1.0f, fontTextures);
        UI_Padding({ 5, 5, 5, 5 });
        UI_Text("This is a test.", 1.0f, fontTextures);
    }
    UI_End();

    bounds = { -200, 100, 400, 200 };
    UI_Begin(bounds);
    {
        UI_Frame();
        UI_Layout(LayoutVertical);
        UI_Center(CenterVertical);
        UI_Padding({ 0, 0, 0, 0 });
        UI_Text("Hello, World!", 1.0f, fontTextures);
        UI_Padding({ 0, 0, 0, 0 });
        UI_Text("This is a test.", 1.0f, fontTextures);
        UI_Padding({ 0, 0, 0, 0 });
        UI_Text("This is a test.", 1.0f, fontTextures);
    }
    UI_End();
}

void MapEditorMode_Update()
{
    cameraEntity.position.x = Window_GetCamera()->target.x;
    cameraEntity.position.y = Window_GetCamera()->target.y;
    cameraEntity.scale      = 1.0f / Window_GetCamera()->zoom;

    drawableCount = 0;
    DeltaTime_Update();

    // uint32_t screenW = Window_GetWidth();
    // uint32_t screenH = Window_GetHeight();
    // texPane.position = (Vector2Float){ (float)screenW - TEX_PANE_W, 0.0f };
    // texPane.size     = (Vector2Float){ TEX_PANE_W, (float)screenH };

    HandleTilePlacement();
    HandleKeyboardShortcuts();


    DrawGrid();
    DrawTest();
    // DrawWorldTiles();
    // DrawTexturePane();
    // DrawInfoPane();


    for (size_t i = 0; i < drawableCount; i++)
        Drawable_Draw(&drawables[i]);

    HandleCameraInput();
}

void MapEditorMode_OnStop()
{
}

void MapEditorMode_OnResume()
{
}
