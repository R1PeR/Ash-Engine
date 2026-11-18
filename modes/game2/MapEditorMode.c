#include "MapEditorMode.h"

#include "MainMode.h"
#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"
#include "ashes/ash_platform.h"
#include "utils/UI.h"

#include <stdio.h>
#include <string.h>

EditorTestMapData g_editorTestMapData = {};

void HandleCameraInput(MapEditorMode* state);
void UpdateGrid(MapEditorMode* state);
void UpdateWorldTiles(MapEditorMode* state);
void HandleTilePlacement(MapEditorMode* state);
void HandleKeyboardShortcuts(MapEditorMode* state);
void UpdateTexturePane(MapEditorMode* state);
void UpdateInfoPane(MapEditorMode* state);
void EraseTileAt(Vector3Int gridPos);
void PlaceTileAt(Vector3Int gridPos);
void SaveMap(MapEditorMode* state, const char* filename);
void LoadMap(MapEditorMode* state, const char* filename);
void TestInMainMode(MapEditorMode* state);

void EraseTileAt(MapEditorMode* state, Vector3Int gridPos)
{
    TileLayer* layer = &state->data.mapData.layers[state->data.activeLayer];
    for (uint16_t i = 0; i < layer->tileCount; i++)
    {
        if (layer->tiles[i].position.x == gridPos.x && layer->tiles[i].position.y == gridPos.y)
        {
            layer->tiles[i] = layer->tiles[--layer->tileCount];
            return;
        }
    }
}

void PlaceTileAt(MapEditorMode* state, Vector3Int gridPos)
{
    if (state->data.selectedTile < 0)
        return;
    TileLayer* layer = &state->data.mapData.layers[state->data.activeLayer];
    for (uint16_t i = 0; i < layer->tileCount; i++)
    {
        if (layer->tiles[i].position.x == gridPos.x && layer->tiles[i].position.y == gridPos.y)
        {
            layer->tiles[i].textureId = (uint16_t)state->data.selectedTile;
            layer->tiles[i].type      = state->data.tileType;
            return;
        }
    }
    if (layer->tileCount < MAP_MAX_TILES)
    {
        Tile* tile       = &layer->tiles[layer->tileCount++];
        tile->position.x = gridPos.x;
        tile->position.y = gridPos.y;
        tile->textureId  = (uint16_t)state->data.selectedTile;
        tile->type       = state->data.tileType;
    }
    else
    {
        LOG_ERR("MapEditor: layer %d is full", state->data.activeLayer);
    }
}


void HandleCameraInput(MapEditorMode* state)
{
    if (state->data.uiInputHandled)
        return;

    float wheel = Platform_GetMouseWheelMove();

    if (wheel != 0.0f)
    {
        Vector2Float mw =
            Utils_ScreenToWorld2D((Vector2Float){ (float)Input_GetMouseX(), (float)Input_GetMouseY() }, state->camera);
        float oldZoom = state->camera.zoom;
        state->camera.zoom += wheel * ZOOM_STEP;
        if (state->camera.zoom < ZOOM_MIN)
            state->camera.zoom = ZOOM_MIN;
        if (state->camera.zoom > ZOOM_MAX)
            state->camera.zoom = ZOOM_MAX;
        float ratio              = state->camera.zoom / oldZoom;
        state->camera.position.x = mw.x - (mw.x - state->camera.position.x) / ratio;
        state->camera.position.y = mw.y - (mw.y - state->camera.position.y) / ratio;
    }

    if (Input_IsMouseButtonDown(INPUT_MOUSE_BUTTON_RIGHT))
    {
        float dx = (float)Input_GetMouseDeltaX() / state->camera.zoom;
        float dy = (float)Input_GetMouseDeltaY() / state->camera.zoom;
        state->camera.position.x -= dx;
        state->camera.position.y -= dy;
    }
}

