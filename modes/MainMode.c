#include "MainMode.h"

#include "engine/components/Collider2D.h"
#include "engine/components/Entity2D.h"
#include "engine/components/Sprite.h"
#include "engine/io/Window.h"
#include "game/GameWindow.h"
#include "game/objects/Player.h"
#include "imgui.h"
#include "libs/engine/components/AsciiWindow.h"
#include "libs/engine/components/Audio.h"
#include "libs/engine/components/AudioPlayer.h"
#include "libs/engine/components/Texture.h"
#include "libs/engine/io/Input.h"
#include "libs/engine/misc/DeltaTime.h"
#include "libs/engine/misc/Logger.h"
#include "libs/engine/misc/Stopwatch.h"
#include "libs/engine/misc/Utils.h"
#include "raylib.h"
#include "rlImGui.h"
#include "utils/SpriteDefines.h"

#include <stdio.h>

#define MAX_LAYERS          8
#define OBJECT_MAX_POSITION 16
#define CHUNK_SIZE          16
#define CHUNK_MAX_OBJECTS   256
#define TEXTURE_SCALE       4
#define TEXTURE_SIZE        8
#define SPRITE_MAX_COUNT    256

Mode mainMode = MODE_FROM_CLASSNAME(MainMode);

struct Object
{
    enum Type : uint16_t
    {
        TILE = 0,
        ENTITY,
        PROJECTILE,
        EFFECT,
    };
    int         id;
    int         type;
    int         layer;
    uint32_t    textureId;
    bool        isCollidable;
    Vector2Int8 position;  // 0-16
    union
    {
        struct
        {
            // for PLAYER, ENEMY
            int entityHealth;
            int entityExperience;
            int entityLevel;
            int entityDamage;
            int entityAttackSpeed;
            int entityArmor;
            int entityStrength;
            int entityDexterity;
            int entityVitality;
            int entityEnergy;
            int entityItems[8];
        } entity;
        struct
        {
            // for PROJECTILE
            float   projectileSpeed;
            Vector2 projectileDirection;  // in degrees
            int     projectileRange;
            int     projectileDistanceTraveled;
            int     projectileDamage;
        } projectile;
        struct
        {
            // for EFFECT
            int   effectType;
            float effectDuration;
            float effectTimeElapsed;
        } effect;
    };
};

struct Chunk
{
    Vector3Int8 chunkPosition;
    Object      objects[CHUNK_MAX_OBJECTS];
    uint16_t    objectCount;
};

struct GameData
{
    Sprite   sprites[SPRITE_MAX_COUNT];
    Chunk    chunks[CHUNK_SIZE * CHUNK_SIZE];
    uint16_t chunkCount;
} gamedata;

struct Position
{
    Vector2Int8 gridPos;
    Vector3Int8 chunkPos;
};

struct Objects
{
    Object   objects[MAX_LAYERS];
    uint16_t count;
};
// Tu chyba się za dużo heapa alokuje :/
struct Chunks
{
    Chunk    chunks[CHUNK_SIZE * CHUNK_SIZE];
    uint16_t count;
};

struct DebugData
{
#define BUTTON_COUNT 256
    bool       isVisible;
    bool       isGridVisible;
    Texture2D* selectedTextureId;
    Object     currentObject;
    uint32_t   currentZPos;
} debugData;

void SaveChunksToFile(Chunk* chunks, uint16_t chunkCount)
{
    if (chunkCount == 0)
    {
        return;
    }
    FILE*    file  = NULL;
    uint32_t error = fopen_s(&file, "world.dat", "wb");
    if (error != 0 || file == NULL)
    {
        LOG_ERR("Failed to open world.dat for writing");
        return;
    }
    fwrite(&chunkCount, sizeof(uint16_t), 1, file);
    for (uint16_t i = 0; i < chunkCount; i++)
    {
        fwrite(&chunks[i], sizeof(Chunk), 1, file);
    }
    LOG_INF("Saved %d chunks to world.dat", chunkCount);
    fclose(file);
}

