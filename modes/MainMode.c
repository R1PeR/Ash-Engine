#include "MainMode.h"

#include "engine/components/Audio.h"
#include "engine/components/Sprite.h"
#include "engine/components/Texture.h"
#include "engine/io/Input.h"
#include "engine/io/Window.h"
#include "engine/misc/AStar.h"
#include "engine/misc/DeltaTime.h"
#include "engine/misc/Logger.h"
#include "engine/misc/Stopwatch.h"
#include "engine/misc/Utils.h"
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include "utils/Prefabs.h"
#include "utils/Stats.h"
#include "utils/Structs.h"

#include <stdio.h>

Mode mainMode = MODE_FROM_CLASSNAME(MainMode);

GameData  gameData;
DebugData debugData;

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

uint16_t LoadWorldMap(char* worldMap, size_t rows, size_t cols, Chunk* chunks)
{
    uint16_t count  = 0;
    uint16_t lastId = 0;
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            char        tile     = worldMap[i * cols + j];
            Vector3Int  pos      = { (int)j, (int)i, 0 };
            Chunk*      chunk    = NULL;
            Vector3Int8 chunkPos = Utils_GridToChunk(pos, CHUNK_SIZE);
            // Find or create chunk
            for (uint16_t k = 0; k < gameData.chunkCount; k++)
            {
                if (gameData.chunks[k].chunkPosition.x == chunkPos.x
                    && gameData.chunks[k].chunkPosition.y == chunkPos.y)
                {
                    chunk = &gameData.chunks[k];
                    break;
                }
            }
            if (chunk == NULL)
            {
                chunk                = &gameData.chunks[gameData.chunkCount++];
                chunk->chunkPosition = chunkPos;
                chunk->objectCount   = 0;
                count++;
            }
            // Create object based on tile type
            Object obj;
            switch (tile)
            {
                case '0':
                    obj                                  = emptyTilePrefab;
                    obj.position                         = pos;
                    obj.id                               = lastId++;
                    chunk->objects[chunk->objectCount++] = obj;
                    break;
                case '1':
                    obj                                  = wallTilePrefab;
                    obj.position                         = pos;
                    obj.id                               = lastId++;
                    chunk->objects[chunk->objectCount++] = obj;
                    break;
                case 'p':
                    obj                                  = playerPrefab;
                    obj.position                         = pos;
                    obj.id                               = lastId++;
                    chunk->objects[chunk->objectCount++] = obj;
                    break;
                case 'r':
                    obj                                  = enemyRatPrefab;
                    obj.position                         = pos;
                    obj.id                               = lastId++;
                    obj.entity.entityOriginalPosition    = { pos.x, pos.y };
                    chunk->objects[chunk->objectCount++] = obj;
                    break;
                default:
                    break;
            }
        }
    }
    return count;
}
Objects GetObjectsAtPosition(Vector3Int pos)
{
    Vector3Int8 chunkPos;
    chunkPos = Utils_GridToChunk(pos, CHUNK_SIZE);

    Objects objs;
    objs.count = 0;
    for (uint16_t i = 0; i < gameData.chunkCount; i++)
    {
        Chunk* chunk = &gameData.chunks[i];
        if (chunk->chunkPosition.x == chunkPos.x && chunk->chunkPosition.y == chunkPos.y)
        {
            for (uint16_t j = 0; j < chunk->objectCount; j++)
            {
                Object* obj = &chunk->objects[j];
                if (obj->position.x == pos.x && obj->position.y == pos.y)
                {
                    if (objs.count < MAX_LAYERS)
                    {
                        objs.objects[objs.count++] = *obj;
                    }
                    else
                    {
                        LOG_ERR("Max layers reached at position (%d, %d) in chunk (%d, %d)", pos.x, pos.y, chunkPos.x,
                                chunkPos.y);
                    }
                }
            }
        }
    }
    return objs;
}