void UpdateGrid(MapEditorMode* state)
{
    if (!state->data.showGrid)
        return;
    Camera2D*    cam      = &state->camera;
    Vector2Float startPos = Utils_ScreenToWorld2D((Vector2Float){ 0.0f, 0.0f }, *cam);
    Vector2Float endPos =
        Utils_ScreenToWorld2D((Vector2Float){ (float)state->window->width, (float)state->window->height }, *cam);
    int   startX    = ((int)(startPos.x / TILE_SIZE) - 1) * TILE_SIZE;
    int   endX      = ((int)(endPos.x / TILE_SIZE) + 1) * TILE_SIZE;
    int   startY    = ((int)(startPos.y / TILE_SIZE) - 1) * TILE_SIZE;
    int   endY      = ((int)(endPos.y / TILE_SIZE) + 1) * TILE_SIZE;
    float thickness = 1.0f / cam->zoom;
    for (int x = startX; x <= endX; x += TILE_SIZE)
    {
        Shape2D line;
        Shape2D_Initialize(&line);
        line.type               = SHAPE2D_LINE;
        line.position.x         = x;
        line.position.y         = startY;
        line.line.endPosition.x = x;
        line.line.endPosition.y = endY;
        line.line.thickness     = thickness;
        line.color              = GRID_COLOR;
        RenderStack_PushShape(&state->renderStack, line);
    }
    for (int y = startY; y <= endY; y += TILE_SIZE)
    {
        Shape2D line;
        Shape2D_Initialize(&line);
        line.type               = SHAPE2D_LINE;
        line.position.x         = startX;
        line.position.y         = y;
        line.line.endPosition.x = endX;
        line.line.endPosition.y = y;
        line.line.thickness     = thickness;
        line.color              = GRID_COLOR;
        RenderStack_PushShape(&state->renderStack, line);
    }
}

void UpdateWorldTiles(MapEditorMode* state)
{
    for (uint8_t l = 0; l < MAP_MAX_LAYERS; l++)
    {
        TileLayer* layer = &state->data.mapData.layers[l];
        for (uint16_t i = 0; i < layer->tileCount; i++)
        {
            Tile*  tile = &layer->tiles[i];
            Sprite sprite;
            Sprite_Initialize(&sprite);
            sprite.scale          = TILE_DRAW_SCALE;
            sprite.position.x     = (float)(tile->position.x * TILE_SIZE);
            sprite.position.y     = (float)(tile->position.y * TILE_SIZE);
            sprite.currentTexture = &state->tileTextures[tile->textureId];

            if (l != state->data.activeLayer)
                sprite.tint.a = 80;

            if (state->data.showTypes && l == state->data.activeLayer)
            {
                switch (tile->type)
                {
                    case TILE_TYPE_SOLID:
                        sprite.tint = (Color){ 255, 150, 150, 255 };
                        break;
                    case TILE_TYPE_JUMP_PLATFORM:
                        sprite.tint = (Color){ 150, 255, 150, 255 };
                        break;
                    case TILE_TYPE_PLAYER_SPAWN:
                        sprite.tint = (Color){ 150, 150, 255, 255 };
                        break;
                    case TILE_TYPE_ENEMY_SPAWN:
                        sprite.tint = (Color){ 255, 255, 150, 255 };
                        break;
                    default:
                        break;
                }
            }
            RenderStack_PushSprite(&state->renderStack, sprite);
        }
    }
}

void HandleTilePlacement(MapEditorMode* state)
{
    if (state->data.uiInputHandled)
        return;

    Vector2Float mouseWorldPos =
        Utils_ScreenToWorld2D((Vector2Float){ (float)Input_GetMouseX(), (float)Input_GetMouseY() }, state->camera);
    Vector3Int gridPos = Utils_WorldToGrid(mouseWorldPos, TILE_SIZE);

    /* Ghost preview */
    if (state->data.selectedTile >= 0 && !state->data.isErasing)
    {
        Sprite ghost;
        Sprite_Initialize(&ghost);
        ghost.scale          = TILE_DRAW_SCALE;
        ghost.position.x     = (float)(gridPos.x * TILE_SIZE);
        ghost.position.y     = (float)(gridPos.y * TILE_SIZE);
        ghost.currentTexture = &state->tileTextures[state->data.selectedTile];
        ghost.tint           = (Color){ 255, 255, 255, 130 };
        RenderStack_PushSprite(&state->renderStack, ghost);
    }

    if (Input_IsMouseButtonDown(INPUT_MOUSE_BUTTON_LEFT))
    {
        if (state->data.isErasing)
            EraseTileAt(state, gridPos);
        else if (state->data.selectedTile >= 0)
            PlaceTileAt(state, gridPos);
    }
}