uint16_t LoadChunksFromFile(Chunk* chunks)
{
    uint16_t count = 0;
    LOG_INF("Loading chunks from world.dat");
    FILE*    file  = NULL;
    uint32_t error = fopen_s(&file, "world.dat", "rb");
    if (error != 0 || file == NULL)
    {
        LOG_ERR("Failed to open world.dat for reading");
        return 0;
    }
    fread(&count, sizeof(uint16_t), 1, file);
    if (count > CHUNK_SIZE * CHUNK_SIZE)
    {
        LOG_ERR("Invalid chunk count in world.dat");
        count = 0;
        fclose(file);
        return count;
    }
    for (uint16_t i = 0; i < count; i++)
    {
        fread(&chunks[i], sizeof(Chunk), 1, file);
    }
    LOG_INF("Loaded %d chunks from world.dat", count);
    fclose(file);
    return count;
}

Position worldToGridAndChunk(Vector2 pos)
{
    Position position;
    position.gridPos.x  = (int)(pos.x / TEXTURE_SIZE / TEXTURE_SCALE) % OBJECT_MAX_POSITION;
    position.chunkPos.x = (pos.x / TEXTURE_SIZE / TEXTURE_SCALE) / CHUNK_SIZE;
    position.gridPos.y  = (int)(pos.y / TEXTURE_SIZE / TEXTURE_SCALE) % OBJECT_MAX_POSITION;
    position.chunkPos.y = (pos.y / TEXTURE_SIZE / TEXTURE_SCALE) / CHUNK_SIZE;
    if (pos.x < 0)
    {
        position.gridPos.x += OBJECT_MAX_POSITION - 1;
        position.chunkPos.x -= 1;
    }
    if (pos.y < 0)
    {
        position.gridPos.y += OBJECT_MAX_POSITION - 1;
        position.chunkPos.y -= 1;
    }
    position.chunkPos.z = debugData.currentZPos;
    return position;
}

Vector2 gridAndChunkToWorld(Position pos)
{
    Vector2 position;
    position.x = (pos.chunkPos.x * CHUNK_SIZE + pos.gridPos.x) * TEXTURE_SIZE * TEXTURE_SCALE;
    position.y = (pos.chunkPos.y * CHUNK_SIZE + pos.gridPos.y) * TEXTURE_SIZE * TEXTURE_SCALE;
    return position;
}

Objects getObjectsAtPosition(Position pos)
{
    Objects objs;
    objs.count = 0;
    for (uint16_t i = 0; i < gamedata.chunkCount; i++)
    {
        Chunk* chunk = &gamedata.chunks[i];
        if (chunk->chunkPosition.x == pos.chunkPos.x && chunk->chunkPosition.y == pos.chunkPos.y)
        {
            for (uint16_t j = 0; j < chunk->objectCount; j++)
            {
                Object* obj = &chunk->objects[j];
                if (obj->position.x == pos.gridPos.x && obj->position.y == pos.gridPos.y)
                {
                    if (objs.count < MAX_LAYERS)
                    {
                        objs.objects[objs.count++] = *obj;
                    }
                    else
                    {
                        LOG_ERR("Max layers reached at position (%d, %d) in chunk (%d, %d)", pos.gridPos.x,
                                pos.gridPos.y, pos.chunkPos.x, pos.chunkPos.y);
                    }
                }
            }
        }
    }
    return objs;
}

