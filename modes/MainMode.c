#include "MainMode.h"

#include "EndGameMode.h"
#include "engine/components/Audio.h"
#include "engine/components/Sprite.h"
#include "engine/components/Texture.h"
#include "engine/context/Context.h"
#include "engine/io/Input.h"
#include "engine/io/Window.h"
#include "engine/misc/AStar.h"
#include "engine/misc/DeltaTime.h"
#include "engine/misc/Logger.h"
#include "engine/misc/Stopwatch.h"
#include "engine/misc/Utils.h"
#include "imgui.h"
#include "raylib.h"
#include "utils/Prefabs.h"
#include "utils/Stats.h"
#include "utils/Structs.h"
#include "utils/UI.h"

#include <stdio.h>

Mode mainMode = MODE_FROM_CLASSNAME(MainMode);

GameData  gameData;
DebugData debugData;

uint8_t GetObjectsAtPosition(Vector3Int pos, Object** outObjs)
{
    Vector3Int8 chunkPos;
    chunkPos = Utils_GridToChunk(pos, CHUNK_SIZE);

    uint8_t count = 0;
    for (uint16_t i = 0; i < gameData.chunkCount; i++)
    {
        Chunk* chunk = &gameData.chunks[i];
        if (chunk->chunkPosition.x == chunkPos.x && chunk->chunkPosition.y == chunkPos.y)
        {
            for (uint16_t j = 0; j < chunk->objectCount; j++)
            {
                Object* obj = chunk->objects[j];
                if (obj->position.x == pos.x && obj->position.y == pos.y)
                {
                    if (count < MAX_LAYERS)
                    {
                        outObjs[count++] = obj;
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
    return count;
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
            Object* obj = chunk->objects[j];
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


bool CheckCollision(Vector3Int pos)
{
    // Objects objsAtPos = GetObjectsAtPosition(pos);
    Object* objects[MAX_LAYERS];
    uint8_t objCount = GetObjectsAtPosition(pos, objects);
    for (uint16_t i = 0; i < objCount; i++)
    {
        if (objects[i]->isCollidable)
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
    // Objects objsAtPos = GetObjectsAtPosition(pos);
    Object* objects[MAX_LAYERS];
    uint8_t objCount = GetObjectsAtPosition(pos, objects);
    for (uint16_t i = 0; i < objCount; i++)
    {
        if (objects[i]->isCollidable)
        {
            // Collision detected
            // LOG_INF("Collision detected at (%d, %d, %d) with object id %d", pos.x, pos.y, pos.z,
            //         objsAtPos.objects[i].id);
            *outObj = *objects[i];
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

void RemoveFromChunk(Object* obj)
{
    if (obj->parentChunk == NULL)
    {
        LOG_WRN("Object id %d has no parent chunk!", obj->id);
        return;
    }
    for (uint16_t j = 0; j < obj->parentChunk->objectCount; j++)
    {
        if (obj->parentChunk->objects[j] == obj)
        {
            // Remove from old chunk
            obj->parentChunk->objects[j] = obj->parentChunk->objects[obj->parentChunk->objectCount - 1];
            obj->parentChunk->objectCount--;
            LOG_INF("Removed object id %d from chunk (%d, %d)", obj->id, obj->parentChunk->chunkPosition.x,
                    obj->parentChunk->chunkPosition.y);
            obj->parentChunk = NULL;
            return;
        }
    }
}

void AddToChunk(Object* obj)
{
    Vector3Int8 toChunkPos = Utils_GridToChunk(obj->position, CHUNK_SIZE);
    for (uint16_t i = 0; i < gameData.chunkCount; i++)
    {
        if (gameData.chunks[i].chunkPosition.x == toChunkPos.x && gameData.chunks[i].chunkPosition.y == toChunkPos.y)
        {
            // Add to new chunk
            if (gameData.chunks[i].objectCount < CHUNK_MAX_OBJECTS)
            {
                obj->parentChunk                                             = &gameData.chunks[i];
                gameData.chunks[i].objects[gameData.chunks[i].objectCount++] = obj;
                LOG_INF("Added object id %d to chunk (%d, %d)", obj->id, toChunkPos.x, toChunkPos.y);
            }
            else
            {
                LOG_ERR("Max object count reached in chunk (%d, %d)!", toChunkPos.x, toChunkPos.y);
            }
            return;
        }
    }
    // Chunk not found, create new chunk
    if (gameData.chunkCount < (CHUNK_SIZE * CHUNK_SIZE))
    {
        Chunk* chunk                         = &gameData.chunks[gameData.chunkCount++];
        chunk->chunkPosition                 = toChunkPos;
        chunk->objectCount                   = 0;
        chunk->objects[chunk->objectCount++] = obj;
        obj->parentChunk                     = chunk;
        LOG_INF("Created new chunk (%d, %d) and added object id %d", toChunkPos.x, toChunkPos.y, obj->id);
    }
    else
    {
        LOG_ERR("Max chunk count reached!");
    }
}

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

void LoadWorldMap(char* worldMap, size_t rows, size_t cols, Chunk* chunks)
{
    uint16_t lastId = 0;
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            char       tile = worldMap[i * cols + j];
            Vector3Int pos  = { (int)j, (int)i, 0 };
            // Create object based on tile type
            Object* obj = &gameData.objects[gameData.objectCount++];
            switch (tile)
            {
                case '0':
                    *obj = emptyTilePrefab;
                    break;
                case '1':
                    *obj = wallTilePrefab;
                    break;
                case 'p':
                    *obj = playerPrefab;
                    for (size_t k = 0; k < ENTITY_MAX_ITEMS; k++)
                    {
                        obj->entity.entityItems[k] = NULL;
                    }
                    gameData.playerObject = obj;
                    break;
                case 'r':
                    *obj = enemyRatPrefab;
                    break;
                case 'i':
                    *obj = itemSwordPrefab;
                    break;
                default:
                    break;
            }
            obj->position                      = pos;
            obj->id                            = lastId++;
            obj->entity.entityOriginalPosition = { pos.x, pos.y };
            AddToChunk(obj);
        }
    }
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
        Vector2 mousePos     = { (float)(Input_GetMouseX()), (float)(Input_GetMouseY()) };
        Vector2 worldPos     = GetScreenToWorld2D(mousePos, *Window_GetCamera());
        Vector2 worldPosCam  = { worldPos.x - Window_GetCamera()->target.x, worldPos.y - Window_GetCamera()->target.y };
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
            Sprite sprite;
            Sprite_Initialize(&sprite);
            sprite.currentTexture = debugData.selectedTextureId;
            sprite.position       = Utils_GridToWorld(gridPos, TEXTURE_SIZE * TEXTURE_SCALE);
            sprite.scale          = TEXTURE_SCALE;
            sprite.isVisible      = true;
            sprite.tint           = LIGHTGRAY;
            Sprite_Add(&sprite);
        }

        if (Input_IsKeyPressed(INPUT_KEYCODE_X) && ImGui::GetIO().WantCaptureKeyboard == false)
        {
            // Check if object exists at position
            // If so, delete it
            Object* objects[MAX_LAYERS];
            uint8_t objCount = GetObjectsAtPosition(gridPos, objects);
            // Objects objsAtPos = GetObjectsAtPosition(gridPos);
            int topLayer = 0;
            for (uint16_t i = 0; i < objCount; i++)
            {
                if (objects[i]->layer > topLayer)
                {
                    topLayer = objects[i]->layer;
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
                    if (chunk->objects[j]->position.x == gridPos.x && chunk->objects[j]->position.y == gridPos.y
                        && chunk->objects[j]->layer == topLayer)
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
            // Objects objsAtPos = GetObjectsAtPosition(gridPos);
            Object* objects[MAX_LAYERS];
            uint8_t objCount = GetObjectsAtPosition(gridPos, objects);
            int     topLayer = -1;
            if (objCount != 0)
            {
                LOG_INF("No objects at position");
                for (uint16_t i = 0; i < objCount; i++)
                {
                    if (objects[i]->layer > topLayer)
                    {
                        topLayer                    = objects[i]->layer;
                        debugData.currentObject     = *objects[i];
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
                    if (chunk->objects[j]->id > maxId)
                    {
                        maxId = chunk->objects[j]->id;
                    }
                }
            }
            debugData.currentObject.id = maxId + 1;
            // }
            // Check if object already exists at position
            // If so, replace it
            // Else, add it
            // Objects objsAtPos = GetObjectsAtPosition(gridPos);
            Object* objects[MAX_LAYERS];
            uint8_t objCount = GetObjectsAtPosition(gridPos, objects);
            for (uint16_t i = 0; i < objCount; i++)
            {
                if (objects[i]->layer == debugData.currentObject.layer)
                {
                    objects[i] = &debugData.currentObject;
                    LOG_INF("Replacing object at layer %d", debugData.currentObject.layer);
                    return;
                }
            }
            gameData.objects[gameData.objectCount++] = debugData.currentObject;
            AddToChunk(&gameData.objects[gameData.objectCount - 1]);
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

        const char* prefabs[] = { "EMPTY_TILE", "WALL_TILE", "PLAYER", "ENEMY_RAT", "SWORD_ITEM" };
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
            case 4:
                debugData.currentObject = itemSwordPrefab;
                break;
            default:
                break;
        }
        // ImGui::InputInt("Id", (int*)&debugData.currentObject.id);
        ImGui::InputInt("Layer", (int*)&debugData.currentObject.layer);

        ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
        ImGui::Text("World Position: (%.1f, %.1f)", worldPos.x, worldPos.y);
        ImGui::Text("World Position relative to Camera: (%.1f, %.1f)", worldPosCam.x, worldPosCam.y);
        ImGui::Text("Grid Position: (%d, %d)", gridPos.x, gridPos.y);
        ImGui::Text("Chunk Position: (%d, %d, %d)", chunkPos.x, chunkPos.y, chunkPos.z);

        // display all objects in memory in a list, only display entities
        ImGui::Separator();


        ImGui::Text("Objects in Memory:");
        for (uint16_t i = 0; i < gameData.objectCount; i++)
        {
            ImGui::PushID(i);
            Object* obj = &gameData.objects[i];
            if (obj->type != Type::ENTITY)
            {
                ImGui::PopID();
                continue;
            }
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
                        for (uint8_t k = 0; k < ENTITY_MAX_ITEMS; k++)
                        {
                            if (obj->entity.entityItems[k] != NULL)
                            {
                                ImGui::Text("Item %d: %d", k, obj->entity.entityItems[k]->id);
                            }
                        }
                    }
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }

        ImGui::End();
    }
}
void UpdateObjectChunk(Object* obj)
{
    if (obj == NULL)
    {
        LOG_ERR("UpdateObjectChunk called with NULL object!");
        return;
    }
    Vector3Int8 currentChunkPos = Utils_GridToChunk(obj->position, CHUNK_SIZE);
    if (obj->parentChunk == NULL || obj->parentChunk->chunkPosition.x != currentChunkPos.x
        || obj->parentChunk->chunkPosition.y != currentChunkPos.y)
    {
        RemoveFromChunk(obj);
        AddToChunk(obj);
    }
}

void UpdatePlayer(Object* obj)
{
    if (obj->entity.entityHealth <= 0)
    {
        Context_SetMode(&endGameMode);
        Context_FinishMode();
    }
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

    Sprite sprite;
    Sprite_Initialize(&sprite);
    sprite.currentTexture = Texture_GetTextureById(obj->textureId);
    sprite.scale          = TEXTURE_SCALE;
    sprite.isVisible      = true;
    sprite.tint           = WHITE;
    sprite.position       = Utils_GridToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
    if (!Stopwatch_IsElapsed(&obj->entity.entityMovementTimer))
    {
        sprite.position.x +=
            float(-obj->entity.entityMovementDirection.x
                  * Stopwatch_GetPercentRemainingTime(&obj->entity.entityMovementTimer) * TEXTURE_SIZE * TEXTURE_SCALE);
        sprite.position.y +=
            float(-obj->entity.entityMovementDirection.y
                  * Stopwatch_GetPercentRemainingTime(&obj->entity.entityMovementTimer) * TEXTURE_SIZE * TEXTURE_SCALE);
    }
    Sprite_Add(&sprite);
}

void UpdateEnemy(Object* obj)
{
    switch (obj->entity.entityState)
    {
        case EntityState::PATROLLING:
        {
            Object* target = NULL;
            GetClosestEntityInRange(obj, obj->entity.entityChaseRadius, &target);
            if (target != NULL && target->entity.entityHealth > 0)
            {
                obj->entity.entityTarget = target;
                obj->entity.entityState  = EntityState::CHASING;
                break;
            }
            if (!Stopwatch_IsZero(&obj->entity.entityMovementTimer))
            {
                break;
            }
            int16_t     x    = Utils_GetRandomInRange(-(uint16_t)obj->entity.entityPatrolRadius,
                                                      (uint16_t)obj->entity.entityPatrolRadius);
            int16_t     y    = Utils_GetRandomInRange(-(uint16_t)obj->entity.entityPatrolRadius,
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
                    // LOG_INF("Enemy %d attacking target %d", obj->id, obj->entity.entityTarget->id);
                    if (obj->entity.entityTarget->entity.entityHealth <= obj->entity.entityDamage
                        || obj->entity.entityTarget->entity.entityHealth <= 0)
                    {
                        obj->entity.entityTarget->entity.entityHealth = 0;
                        obj->entity.entityExperience += obj->entity.entityTarget->entity.entityExperience;
                        obj->entity.entityState  = EntityState::PATROLLING;
                        obj->entity.entityTarget = NULL;
                        break;
                    }
                    else
                    {
                        obj->entity.entityTarget->entity.entityHealth -= obj->entity.entityDamage;
                        Stopwatch_Start(&obj->entity.entityAttackTimer,
                                        Stats_AttackDelay(obj->entity.entityAttackSpeed, obj->entity.entityDexterity));
                    }
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
    Sprite sprite;
    Sprite_Initialize(&sprite);
    sprite.currentTexture = Texture_GetTextureById(obj->textureId);
    sprite.scale          = TEXTURE_SCALE;
    sprite.isVisible      = true;
    sprite.tint           = WHITE;
    sprite.position       = Utils_GridToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
    if (!Stopwatch_IsElapsed(&obj->entity.entityMovementTimer))
    {
        sprite.position.x +=
            float(-obj->entity.entityMovementDirection.x
                  * Stopwatch_GetPercentRemainingTime(&obj->entity.entityMovementTimer) * TEXTURE_SIZE * TEXTURE_SCALE);
        sprite.position.y +=
            float(-obj->entity.entityMovementDirection.y
                  * Stopwatch_GetPercentRemainingTime(&obj->entity.entityMovementTimer) * TEXTURE_SIZE * TEXTURE_SCALE);
    }
    Sprite_Add(&sprite);
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
    Sprite sprite;
    Sprite_Initialize(&sprite);
    sprite.currentTexture = Texture_GetTextureById(obj->textureId);
    sprite.scale          = TEXTURE_SCALE;
    sprite.isVisible      = true;
    sprite.tint           = WHITE;
    sprite.position       = Utils_GridToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
    Sprite_Add(&sprite);
}

void UpdateUI()
{
    char buffer[32];

    snprintf(buffer, sizeof(buffer), "FPS: %d", GetFPS());
    Text text;
    text.position   = (Vector2Float){ -540.0f, -340.0f };
    text.buffer     = buffer;
    text.bufferSize = strlen(buffer);
    text.scale      = 2.0f;
    text.bounds =
        (Rectangle){ text.position.x, text.position.y, text.scale * 8.0f * text.bufferSize, text.scale * 8.0f };
    if (UI_Text(&text, "Anikki_square_8x8"))
    {
        // LOG_INF("Text clicked!");
    }

    if (gameData.playerObject == NULL)
    {
        return;
    }
    snprintf(buffer, sizeof(buffer), "Player health: %d", gameData.playerObject->entity.entityHealth);
    text.position   = (Vector2Float){ -540.0f, -240.0f };
    text.buffer     = buffer;
    text.bufferSize = strlen(buffer);
    text.scale      = 1.0f;
    text.bounds =
        (Rectangle){ text.position.x, text.position.y, text.scale * 8.0f * text.bufferSize, text.scale * 8.0f };
    if (UI_Text(&text, "Anikki_square_8x8"))
    {
        LOG_INF("Text clicked!");
    }

    ProgressBar progressbar;
    progressbar.backgroundTexture = NULL;
    progressbar.progressTexture   = Texture_GetTextureByName("Anikki_square_8x8_23");
    progressbar.position          = (Vector2Float){ -550.0f, -200.0f };
    progressbar.scale             = 4.0f;
    progressbar.bounds       = (Rectangle){ progressbar.position.x, progressbar.position.y, 8.0f * progressbar.scale,
                                            8.0f * progressbar.scale };
    progressbar.minValue     = 0.0f;
    progressbar.maxValue     = 100.0f;
    progressbar.currentValue = (float)gameData.playerObject->entity.entityHealth;
    UI_ProgressBar(&progressbar);

    ItemSlot itemSlot;
    for (uint8_t i = 0; i < ENTITY_MAX_ITEMS; i++)
    {
        itemSlot.position = (Vector2Float){ -550.0f + i % 4 * 34.0f, -150.0f + (i / 4 * 34.0f) };//integer division on purpose
        itemSlot.scale    = 4.0f;
        itemSlot.bounds =
            (Rectangle){ itemSlot.position.x, itemSlot.position.y, 8.0f * itemSlot.scale, 8.0f * itemSlot.scale };
        itemSlot.backgroundTexture = Texture_GetTextureByName("Anikki_square_8x8_211");
        if (gameData.playerObject->entity.entityItems[i] == NULL)
        {
            itemSlot.itemTexture = NULL;
        }
        else
        {
            itemSlot.itemTexture = Texture_GetTextureById(gameData.playerObject->entity.entityItems[i]->textureId);
        }
        if (UI_ItemSlot(&itemSlot))
        {
            if (Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
            {
                LOG_INF("Item slot %d clicked!", i);
                // if(gameData.playerObject->entity.entityItems[i] != NULL)
                // {
                //     gameData.isDraggingObject = true;
                //     gameData.draggedObject = gameData.playerObject->entity.entityItems[i];
                // }
            }
            if (Input_IsMouseButtonReleased(INPUT_MOUSE_BUTTON_LEFT))
            {
                LOG_INF("Item slot %d released!", i);
                // Pick up item if dragging
                if (gameData.isDraggingObject && gameData.draggedObject != NULL)
                {
                    LOG_INF("Picking up item id %d", gameData.draggedObject->id);
                    gameData.playerObject->entity.entityItems[i] = gameData.draggedObject;
                    RemoveFromChunk(gameData.draggedObject);
                    gameData.isDraggingObject = false;
                    gameData.draggedObject    = NULL;
                }
            }
        }
    }
}

void UpdateDragItems()
{
    if (Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_LEFT))
    {
        // LOG_INF("Mouse button down");
        Vector2    mousePos = { (float)(Input_GetMouseX()), (float)(Input_GetMouseY()) };
        Vector2    worldPos = GetScreenToWorld2D(mousePos, *Window_GetCamera());
        Vector3Int gridPos  = Utils_WorldToGrid(worldPos, TEXTURE_SIZE * TEXTURE_SCALE);
        Object*    objects[MAX_LAYERS];
        uint8_t    objCount = GetObjectsAtPosition(gridPos, objects);
        if (objCount > 0)
        {
            for (uint16_t i = objCount; i > 0; i--)
            {
                if (objects[i - 1]->type != Type::ITEM)
                {
                    continue;  // Skip items
                }
                Vector2Int playerPos = { gameData.playerObject->position.x, gameData.playerObject->position.y };
                Vector2Int objectPos = { objects[i - 1]->position.x, objects[i - 1]->position.y };
                if (Utils_ManhattanDistance(playerPos, objectPos) > 5)
                {
                    continue;  // Too far away
                }
                gameData.draggedObject    = objects[i - 1];  // Drag the topmost object
                gameData.isDraggingObject = true;
                LOG_INF("Dragging object id %d", gameData.draggedObject->id);
                break;
            }
        }
    }
    if (Input_IsMouseButtonReleased(INPUT_MOUSE_BUTTON_LEFT))
    {
        // LOG_INF("Mouse button released");
        if (gameData.isDraggingObject)
        {
            Vector2Int playerPos = { gameData.playerObject->position.x, gameData.playerObject->position.y };
            Vector2Int objectPos = { gameData.draggedObject->position.x, gameData.draggedObject->position.y };
            if (Utils_ManhattanDistance(playerPos, objectPos) > 5)
            {
                LOG_INF("Cannot drop object id %d, too far from player", gameData.draggedObject->id);
                gameData.isDraggingObject = false;
                gameData.draggedObject    = NULL;
                return;  // Too far away
            }
            LOG_INF("Dropping dragged object id %d", gameData.draggedObject->id);
            Vector2    mousePos              = { (float)(Input_GetMouseX()), (float)(Input_GetMouseY()) };
            Vector2    worldPos              = GetScreenToWorld2D(mousePos, *Window_GetCamera());
            Vector3Int gridPos               = Utils_WorldToGrid(worldPos, TEXTURE_SIZE * TEXTURE_SCALE);
            gameData.draggedObject->position = gridPos;
            gameData.isDraggingObject        = false;
            gameData.draggedObject           = NULL;
        }
    }
}

void MainMode_OnStart()
{
    UI_Init(&gameData.cameraEntity);
    Texture_LoadTextureSheet("resources/sprites/Anikki_square_8x8.png", 8, 8, 256);

    Window_GetCamera()->target = (Vector2){ 0.0f, 0.0f };
    LoadWorldMap((char*)worldMap, WORLD_MAP_SIZE, WORLD_MAP_SIZE, gameData.chunks);
    for (uint16_t i = 0; i < gameData.objectCount; i++)
    {
        Object* obj = &gameData.objects[i];
        if (obj->type == Type::ENTITY)
        {
            Stopwatch_Stop(&obj->entity.entityMovementTimer);
            Stopwatch_Stop(&obj->entity.entityAttackTimer);
        }
    }
    Sprite_SetPool(gameData.sprites, SPRITE_MAX_COUNT);
}

void MainMode_OnPause()
{
}

void MainMode_Update()
{
    Sprite_Clear();
    DrawDebug();
    // check what objects are in view of the camera and draw them
    Vector3Int8 camPosChunk =
        Utils_WorldToChunk(gameData.cameraEntity.position, TEXTURE_SIZE * TEXTURE_SCALE, CHUNK_SIZE);
    camPosChunk.z = gameData.currentZPos;
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
            switch (visibleChunks[i]->objects[j]->type)
            {
                case Type::TILE:
                {
                    Sprite sprite;
                    Sprite_Initialize(&sprite);
                    sprite.currentTexture = Texture_GetTextureById(visibleChunks[i]->objects[j]->textureId);
                    sprite.position =
                        Utils_GridToWorld(visibleChunks[i]->objects[j]->position, TEXTURE_SIZE * TEXTURE_SCALE);
                    sprite.scale     = TEXTURE_SCALE;
                    sprite.isVisible = true;
                    sprite.zOrder    = visibleChunks[i]->objects[j]->layer;
                    Sprite_Add(&sprite);
                }
                break;
                case Type::ENTITY:
                {
                    Object* obj = visibleChunks[i]->objects[j];
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
                    UpdateProjectile(visibleChunks[i]->objects[j]);
                    // Update projectile logic here
                    break;
                case Type::EFFECT:
                    UpdateEffect(visibleChunks[i]->objects[j]);
                    // Update effect logic here
                    break;
                case Type::INTERACTIVE:
                    UpdateInteractive(visibleChunks[i]->objects[j]);
                    // Update interactive logic here
                    break;
                case Type::ITEM:
                    UpdateItem(visibleChunks[i]->objects[j]);
                    // Update item logic here
                    break;
                default:
                    break;
            }
            UpdateObjectChunk(visibleChunks[i]->objects[j]);
        }
    }
    UpdateUI();
    UpdateDragItems();
    // update camera for sprite rendering
    gameData.cameraEntity.position = Window_GetCamera()->target;
    gameData.cameraEntity.scale    = 1.0f / Window_GetCamera()->zoom;
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