void UpdateTexturePane(MapEditorMode* state)
{
    static const char* typeLabels[] = { "EMPT", "SOLI", "JUMP", "PLYR", "ENMY" };
    char               label[MAP_MAX_LAYERS][8];

    UI_Begin(&state->uiState, UI_GetBounds(&state->uiState, AnchorTopRight, { 0.0, 0.0, 0.5, 1.0 }));
    {
        UI_FrameSize(&state->uiState, 0.2f);
        UI_Layout(&state->uiState, LayoutHorizontal);
        UI_Center(&state->uiState, CenterBoth);
        UI_Padding(&state->uiState, UI_GetSize(&state->uiState, { 0.01, 0.01, 0.01, 0.01 }));
        for (int l = 0; l < MAP_MAX_LAYERS; l++)
        {
            snprintf(label[l], sizeof(label), "L%d", l);
            if (UI_Toggle(&state->uiState, label[l], l == state->data.activeLayer, 1.0, state->fontTextures))
            {
                state->data.activeLayer = (uint8_t)l;
            }
        }

        UI_FrameSize(&state->uiState, 0.05f);
        UI_Layout(&state->uiState, LayoutVertical);
        UI_Separator(&state->uiState);

        UI_FrameSize(&state->uiState, 0.2f);
        UI_Layout(&state->uiState, LayoutHorizontal);
        UI_Center(&state->uiState, CenterBoth);
        UI_Padding(&state->uiState, UI_GetSize(&state->uiState, { 0.01, 0.01, 0.01, 0.01 }));
        for (int t = 0; t < 5; t++)
        {
            if (UI_Toggle(&state->uiState, typeLabels[t], state->data.tileType == (TileType)t, 1.0,
                          state->fontTextures))
            {
                state->data.tileType = (TileType)t;
            }
        }

        UI_Frame(&state->uiState);
        state->data.selectedTile =
            UI_TileGrid(&state->uiState, state->tileTextures, TILESET_COUNT, PANE_TILE_COLS, state->data.selectedTile);
    }
    UI_End(&state->uiState);

    state->data.uiInputHandled |= UI_GetMouseCaptured(&state->uiState);
}

void UpdateInfoPane(MapEditorMode* state)
{
    Camera2D*          camera      = &state->camera;
    static const char* typeNames[] = { "EMPTY", "SOLID", "JUMP", "PSPAWN", "ESPAWN" };

    char layer[32];
    snprintf(layer, sizeof(layer), "LAYER: %d  (PGUP/PGDN)", state->data.activeLayer);
    char type[32];
    snprintf(type, sizeof(type), "TYPE:  %-6s  (1-5)", typeNames[state->data.tileType]);
    char tile[32];
    snprintf(tile, sizeof(tile), "TILE:  %-1d", state->data.selectedTile);
    char zoom[32];
    snprintf(zoom, sizeof(zoom), "ZOOM:  %2.2fx  (WHEEL)", camera->zoom);

    UI_Begin(&state->uiState, UI_GetBounds(&state->uiState, AnchorTopLeft, { 0.0, 0.0, 0.3, 0.3 }));
    {
        UI_Frame(&state->uiState);
        UI_Layout(&state->uiState, LayoutVertical);
        UI_Center(&state->uiState, CenterVertical);
        UI_Padding(&state->uiState, { 5, 5, 5, 5 });
        UI_Text(&state->uiState, layer, 1.0, state->fontTextures);
        UI_Text(&state->uiState, type, 1.0, state->fontTextures);
        UI_Text(&state->uiState, tile, 1.0, state->fontTextures);
        UI_Text(&state->uiState, zoom, 1.0, state->fontTextures);
        UI_Text(&state->uiState, state->data.isErasing ? "MODE:  ERASE  (E)" : "MODE:  DRAW   (E)", 1.0,
                state->fontTextures);
        UI_Text(&state->uiState, state->data.showTypes ? "TYPES: ON  (T)" : "TYPES: OFF (T)", 1.0, state->fontTextures);
        UI_Text(&state->uiState, state->data.showGrid ? "GRID:  ON  (G)" : "GRID:  OFF (G)", 1.0, state->fontTextures);
        UI_Text(&state->uiState, "F2:SAVE  F3:LOAD  F9:CLR", 1.0, state->fontTextures);
        UI_Text(&state->uiState, "F5:TEST  (PAN:RMB)", 1.0, state->fontTextures);
    }
    UI_End(&state->uiState);
    state->data.uiInputHandled |= UI_GetMouseCaptured(&state->uiState);
}

