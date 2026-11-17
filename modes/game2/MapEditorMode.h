
#ifndef LIBS_ENGINE_MAPEDITORMODE_H
#define LIBS_ENGINE_MAPEDITORMODE_H
#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_misc.h"
#include "utils/UI.h"

#include <stdint.h>

#define MAP_MAX_TILES      1024
#define MAP_MAX_LAYERS     4
#define TILE_SIZE          32
#define MAP_TILESET_COLS   24
#define MAP_TILESET_ROWS   16
#define MAP_TILESET_COUNT  (MAP_TILESET_COLS * MAP_TILESET_ROWS)
#define TILESET_ATLAS_COLS MAP_TILESET_COLS
#define TILESET_ATLAS_ROWS MAP_TILESET_ROWS
#define TILESET_COUNT      MAP_TILESET_COUNT
#define FONT_ATLAS_COLS    16
#define FONT_ATLAS_ROWS    16
#define FONT_GLYPH_COUNT   (FONT_ATLAS_COLS * FONT_ATLAS_ROWS)
#define RENDERSTACKITEM_MAX       4096
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

enum TileType
{
    TILE_TYPE_EMPTY         = 0,
    TILE_TYPE_SOLID         = 1,
    TILE_TYPE_JUMP_PLATFORM = 2,
    TILE_TYPE_PLAYER_SPAWN  = 3,
    TILE_TYPE_ENEMY_SPAWN   = 4,
};

struct Tile
{
    Vector2Int position;
    uint16_t   textureId;
    TileType   type;
};

struct TileLayer
{
    Tile     tiles[MAP_MAX_TILES];
    uint16_t tileCount = 0;
};

struct MapData
{
    TileLayer layers[MAP_MAX_LAYERS];
};

struct EditorTestMapData
{
    MapData    mapData;
    bool       isValid        = false;
    Vector2Int playerSpawn    = { 0, 0 };
    bool       hasPlayerSpawn = false;
};

extern EditorTestMapData g_editorTestMapData;

struct MapEditorMode
{
    EditorTestMapData g_editorTestMapData = {};

    RenderStack     renderStack;
    RenderStackItem renderStackItems[RENDERSTACKITEM_MAX];
    size_t          renderStackItemCount = 0;

    TextureData tileTextures[TILESET_COUNT];
    TextureData tileAtlasBase;

    TextureData fontTextures[FONT_GLYPH_COUNT];
    TextureData fontAtlasBase;

    Entity2D cameraEntity;

    struct EditorData
    {
        MapData  mapData;
        uint8_t  activeLayer    = 0;
        int32_t  selectedTile   = -1;
        TileType tileType       = TILE_TYPE_SOLID;
        bool     showTypes      = false;
        bool     showGrid       = true;
        bool     isErasing      = false;
        bool     uiInputHandled = false;
    } data;

    UI_State  uiState;
    Window*   window;
    DeltaTime deltaTime;
    Arena*    arena;
    Camera2D  camera;
};

void MapEditorMode_OnStart(Context* context, void* data);
void MapEditorMode_OnPause(Context* context, void* data);
void MapEditorMode_Update(Context* context, void* data);
void MapEditorMode_Draw(Context* context, void* data);
void MapEditorMode_OnStop(Context* context, void* data);
void MapEditorMode_OnResume(Context* context, void* data);

#endif  // LIBS_ENGINE_MAPEDITORMODE_H