bool GetClosestEntityInRange(Object* sourceObj, uint8_t range, Object** outObj)
{
    Vector2 sourceWorldPos = Utils_GridCenterToWorld(sourceObj->position, TEXTURE_SIZE * TEXTURE_SCALE);
    float   closestDist    = float(range * TEXTURE_SIZE * TEXTURE_SCALE);
    Object* closestObj     = NULL;
    for (uint16_t i = 0; i < gameData.chunkCount; i++)
    {
        Chunk* chunk = &gameData.chunks[i];
        for (uint16_t j = 0; j < chunk->objectCount; j++)
        {
            Object* obj = &chunk->objects[j];
            if (obj->type == Type::ENTITY && obj->id != sourceObj->id)
            {
                Vector2 targetWorldPos = Utils_GridCenterToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
                float   dist           = Utils_Vector2Distance(sourceWorldPos, targetWorldPos);
                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestObj  = obj;
                }
            }
        }
    }
    if (closestObj != NULL)
    {
        *outObj = closestObj;
        return true;
    }
    return false;
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
        Vector2    mousePos  = { (float)(Input_GetMouseX()), (float)(Input_GetMouseY()) };
        Vector2    worldPos  = GetScreenToWorld2D(mousePos, *Window_GetCamera());
        Vector3Int gridPos   = Utils_WorldToGrid(worldPos, TEXTURE_SIZE * TEXTURE_SCALE);
        gridPos.z            = gameData.currentZPos;
        Vector3Int8 chunkPos = Utils_WorldToChunk(worldPos, TEXTURE_SIZE * TEXTURE_SCALE, CHUNK_SIZE);
        chunkPos.z           = gameData.currentZPos;

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
            Sprite* sprite                    = &gameData.sprites[gameData.spriteCount++];
            Sprite_Initialize(sprite);
            sprite->currentTexture = debugData.selectedTextureId;
            sprite->position       = Utils_GridToWorld(gridPos, TEXTURE_SIZE * TEXTURE_SCALE);
            sprite->scale          = TEXTURE_SCALE;
            sprite->isVisible      = true;
            sprite->tint           = LIGHTGRAY;
            Sprite_Add(sprite);
        }

        if (Input_IsKeyPressed(INPUT_KEYCODE_X) && ImGui::GetIO().WantCaptureKeyboard == false)
        {
            // Check if object exists at position
            // If so, delete it
            Objects objsAtPos = GetObjectsAtPosition(gridPos);
            int     topLayer  = 0;
            for (uint16_t i = 0; i < objsAtPos.count; i++)
            {
                if (objsAtPos.objects[i].layer > topLayer)
                {
                    topLayer = objsAtPos.objects[i].layer;
                }
            }
            // Find chunk
            Chunk* chunk = NULL;
            for (uint16_t j = 0; j < gameData.chunkCount; j++)
            {
                if (gameData.chunks[j].chunkPosition.x == chunkPos.x
                    && gameData.chunks[j].chunkPosition.y == chunkPos.y)
                {
                    chunk = &gameData.chunks[j];
                    break;
                }
            }
            if (chunk != NULL)
            {
                // Find object in chunk
                for (uint16_t j = 0; j < chunk->objectCount; j++)
                {
                    if (chunk->objects[j].position.x == gridPos.x && chunk->objects[j].position.y == gridPos.y
                        && chunk->objects[j].layer == topLayer)
                    {
                        // Delete object by shifting array
                        for (uint16_t k = j; k < chunk->objectCount - 1; k++)
                        {
                            chunk->objects[k] = chunk->objects[k + 1];
                        }
                        chunk->objectCount--;
                        LOG_INF("Deleted object at layer %d", topLayer);
                        break;
                    }
                }
            }
        }

        if (Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_RIGHT) && ImGui::GetIO().WantCaptureMouse == false)
        {
            LOG_INF("Selecting object at position");
            // Select top object at position
            Objects objsAtPos = GetObjectsAtPosition(gridPos);
            int     topLayer  = -1;
            if (objsAtPos.count != 0)
            {
                LOG_INF("No objects at position");
                for (uint16_t i = 0; i < objsAtPos.count; i++)
                {
                    if (objsAtPos.objects[i].layer > topLayer)
                    {
                        topLayer                    = objsAtPos.objects[i].layer;
                        debugData.currentObject     = objsAtPos.objects[i];
                        debugData.selectedTextureId = Texture_GetTextureById(debugData.currentObject.textureId);
                    }
                }
            }
        }

        if (Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT) && debugData.selectedTextureId != NULL
            && ImGui::GetIO().WantCaptureMouse == false)
        {
            // if (debugData.currentObject.id == -1)
            // {
            // auto assign id
            int maxId = 0;
            for (uint16_t i = 0; i < gameData.chunkCount; i++)
            {
                Chunk* chunk = &gameData.chunks[i];
                for (uint16_t j = 0; j < chunk->objectCount; j++)
                {
                    if (chunk->objects[j].id > maxId)
                    {
                        maxId = chunk->objects[j].id;
                    }
                }
            }
            debugData.currentObject.id = maxId + 1;
            // }
            // Check if object already exists at position
            // If so, replace it
            // Else, add it
            Objects objsAtPos = GetObjectsAtPosition(gridPos);
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
            for (uint16_t i = 0; i < gameData.chunkCount; i++)
            {
                if (gameData.chunks[i].chunkPosition.x == chunkPos.x
                    && gameData.chunks[i].chunkPosition.y == chunkPos.y)
                {
                    chunk = &gameData.chunks[i];
                    break;
                }
            }

            if (chunk == NULL)
            {
                if (gameData.chunkCount < (CHUNK_SIZE * CHUNK_SIZE))
                {
                    chunk                = &gameData.chunks[gameData.chunkCount++];
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

        if (Input_IsKeyPressed(INPUT_KEYCODE_UP) && ImGui::GetIO().WantCaptureKeyboard == false)
        {
            gameData.currentZPos++;
            LOG_INF("Current Z Pos: %d", gameData.currentZPos);
        }

        if (Input_IsKeyPressed(INPUT_KEYCODE_DOWN) && ImGui::GetIO().WantCaptureKeyboard == false)
        {
            gameData.currentZPos--;
            LOG_INF("Current Z Pos: %d", gameData.currentZPos);
        }

        ImGui::SetNextWindowPos(ImVec2(50, 20), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(500, 680), ImGuiCond_Always);

        ImGuiWindowFlags window_flags = 0;

        if (!ImGui::Begin("Select View", nullptr, window_flags))
        {
            ImGui::End();
            return;
        }

        const char* prefabs[] = { "EMPTY_TILE", "WALL_TILE", "PLAYER", "ENEMY_RAT" };
        ImGui::ListBox("Prefab", &debugData.currentPrefab, prefabs, IM_ARRAYSIZE(prefabs), 5);
        switch (debugData.currentPrefab)
        {
            case 0:
                debugData.currentObject = emptyTilePrefab;
                break;
            case 1:
                debugData.currentObject = wallTilePrefab;
                break;
            case 2:
                debugData.currentObject = playerPrefab;
                break;
            case 3:
                debugData.currentObject = enemyRatPrefab;
                break;
            default:
                break;
        }
        // ImGui::InputInt("Id", (int*)&debugData.currentObject.id);
        ImGui::InputInt("Layer", (int*)&debugData.currentObject.layer);

        ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
        ImGui::Text("World Position: (%.1f, %.1f)", worldPos.x, worldPos.y);
        ImGui::Text("Grid Position: (%d, %d)", gridPos.x, gridPos.y);
        ImGui::Text("Chunk Position: (%d, %d, %d)", chunkPos.x, chunkPos.y, chunkPos.z);

        // display all objects in memory in a list, only display entities
        ImGui::Separator();


        ImGui::Text("Objects in Memory:");
        for (uint16_t i = 0; i < gameData.chunkCount; i++)
        {
            Chunk* chunk = &gameData.chunks[i];
            ImGui::Text("Chunk (%d, %d):", chunk->chunkPosition.x, chunk->chunkPosition.y);
            for (uint16_t j = 0; j < chunk->objectCount; j++)
            {
                if (chunk->objects[j].type != Type::ENTITY)
                {
                    continue;
                }
                ImGui::PushID(j);
                Object* obj = &chunk->objects[j];
                if (ImGui::TreeNode("Entity"))
                {
                    ImGui::Text("Object ID: %d", obj->id);
                    ImGui::Text("Type: %d", (int)obj->type);
                    ImGui::Text("Layer: %d", obj->layer);
                    ImGui::Text("Texture ID: %d", obj->textureId);
                    ImGui::Text("Is Collidable: %d", obj->isCollidable);
                    ImGui::Text("Position: (%d, %d, %d)", obj->position.x, obj->position.y, obj->position.z);
                    if (obj->type == Type::ENTITY)
                    {
                        ImGui::Text("Entity Type: %d", (int)obj->entity.entityType);
                        ImGui::Text("Health: %d", obj->entity.entityHealth);
                        ImGui::Text("Experience: %d", obj->entity.entityExperience);
                        ImGui::Text("Level: %d", obj->entity.entityLevel);
                        ImGui::Text("Speed: %d", obj->entity.entitySpeed);
                        ImGui::Text("Damage: %d", obj->entity.entityDamage);
                        if (obj->entity.entityType == EntityType::ENEMY)
                        {
                            // ImGui::Text("Entity State: %d", (int)obj->entity.entityState);
                            switch (obj->entity.entityState)
                            {
                                case EntityState::PATROLLING:
                                    ImGui::Text("Entity State: PATROLLING");
                                    break;
                                case EntityState::CHASING:
                                    ImGui::Text("Entity State: CHASING");
                                    break;
                                case EntityState::GOING_BACK:
                                    ImGui::Text("Entity State: GOING_BACK");
                                    break;
                                default:
                                    ImGui::Text("Entity State: UNKNOWN");
                                    break;
                            }
                            if (obj->entity.entityState == EntityState::CHASING && obj->entity.entityTarget != NULL)
                            {
                                ImGui::Text("Chasing Target ID: %d", obj->entity.entityTarget->id);
                            }
                            ImGui::Text("Original Position: (%d, %d)", obj->entity.entityOriginalPosition.x,
                                        obj->entity.entityOriginalPosition.y);
                            ImGui::Text("Patrol Radius: %d", obj->entity.entityPatrolRadius);
                        }
                        if (obj->entity.entityType == EntityType::PLAYER)
                        {
                            ImGui::Text("Items:");
                            for (uint8_t k = 0; k < 8; k++)
                            {
                                ImGui::Text("Item %d: %d", k, obj->entity.entityItems[k]);
                            }
                        }
                    }
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
        }

        ImGui::End();
    }
}

bool CheckCollision(Vector3Int pos)
{
    Objects objsAtPos = GetObjectsAtPosition(pos);
    for (uint16_t i = 0; i < objsAtPos.count; i++)
    {
        if (objsAtPos.objects[i].isCollidable)
        {
            // Collision detected
            // LOG_INF("Collision detected at (%d, %d, %d) with object id %d", pos.x, pos.y, pos.z,
            //         objsAtPos.objects[i].id);
            return true;
        }
    }
    return false;
    // No collision
}

bool CheckCollision(Vector3Int pos, Object* outObj)
{
    Objects objsAtPos = GetObjectsAtPosition(pos);
    for (uint16_t i = 0; i < objsAtPos.count; i++)
    {
        if (objsAtPos.objects[i].isCollidable)
        {
            // Collision detected
            // LOG_INF("Collision detected at (%d, %d, %d) with object id %d", pos.x, pos.y, pos.z,
            //         objsAtPos.objects[i].id);
            *outObj = objsAtPos.objects[i];
            return true;
        }
    }
    return false;
    // No collision
}

bool MoveCost(Vector2Int startPos, Vector2Int targetPos, uint16_t& outCost)
{
    outCost = Utils_ManhattanDistance(startPos, targetPos);
    if (!CheckCollision({ startPos.x, startPos.y, 0 }))
    {
        return true;
    }
    return false;
}

Vector2Int8 GetMoveTowardsPosition(Vector2Int source, Vector2Int target)
{
    Vector2Int8 direction = AStar_GetMoveDirection(source, target, 256, MoveCost);
    return direction;
}

void UpdatePlayer(Object* obj)
{
    Camera2D* camera   = Window_GetCamera();
    Vector2   worldPos = Utils_GridCenterToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
    camera->target.x += (worldPos.x - camera->target.x) * DeltaTime_GetDeltaTime() * 2.0f;
    camera->target.y += (worldPos.y - camera->target.y) * DeltaTime_GetDeltaTime() * 2.0f;
    // Example: simple player movement logic
    int8_t y = (Input_IsKeyDown(INPUT_KEYCODE_S) - Input_IsKeyDown(INPUT_KEYCODE_W));
    int8_t x = (Input_IsKeyDown(INPUT_KEYCODE_D) - Input_IsKeyDown(INPUT_KEYCODE_A));
    if (x * x + y * y <= 1 && (x != 0 || y != 0))
    {
        if (Stopwatch_IsZero(&obj->entity.entityMovementTimer))
        {
            if (!CheckCollision({ obj->position.x + x, obj->position.y + y, obj->position.z }))
            {
                obj->position.x += x;
                obj->position.y += y;
                obj->entity.entityMovementDirection = { x, y };
                Stopwatch_Start(&obj->entity.entityMovementTimer, Stats_MovementDelay(obj->entity.entitySpeed));
            }
        }
    }

    Sprite* sprite = &gameData.sprites[gameData.spriteCount++];
    Sprite_Initialize(sprite);
    sprite->currentTexture = Texture_GetTextureById(obj->textureId);
    sprite->scale          = TEXTURE_SCALE;
    sprite->isVisible      = true;
    sprite->tint           = WHITE;
    sprite->position       = Utils_GridToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
    if (!Stopwatch_IsElapsed(&obj->entity.entityMovementTimer))
    {
        sprite->position.x +=
            float(-obj->entity.entityMovementDirection.x
                  * Stopwatch_GetPercentRemainingTime(&obj->entity.entityMovementTimer) * TEXTURE_SIZE * TEXTURE_SCALE);
        sprite->position.y +=
            float(-obj->entity.entityMovementDirection.y
                  * Stopwatch_GetPercentRemainingTime(&obj->entity.entityMovementTimer) * TEXTURE_SIZE * TEXTURE_SCALE);
    }
    Sprite_Add(sprite);
}

void UpdateEnemy(Object* obj)
{
    switch (obj->entity.entityState)
    {
        case EntityState::PATROLLING:
        {
            if (GetClosestEntityInRange(obj, obj->entity.entityChaseRadius, &obj->entity.entityTarget))
            {
                obj->entity.entityState = EntityState::CHASING;
                break;
            }
            if (!Stopwatch_IsZero(&obj->entity.entityMovementTimer))
            {
                break;
            }
            int16_t     x    = Utils_GerRandomInRange(-(uint16_t)obj->entity.entityPatrolRadius,
                                                      (uint16_t)obj->entity.entityPatrolRadius);
            int16_t     y    = Utils_GerRandomInRange(-(uint16_t)obj->entity.entityPatrolRadius,
                                                      (uint16_t)obj->entity.entityPatrolRadius);
            Vector2Int8 move = GetMoveTowardsPosition({ obj->position.x, obj->position.y },
                                                      { obj->position.x + x, obj->position.y + y });
            if (move.x == 0 && move.y == 0)
            {
                break;
            }
            if (CheckCollision({ obj->position.x + move.x, obj->position.y + move.y, obj->position.z }))
            {
                break;
            }
            Vector2Int newPos = { obj->position.x + x, obj->position.y + y };
            if (!Utils_IsInGridRadius(obj->entity.entityOriginalPosition, newPos, obj->entity.entityPatrolRadius))
            {
                obj->entity.entityState = EntityState::GOING_BACK;
                break;
            }
            obj->position.x += move.x;
            obj->position.y += move.y;
            obj->entity.entityMovementDirection = { move.x, move.y };
            Stopwatch_Start(&obj->entity.entityMovementTimer, Stats_MovementDelay(obj->entity.entitySpeed));
        }
        break;
        case EntityState::CHASING:
        {
            if (obj->entity.entityTarget == NULL)
            {
                obj->entity.entityState = EntityState::PATROLLING;
                break;
            }
            if (Utils_Vector2DistanceInt({ obj->position.x, obj->position.y },
                                         { obj->entity.entityTarget->position.x, obj->entity.entityTarget->position.y })
                <= obj->entity.entityRange)
            {
                if (Stopwatch_IsZero(&obj->entity.entityAttackTimer))
                {
                    LOG_INF("Enemy %d attacking target %d", obj->id, obj->entity.entityTarget->id);
                    obj->entity.entityTarget->entity.entityHealth -= obj->entity.entityDamage;
                    Stopwatch_Start(&obj->entity.entityAttackTimer,
                                    Stats_AttackDelay(obj->entity.entityAttackSpeed, obj->entity.entityDexterity));
                }
                break;
            }
            Vector2 sourceWorldPos = Utils_GridCenterToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
            Vector2 targetWorldPos =
                Utils_GridCenterToWorld(obj->entity.entityTarget->position, TEXTURE_SIZE * TEXTURE_SCALE);
            float dist = Utils_Vector2Distance(sourceWorldPos, targetWorldPos);
            if (dist > float(5 * TEXTURE_SIZE * TEXTURE_SCALE))
            {
                obj->entity.entityTarget = NULL;
                obj->entity.entityState  = EntityState::PATROLLING;
                break;
            }
            if (!Utils_IsInGridRadius(obj->entity.entityOriginalPosition, { obj->position.x, obj->position.y },
                                      obj->entity.entityPatrolRadius + 2))
            {
                obj->entity.entityTarget = NULL;
                obj->entity.entityState  = EntityState::GOING_BACK;
                break;
            }
            if (!Stopwatch_IsZero(&obj->entity.entityMovementTimer))
            {
                break;
            }
            Vector2Int8 dir =
                GetMoveTowardsPosition({ obj->position.x, obj->position.y },
                        { obj->entity.entityTarget->position.x, obj->entity.entityTarget->position.y });
            if (!CheckCollision({ obj->position.x + dir.x, obj->position.y + dir.y, obj->position.z }))
            {
                obj->position.x += dir.x;
                obj->position.y += dir.y;
                obj->entity.entityMovementDirection = { dir.x, dir.y };
                Stopwatch_Start(&obj->entity.entityMovementTimer, Stats_MovementDelay(obj->entity.entitySpeed));
            }
        }
        break;
        case EntityState::GOING_BACK:
        {
            if (obj->position.x == obj->entity.entityOriginalPosition.x
                && obj->position.y == obj->entity.entityOriginalPosition.y)
            {
                obj->entity.entityState = EntityState::PATROLLING;
                break;
            }
            if (!Stopwatch_IsZero(&obj->entity.entityMovementTimer))
            {
                break;
            }
            Vector2Int8 dir =
                GetMoveTowardsPosition({ obj->position.x, obj->position.y }, obj->entity.entityOriginalPosition);
            LOG_INF("Going back dir: (%d, %d)", dir.x, dir.y);
            if (!CheckCollision({ obj->position.x + dir.x, obj->position.y + dir.y, obj->position.z }))
            {
                obj->position.x += dir.x;
                obj->position.y += dir.y;
                obj->entity.entityMovementDirection = { dir.x, dir.y };
                Stopwatch_Start(&obj->entity.entityMovementTimer, Stats_MovementDelay(obj->entity.entitySpeed));
            }
        }
        break;
        default:
            break;
    }
    // Example: simple enemy AI logic
    Sprite* sprite = &gameData.sprites[gameData.spriteCount++];
    Sprite_Initialize(sprite);
    sprite->currentTexture = Texture_GetTextureById(obj->textureId);
    sprite->scale          = TEXTURE_SCALE;
    sprite->isVisible      = true;
    sprite->tint           = WHITE;
    sprite->position       = Utils_GridToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
    if (!Stopwatch_IsElapsed(&obj->entity.entityMovementTimer))
    {
        sprite->position.x +=
            float(-obj->entity.entityMovementDirection.x
                  * Stopwatch_GetPercentRemainingTime(&obj->entity.entityMovementTimer) * TEXTURE_SIZE * TEXTURE_SCALE);
        sprite->position.y +=
            float(-obj->entity.entityMovementDirection.y
                  * Stopwatch_GetPercentRemainingTime(&obj->entity.entityMovementTimer) * TEXTURE_SIZE * TEXTURE_SCALE);
    }
    Sprite_Add(sprite);
}

void UpdateProjectile(Object* obj)
{
    // // Example: simple movement logic
    // obj->position.x += (int8_t)(obj->projectile.projectileDirection.x * obj->projectile.projectileSpeed);
    // obj->position.y += (int8_t)(obj->projectile.projectileDirection.y * obj->projectile.projectileSpeed);
    // obj->projectile.projectileDistanceTraveled += (int)(obj->projectile.projectileSpeed);
    // if (obj->projectile.projectileDistanceTraveled >= obj->projectile.projectileRange)
    // {
    //     // Mark for deletion or reset
    //     obj->position.x = -1;  // Example of marking for deletion
    // }
}

void UpdateEffect(Object* obj)
{
    // obj->effect.effectTimeElapsed += DeltaTime_Get();
    // if (obj->effect.effectTimeElapsed >= obj->effect.effectDuration)
    // {
    //     // Mark for deletion or reset
    //     obj->position.x = -1;  // Example of marking for deletion
    // }
}

void UpdateInteractive(Object* obj)
{
    // // Example: toggle open state
    // if (obj->interactive.interactiveType == Object::InteractiveType::CHEST)
    // {
    //     obj->interactive.isOpen = !obj->interactive.isOpen;
    // }
}

void UpdateItem(Object* obj)
{
    // Example: item logic
    // Items might not have update logic
}

void MainMode_OnStart()
{
    Texture_LoadTextureSheet("resources/sprites/Anikki_square_8x8.png", 8, 8, 256);

    Window_GetCamera()->target = (Vector2){ 0.0f, 0.0f };
    gameData.chunkCount        = LoadWorldMap((char*)worldMap, WORLD_MAP_SIZE, WORLD_MAP_SIZE, gameData.chunks);
    for (uint16_t i = 0; i < gameData.chunkCount; i++)
    {
        Chunk* chunk = &gameData.chunks[i];
        for (uint16_t j = 0; j < chunk->objectCount; j++)
        {
            Object* obj = &chunk->objects[j];
            if (obj->type == Type::ENTITY)
            {
                Stopwatch_Stop(&obj->entity.entityMovementTimer);
                Stopwatch_Stop(&obj->entity.entityAttackTimer);
            }
        }
    }
}

void MainMode_OnPause()
{
}

void MainMode_Update()
{
    Sprite_Clear();
    gameData.spriteCount = 0;

    DrawDebug();
    // check what objects are in view of the camera and draw them
    Vector3Int8 camPosChunk = Utils_WorldToChunk(Window_GetCamera()->target, TEXTURE_SIZE * TEXTURE_SCALE, CHUNK_SIZE);
    camPosChunk.z           = gameData.currentZPos;
    Chunk*   visibleChunks[15];
    uint16_t visibleChunkCount = 0;
    for (uint16_t i = 0; i < gameData.chunkCount; i++)
    {
        Chunk* chunk = &gameData.chunks[i];
        if (chunk->chunkPosition.x >= camPosChunk.x - 2 && chunk->chunkPosition.x <= camPosChunk.x + 2
            && chunk->chunkPosition.y >= camPosChunk.y - 1 && chunk->chunkPosition.y <= camPosChunk.y + 1
            && chunk->chunkPosition.z == camPosChunk.z)
        {
            visibleChunks[visibleChunkCount++] = chunk;
        }
    }
    for (uint32_t i = 0; i < visibleChunkCount; i++)
    {
        for (uint16_t j = 0; j < visibleChunks[i]->objectCount; j++)
        {
            switch (visibleChunks[i]->objects[j].type)
            {
                case Type::TILE:
                {
                    Sprite* sprite = &gameData.sprites[gameData.spriteCount++];
                    Sprite_Initialize(sprite);
                    sprite->currentTexture = Texture_GetTextureById(visibleChunks[i]->objects[j].textureId);
                    sprite->position =
                        Utils_GridToWorld(visibleChunks[i]->objects[j].position, TEXTURE_SIZE * TEXTURE_SCALE);
                    sprite->scale     = TEXTURE_SCALE;
                    sprite->isVisible = true;
                    sprite->zOrder    = visibleChunks[i]->objects[j].layer;
                    Sprite_Add(sprite);
                }
                break;
                case Type::ENTITY:
                {
                    Object* obj = &visibleChunks[i]->objects[j];
                    if (obj->entity.entityType == EntityType::PLAYER)
                    {
                        UpdatePlayer(obj);
                    }
                    else if (obj->entity.entityType == EntityType::ENEMY)
                    {
                        UpdateEnemy(obj);
                    }
                }
                break;
                case Type::PROJECTILE:
                    UpdateProjectile(&visibleChunks[i]->objects[j]);
                    // Update projectile logic here
                    break;
                case Type::EFFECT:
                    UpdateEffect(&visibleChunks[i]->objects[j]);
                    // Update effect logic here
                    break;
                case Type::INTERACTIVE:
                    UpdateInteractive(&visibleChunks[i]->objects[j]);
                    // Update interactive logic here
                    break;
                case Type::ITEM:
                    UpdateItem(&visibleChunks[i]->objects[j]);
                    // Update item logic here
                    break;
                default:
                    break;
            }
        }
    }
}

void MainMode_OnStop()
{
    // SaveChunksToFile(gameData.chunks, gameData.chunkCount);
    Texture_UnloadTextures();
    Audio_UnloadAudios();
}

void MainMode_OnResume()
{
}