void HandleKeyboardShortcuts(MapEditorMode* state)
{
    if (Input_IsKeyPressed(INPUT_KEYCODE_T))
        state->data.showTypes = !state->data.showTypes;
    if (Input_IsKeyPressed(INPUT_KEYCODE_G))
        state->data.showGrid = !state->data.showGrid;
    if (Input_IsKeyPressed(INPUT_KEYCODE_E))
        state->data.isErasing = !state->data.isErasing;

    if (Input_IsKeyPressed(INPUT_KEYCODE_ONE))
        state->data.tileType = TILE_TYPE_EMPTY;
    if (Input_IsKeyPressed(INPUT_KEYCODE_TWO))
        state->data.tileType = TILE_TYPE_SOLID;
    if (Input_IsKeyPressed(INPUT_KEYCODE_THREE))
        state->data.tileType = TILE_TYPE_JUMP_PLATFORM;
    if (Input_IsKeyPressed(INPUT_KEYCODE_FOUR))
        state->data.tileType = TILE_TYPE_PLAYER_SPAWN;
    if (Input_IsKeyPressed(INPUT_KEYCODE_FIVE))
        state->data.tileType = TILE_TYPE_ENEMY_SPAWN;

    if (Input_IsKeyPressed(INPUT_KEYCODE_PAGE_UP) && state->data.activeLayer + 1 < MAP_MAX_LAYERS)
        state->data.activeLayer++;
    if (Input_IsKeyPressed(INPUT_KEYCODE_PAGE_DOWN) && state->data.activeLayer > 0)
        state->data.activeLayer--;

    if (Input_IsKeyPressed(INPUT_KEYCODE_F2))
        SaveMap(state, MAP_SAVE_FILE);
    if (Input_IsKeyPressed(INPUT_KEYCODE_F3))
        LoadMap(state, MAP_SAVE_FILE);
    if (Input_IsKeyPressed(INPUT_KEYCODE_F5))
        TestInMainMode(state);
    if (Input_IsKeyPressed(INPUT_KEYCODE_F9))
    {
        for (int l = 0; l < MAP_MAX_LAYERS; l++)
            state->data.mapData.layers[l].tileCount = 0;
    }
}

void SaveMap(MapEditorMode* state, const char* filename)
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
        TileLayer* layer = &state->data.mapData.layers[l];
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