void DrawDebug()
{
    if (Input_IsKeyPressed(INPUT_KEYCODE_F1))
    {
        debugData.isVisible = !debugData.isVisible;
    }
    if (Input_IsKeyPressed(INPUT_KEYCODE_F2))
    {
        debugData.isGridVisible = !debugData.isGridVisible;
    }
    if (debugData.isGridVisible)
    {
        for (int y = -5; y < 5; y++)
        {
            for (int x = -5; x < 5; x++)
            {
                DrawRectangleLines(
                    CHUNK_SIZE * TEXTURE_SIZE * TEXTURE_SCALE * x, CHUNK_SIZE * TEXTURE_SIZE * TEXTURE_SCALE * y,
                    CHUNK_SIZE * TEXTURE_SIZE * TEXTURE_SCALE, CHUNK_SIZE * TEXTURE_SIZE * TEXTURE_SCALE, RED);
            }
        }
    }
    if (debugData.isVisible)
    {
        ImGui::SetNextWindowPos(ImVec2(50, 20), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(500, 680), ImGuiCond_Always);

        ImGuiWindowFlags window_flags = 0;

        if (!ImGui::Begin("Select View", nullptr, window_flags))
        {
            ImGui::End();
            return;
        }

        ImGui::Text("Select a texture to paint with:");
        for (int i = 0; i < BUTTON_COUNT; i++)
        {
            ImGui::PushID(i);
            if (rlImGuiImageButtonSize("", Texture_GetTextureById(i + 3), { 32, 32 }))
            {
                debugData.selectedTextureId = Texture_GetTextureById(i + 3);
            }
            ImGui::PopID();
            if ((i + 1) % 10 != 0)
            {
                ImGui::SameLine();
            }
        }

        ImGui::NewLine();

        if (debugData.selectedTextureId != NULL)
        {
            ImGui::Text("Selected Texture:");
            rlImGuiImageSize(debugData.selectedTextureId, 64, 64);
        }
        else
        {
            ImGui::Text("No Texture Selected");
        }

        ImGui::InputInt("Id", &debugData.currentObject.id);
        ImGui::InputInt("Layer", (int*)&debugData.currentObject.layer);
        ImGui::Checkbox("Collidable", &debugData.currentObject.isCollidable);

        const char* items[] = { "TILE", "ENTITY", "PROJECTILE", "EFFECT" };
        ImGui::ListBox("Type", &debugData.currentObject.type, items, IM_ARRAYSIZE(items), 4);

        if (debugData.currentObject.type == Object::Type::ENTITY)
        {
            ImGui::InputInt("Health", &debugData.currentObject.entity.entityHealth);
            ImGui::InputInt("Damage", &debugData.currentObject.entity.entityDamage);
            ImGui::InputInt4("Items1", debugData.currentObject.entity.entityItems);
            ImGui::InputInt4("Items2", debugData.currentObject.entity.entityItems);
            ImGui::InputInt("Experience", &debugData.currentObject.entity.entityExperience);
            ImGui::InputInt("Level", &debugData.currentObject.entity.entityLevel);
            ImGui::InputInt("Attack Speed", &debugData.currentObject.entity.entityAttackSpeed);
            ImGui::InputInt("Armor", &debugData.currentObject.entity.entityArmor);
            ImGui::InputInt("Strength", &debugData.currentObject.entity.entityStrength);
            ImGui::InputInt("Dexterity", &debugData.currentObject.entity.entityDexterity);
            ImGui::InputInt("Vitality", &debugData.currentObject.entity.entityVitality);
            ImGui::InputInt("Energy", &debugData.currentObject.entity.entityEnergy);
        }
        else if (debugData.currentObject.type == Object::Type::PROJECTILE)
        {
            ImGui::InputFloat("Speed", &debugData.currentObject.projectile.projectileSpeed);
            ImGui::InputFloat2("Direction", (float*)&debugData.currentObject.projectile.projectileDirection);
            ImGui::InputInt("Range", &debugData.currentObject.projectile.projectileRange);
            ImGui::InputInt("Damage", &debugData.currentObject.projectile.projectileDamage);
        }
        else if (debugData.currentObject.type == Object::Type::EFFECT)
        {
            ImGui::InputInt("Effect Type", &debugData.currentObject.effect.effectType);
            ImGui::InputFloat("Duration", &debugData.currentObject.effect.effectDuration);
        }

        Vector2     mousePos = { (float)(Input_GetMouseX()), (float)(Input_GetMouseY()) };
        Vector2     worldPos = GetScreenToWorld2D(mousePos, *Window_GetCamera());
        Position    pos      = worldToGridAndChunk(worldPos);
        Vector2Int8 gridPos  = pos.gridPos;
        Vector3Int8 chunkPos = pos.chunkPos;

        ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
        ImGui::Text("World Position: (%.1f, %.1f)", worldPos.x, worldPos.y);
        ImGui::Text("Grid Position: (%d, %d)", gridPos.x, gridPos.y);
        ImGui::Text("Chunk Position: (%d, %d)", chunkPos.x, chunkPos.y);

        ImGui::End();

        if (Input_IsKeyDown(INPUT_KEYCODE_W) || Input_IsKeyDown(INPUT_KEYCODE_S) || Input_IsKeyDown(INPUT_KEYCODE_A)
            || Input_IsKeyDown(INPUT_KEYCODE_D) || Input_IsKeyDown(INPUT_KEYCODE_Q) || Input_IsKeyDown(INPUT_KEYCODE_E))
        {
            int8_t y = (Input_IsKeyDown(INPUT_KEYCODE_W) - Input_IsKeyDown(INPUT_KEYCODE_S));
            int8_t x = (Input_IsKeyDown(INPUT_KEYCODE_A) - Input_IsKeyDown(INPUT_KEYCODE_D));
            int8_t z = (Input_IsKeyDown(INPUT_KEYCODE_Q) - Input_IsKeyDown(INPUT_KEYCODE_E));
            if (Input_IsKeyDown(INPUT_KEYCODE_LEFT_SHIFT))
            {
                y *= 5;
                x *= 5;
                z *= 5;
            }
            Window_GetCamera()->zoom += -z * 0.01f;
            if (Window_GetCamera()->zoom > 1.0f)
            {
                Window_GetCamera()->zoom = 1.0f;
            }
            else if (Window_GetCamera()->zoom < 0.1f)
            {
                Window_GetCamera()->zoom = 0.1f;
            }
            Window_GetCamera()->target.x -= x / Window_GetCamera()->zoom * 2;
            Window_GetCamera()->target.y -= y / Window_GetCamera()->zoom * 2;
        }

        if (debugData.selectedTextureId != NULL)
        {
            debugData.currentObject.textureId = debugData.selectedTextureId->id;
            debugData.currentObject.position  = gridPos;
            Sprite sprite;
            Sprite_Initialize(&sprite);
            sprite.currentTexture = debugData.selectedTextureId;
            sprite.position       = gridAndChunkToWorld(pos);
            sprite.scale          = TEXTURE_SCALE;
            sprite.isVisible      = true;
            Sprite_Draw(&sprite);
        }

        if (Input_IsKeyPressed(INPUT_KEYCODE_X) && ImGui::GetIO().WantCaptureKeyboard == false)
        {
            // Check if object exists at position
            // If so, delete it
            Objects objsAtPos = getObjectsAtPosition(pos);
            for (uint16_t i = 0; i < objsAtPos.count; i++)
            {
                if (objsAtPos.objects[i].layer == debugData.currentObject.layer)
                {
                    // Find chunk
                    Chunk* chunk = NULL;
                    for (uint16_t j = 0; j < gamedata.chunkCount; j++)
                    {
                        if (gamedata.chunks[j].chunkPosition.x == chunkPos.x
                            && gamedata.chunks[j].chunkPosition.y == chunkPos.y)
                        {
                            chunk = &gamedata.chunks[j];
                            break;
                        }
                    }
                    if (chunk != NULL)
                    {
                        // Find object in chunk
                        for (uint16_t j = 0; j < chunk->objectCount; j++)
                        {
                            if (chunk->objects[j].position.x == pos.gridPos.x
                                && chunk->objects[j].position.y == pos.gridPos.y
                                && chunk->objects[j].layer == debugData.currentObject.layer)
                            {
                                // Delete object by shifting array
                                for (uint16_t k = j; k < chunk->objectCount - 1; k++)
                                {
                                    chunk->objects[k] = chunk->objects[k + 1];
                                }
                                chunk->objectCount--;
                                LOG_INF("Deleted object at layer %d", debugData.currentObject.layer);
                                break;
                            }
                        }
                    }
                    return;
                }
            }
        }

        if (Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT) && debugData.selectedTextureId != NULL
            && ImGui::GetIO().WantCaptureMouse == false)
        {
            // Check if object already exists at position
            // If so, replace it
            // Else, add it
            Objects objsAtPos = getObjectsAtPosition(pos);
            for (uint16_t i = 0; i < objsAtPos.count; i++)
            {
                if (objsAtPos.objects[i].layer == debugData.currentObject.layer)
                {
                    objsAtPos.objects[i] = debugData.currentObject;
                    LOG_INF("Replacing object at layer %d", debugData.currentObject.layer);
                    return;
                }
            }
            // Add object to chunk
            LOG_INF("Grid Pos: (%d, %d), ChunkPos: (%d, %d)", gridPos.x, gridPos.y, chunkPos.x, chunkPos.y);
            Chunk* chunk = NULL;
            for (uint16_t i = 0; i < gamedata.chunkCount; i++)
            {
                if (gamedata.chunks[i].chunkPosition.x == chunkPos.x
                    && gamedata.chunks[i].chunkPosition.y == chunkPos.y)
                {
                    chunk = &gamedata.chunks[i];
                    break;
                }
            }

            if (chunk == NULL)
            {
                if (gamedata.chunkCount < (CHUNK_SIZE * CHUNK_SIZE))
                {
                    chunk                = &gamedata.chunks[gamedata.chunkCount++];
                    chunk->chunkPosition = chunkPos;
                    chunk->objectCount   = 0;
                }
                else
                {
                    LOG_ERR("Max chunk count reached!");
                    return;
                }
            }
            if (chunk->objectCount < CHUNK_MAX_OBJECTS)
            {
                chunk->objects[chunk->objectCount++] = debugData.currentObject;
                LOG_INF("Placing object at chunk (%d, %d) pos (%d, %d)", chunkPos.x, chunkPos.y,
                        debugData.currentObject.position.x, debugData.currentObject.position.y);
            }
            else
            {
                LOG_ERR("Max object count reached in chunk!");
            }
        }
    }
}

