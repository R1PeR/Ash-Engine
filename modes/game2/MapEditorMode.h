
#ifndef LIBS_ENGINE_MAPEDITORMODE_H
#define LIBS_ENGINE_MAPEDITORMODE_H
#include "ashes/ash_context.h"
#include "ashes/ash_misc.h"

#include <stdint.h>

#define MAP_MAX_TILES     1024
#define MAP_MAX_LAYERS    4
#define TILE_SIZE         32
#define MAP_TILESET_COLS  24
#define MAP_TILESET_ROWS  16
#define MAP_TILESET_COUNT (MAP_TILESET_COLS * MAP_TILESET_ROWS)

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

extern Mode mapEditorMode;

void MapEditorMode_OnStart();
void MapEditorMode_OnPause();
void MapEditorMode_Update();
void MapEditorMode_OnStop();
void MapEditorMode_OnResume();

#endif  // LIBS_ENGINE_MAPEDITORMODE_H