void LoadMap(MapEditorMode* state, const char* filename)
{
    FILE* f = fopen(filename, "r");
    if (!f)
    {
        LOG_ERR("MapEditor: cannot open '%s' for reading", filename);
        return;
    }
    for (int l = 0; l < MAP_MAX_LAYERS; l++)
        state->data.mapData.layers[l].tileCount = 0;

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
                TileLayer* layer = &state->data.mapData.layers[currentLayer];
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

void TestInMainMode(MapEditorMode* state)
{
    g_editorTestMapData.isValid        = true;
    g_editorTestMapData.hasPlayerSpawn = false;
    g_editorTestMapData.mapData        = state->data.mapData;

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
    // Context_SetMode(&mainMode);
}

void DrawTest(MapEditorMode* state)
{
    Vector4Float bounds = UI_GetBounds(&state->uiState, AnchorTopLeft, { 0.1f, 0.1f, 0.1f, 0.1f });
    UI_Begin(&state->uiState, bounds);
    {
        UI_Frame(&state->uiState);
        UI_Layout(&state->uiState, LayoutVertical);
        UI_Center(&state->uiState, CenterBoth);
        UI_Text(&state->uiState, "TopLeft", 1.0f, state->fontTextures);
    }
    UI_End(&state->uiState);

    bounds = UI_GetBounds(&state->uiState, AnchorTopCenter, { 0.05f, 0.1f, 0.1f, 0.1f });
    UI_Begin(&state->uiState, bounds);
    {
        UI_Frame(&state->uiState);
        UI_Layout(&state->uiState, LayoutVertical);
        UI_Center(&state->uiState, CenterBoth);
        UI_Text(&state->uiState, "TopCenter", 1.0f, state->fontTextures);
    }
    UI_End(&state->uiState);

    bounds = UI_GetBounds(&state->uiState, AnchorTopRight, { 0.1f, 0.1f, 0.1f, 0.1f });
    UI_Begin(&state->uiState, bounds);
    {
        UI_Frame(&state->uiState);
        UI_Layout(&state->uiState, LayoutVertical);
        UI_Center(&state->uiState, CenterBoth);
        UI_Text(&state->uiState, "TopRight", 1.0f, state->fontTextures);
    }
    UI_End(&state->uiState);

    bounds = UI_GetBounds(&state->uiState, AnchorMiddleLeft, { 0.1f, 0.05f, 0.1f, 0.1f });
    UI_Begin(&state->uiState, bounds);
    {
        UI_Frame(&state->uiState);
        UI_Layout(&state->uiState, LayoutVertical);
        UI_Center(&state->uiState, CenterBoth);
        UI_Text(&state->uiState, "MiddleLeft", 1.0f, state->fontTextures);
    }
    UI_End(&state->uiState);

    bounds = UI_GetBounds(&state->uiState, AnchorMiddleCenter, { 0.05f, 0.05f, 0.1f, 0.1f });
    UI_Begin(&state->uiState, bounds);
    {
        UI_Frame(&state->uiState);
        UI_Layout(&state->uiState, LayoutVertical);
        UI_Center(&state->uiState, CenterBoth);
        UI_Text(&state->uiState, "MiddleCenter", 1.0f, state->fontTextures);
    }
    UI_End(&state->uiState);

    bounds = UI_GetBounds(&state->uiState, AnchorMiddleRight, { 0.1f, 0.05f, 0.1f, 0.1f });
    UI_Begin(&state->uiState, bounds);
    {
        UI_Frame(&state->uiState);
        UI_Layout(&state->uiState, LayoutVertical);
        UI_Center(&state->uiState, CenterBoth);
        UI_Text(&state->uiState, "MiddleRight", 1.0f, state->fontTextures);
    }
    UI_End(&state->uiState);

    bounds = UI_GetBounds(&state->uiState, AnchorBottomLeft, { 0.1f, 0.1f, 0.1f, 0.1f });
    UI_Begin(&state->uiState, bounds);
    {
        UI_Frame(&state->uiState);
        UI_Layout(&state->uiState, LayoutVertical);
        UI_Center(&state->uiState, CenterBoth);
        UI_Text(&state->uiState, "BottomLeft", 1.0f, state->fontTextures);
    }
    UI_End(&state->uiState);

    bounds = UI_GetBounds(&state->uiState, AnchorBottomCenter, { 0.05f, 0.1f, 0.1f, 0.1f });
    UI_Begin(&state->uiState, bounds);
    {
        UI_Frame(&state->uiState);
        UI_Layout(&state->uiState, LayoutVertical);
        UI_Center(&state->uiState, CenterBoth);
        UI_Text(&state->uiState, "BottomCenter", 1.0f, state->fontTextures);
    }
    UI_End(&state->uiState);

    bounds = UI_GetBounds(&state->uiState, AnchorBottomRight, { 0.1f, 0.1f, 0.1f, 0.1f });
    UI_Begin(&state->uiState, bounds);
    {
        UI_Frame(&state->uiState);
        UI_Layout(&state->uiState, LayoutVertical);
        UI_Center(&state->uiState, CenterBoth);
        UI_Text(&state->uiState, "BottomRight", 1.0f, state->fontTextures);
    }
    UI_End(&state->uiState);
}

void MapEditorMode_OnStart(Context* context, void* data)
{
    (void)context;
    MapEditorMode* state = (MapEditorMode*)data;
    state->tileAtlasBase = Texture_LoadTexture("resources/sprites/tileset.png", state->arena);
    LOG_INF("MapEditor: tile atlas %dx%d", state->tileAtlasBase.size.x, state->tileAtlasBase.size.y);
    if (!Texture_CreateTextureAtlas(state->tileAtlasBase, TILESET_ATLAS_COLS, TILESET_ATLAS_ROWS, state->tileTextures))
        LOG_ERR("MapEditor: failed to create tile atlas");

    state->fontAtlasBase = Texture_LoadTexture("resources/sprites/Anikki_square_8x8.png", NULL);
    LOG_INF("MapEditor: font atlas %dx%d", state->fontAtlasBase.size.x, state->fontAtlasBase.size.y);
    if (!Texture_CreateTextureAtlas(state->fontAtlasBase, FONT_ATLAS_COLS, FONT_ATLAS_ROWS, state->fontTextures))
        LOG_ERR("MapEditor: failed to create font atlas");

    Entity2D_Initialize(&state->cameraEntity);

    state->camera.zoom     = 1.0f;
    state->camera.position = (Vector2Float){ 0.0f, 0.0f };
    state->camera.rotation = 0.0f;
    state->camera.offset   = (Vector2Float){ state->window->width / 2.0f, state->window->height / 2.0f };

    state->cameraEntity.position.x = state->camera.position.x;
    state->cameraEntity.position.y = state->camera.position.y;
    state->cameraEntity.scale      = 1.0f / state->camera.zoom;

    UI_Initialize(&state->uiState, &state->renderStack, state->window, &state->camera);
    UI_SetParentEntity(&state->uiState, &state->cameraEntity);
}

void MapEditorMode_OnPause(Context* context, void* data)
{
    (void)context;
    (void)data;
}

void MapEditorMode_Update(Context* context, void* data)
{
    (void)context;
    MapEditorMode* state        = (MapEditorMode*)data;
    state->renderStackItemCount = 0;
    state->data.uiInputHandled  = false;
    DeltaTime_Update(&state->deltaTime);

    RenderStack_Clear(&state->renderStack);
    RenderStack_PushClearScreen(&state->renderStack, (Color){ 0, 0, 0, 255 });
    RenderStack_PushCamera2d(&state->renderStack, &state->camera);
    RenderStack_PushMode2d(&state->renderStack);

    UpdateGrid(state);
    UpdateWorldTiles(state);
    UpdateTexturePane(state);
    UpdateInfoPane(state);
    // DrawTest();

    HandleTilePlacement(state);
    HandleKeyboardShortcuts(state);
    HandleCameraInput(state);

    state->cameraEntity.position.x = state->camera.position.x;
    state->cameraEntity.position.y = state->camera.position.y;
    state->cameraEntity.scale      = 1.0f / state->camera.zoom;

    RenderStack_PopMode2d(&state->renderStack);
    RenderStack_PopCamera2d(&state->renderStack);
}

void MapEditorMode_Draw(Context* context, void* data)
{
    (void)context;
    MapEditorMode* state = (MapEditorMode*)data;
    RenderStack_Draw(&state->renderStack);
}

void MapEditorMode_OnStop(Context* context, void* data)
{
    (void)context;
    (void)data;
}

void MapEditorMode_OnResume(Context* context, void* data)
{
    (void)context;
    (void)data;
}