void DrawObjectsInChunks(Chunk* chunks, uint16_t chunkCount)
{
    for (uint16_t i = 0; i < chunkCount; i++)
    {
        Chunk* chunk = &chunks[i];
        for (uint16_t i = 0; i < chunk->objectCount; i++)
        {
            Object* obj = &chunk->objects[i];
            Sprite  spr;
            Sprite_Initialize(&spr);
            spr.currentTexture = Texture_GetTextureById(obj->textureId);
            if (spr.currentTexture != NULL)
            {
                spr.parent = NULL;
                Position pos;
                pos.gridPos      = obj->position;
                pos.chunkPos     = chunk->chunkPosition;
                Vector2 worldPos = gridAndChunkToWorld(pos);
                spr.position     = worldPos;
                spr.scale        = TEXTURE_SCALE;
                spr.rotation     = 0.0f;
                spr.tint         = WHITE;
                spr.isVisible    = true;
                // LOG_INF("Drawing object at (%f, %f)", spr.position.x, spr.position.y);
                Sprite_Draw(&spr);
            }
        }
    }
}

void MainMode_OnStart()
{
    Texture_LoadTextureSheet("resources/sprites/Anikki_square_8x8.png", 8, 8, 256);

    gamedata.chunkCount = LoadChunksFromFile(gamedata.chunks);
}

void MainMode_OnPause()
{
}

void MainMode_Update()
{
    DrawDebug();
    DrawObjectsInChunks(gamedata.chunks, gamedata.chunkCount);
}

void MainMode_OnStop()
{
    SaveChunksToFile(gamedata.chunks, gamedata.chunkCount);
    Texture_UnloadTextures();
    Audio_UnloadAudios();
}

void MainMode_OnResume()
{
}
