#include "MainMode.h"

#include "engine/components/Audio.h"
#include "engine/components/Collider2D.h"
#include "engine/components/Entity2D.h"
#include "engine/components/Sprite.h"
#include "engine/components/Texture.h"
#include "engine/io/Input.h"
#include "engine/io/Window.h"
#include "engine/misc/DeltaTime.h"
#include "engine/misc/Logger.h"
#include "engine/misc/Stopwatch.h"
#include "engine/misc/Utils.h"
#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"
#include <stdio.h>
#include "utils/Structs.h"
#include "utils/Prefabs.h"

Mode mainMode = MODE_FROM_CLASSNAME(MainMode);

GameData gameData;
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
        ImGui::Text("Select a texture to paint with:");
        for (int i = 0; i < 256; i++)
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

        const char* items[] = { "TILE", "ENTITY", "PROJECTILE", "EFFECT", "INTERACTIVE", "ITEM" };
        ImGui::ListBox("Type", &debugData.currentObject.type, items, IM_ARRAYSIZE(items), 5);

        if (debugData.currentObject.type == Type::ENTITY)
        {
            const char* entityItems[] = { "PLAYER", "ENEMY" };
            ImGui::ListBox("Entity Type", &debugData.currentObject.entity.entityType, entityItems,
                           IM_ARRAYSIZE(entityItems), 2);
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
        else if (debugData.currentObject.type == Type::PROJECTILE)
        {
            ImGui::InputFloat("Speed", &debugData.currentObject.projectile.projectileSpeed);
            ImGui::InputFloat2("Direction", (float*)&debugData.currentObject.projectile.projectileDirection);
            ImGui::InputInt("Range", &debugData.currentObject.projectile.projectileRange);
            ImGui::InputInt("Damage", &debugData.currentObject.projectile.projectileDamage);
        }
        else if (debugData.currentObject.type == Type::EFFECT)
        {
            ImGui::InputInt("Effect Type", &debugData.currentObject.effect.effectType);
            ImGui::InputFloat("Duration", &debugData.currentObject.effect.effectDuration);
        }
        else if (debugData.currentObject.type == Type::INTERACTIVE)
        {
            // No additional properties for TILE
            const char* items[] = { "CHEST", "DOOR", "STAIRS_UP", "STAIRS_DOWN" };
            ImGui::ListBox("Type", &debugData.currentObject.interactive.interactiveType, items, IM_ARRAYSIZE(items), 4);
        }
        else if (debugData.currentObject.type == Type::ITEM)
        {
            ImGui::InputInt("Item ID", &debugData.currentObject.item.itemId);
        }

        ImGui::Text("Mouse Position: (%.1f, %.1f)", mousePos.x, mousePos.y);
        ImGui::Text("World Position: (%.1f, %.1f)", worldPos.x, worldPos.y);
        ImGui::Text("Grid Position: (%d, %d)", gridPos.x, gridPos.y);
        ImGui::Text("Chunk Position: (%d, %d)", chunkPos.x, chunkPos.y);


        ImGui::End();
    }
}

bool checkCollision(Vector3Int pos)
{
    Objects objsAtPos = GetObjectsAtPosition(pos);
    for (uint16_t i = 0; i < objsAtPos.count; i++)
    {
        if (objsAtPos.objects[i].isCollidable)
        {
            // Collision detected
            LOG_INF("Collision detected at (%d, %d, %d) with object id %d", pos.x, pos.y, pos.z,
                    objsAtPos.objects[i].id);
            return true;
        }
    }
    return false;
    // No collision
}

bool checkCollision(Vector3Int pos, Object* outObj)
{
    Objects objsAtPos = GetObjectsAtPosition(pos);
    for (uint16_t i = 0; i < objsAtPos.count; i++)
    {
        if (objsAtPos.objects[i].isCollidable)
        {
            // Collision detected
            LOG_INF("Collision detected at (%d, %d, %d) with object id %d", pos.x, pos.y, pos.z,
                    objsAtPos.objects[i].id);
            *outObj = objsAtPos.objects[i];
            return true;
        }
    }
    return false;
    // No collision
}

void UpdateEntity(Object* obj)
{
    if (obj->entity.entityType == EntityType::PLAYER)
    {
        Camera2D* camera   = Window_GetCamera();
        Vector2   worldPos = Utils_GridCenterToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
        camera->target.x += (worldPos.x - camera->target.x) * DeltaTime_GetDeltaTime() * 2.0f;
        camera->target.y += (worldPos.y - camera->target.y) * DeltaTime_GetDeltaTime() * 2.0f;
        // Example: simple player movement logic
        int8_t y = (Input_IsKeyDown(INPUT_KEYCODE_S) - Input_IsKeyDown(INPUT_KEYCODE_W));
        int8_t x = (Input_IsKeyDown(INPUT_KEYCODE_D) - Input_IsKeyDown(INPUT_KEYCODE_A));
        if (abs(x + y) > 0 && abs(x + y) < 2)
        {
            if (Stopwatch_IsZero(&obj->entity.entityMovementTimer))
            {
                if (!checkCollision({ obj->position.x + x, obj->position.y + y, obj->position.z }))
                {
                    obj->position.x += x;
                    obj->position.y += y;
                    obj->entity.entityMovementDirection = { x, y };
                    Stopwatch_Start(&obj->entity.entityMovementTimer, 500);
                }
            }
        }

        Sprite* sprite = &gameData.sprites[gameData.spriteCount++];
        Sprite_Initialize(sprite);
        sprite->currentTexture = Texture_GetTextureById(obj->textureId);
        sprite->position       = Utils_GridToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
        sprite->scale          = TEXTURE_SCALE;
        sprite->isVisible      = true;
        sprite->tint           = WHITE;
        sprite->position       = Utils_GridToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
        if (!Stopwatch_IsElapsed(&obj->entity.entityMovementTimer))
        {
            sprite->position.x += float(-obj->entity.entityMovementDirection.x
                                        * Stopwatch_GetPercentRemainingTime(&obj->entity.entityMovementTimer)
                                        * TEXTURE_SIZE * TEXTURE_SCALE);
            sprite->position.y += float(-obj->entity.entityMovementDirection.y
                                        * Stopwatch_GetPercentRemainingTime(&obj->entity.entityMovementTimer)
                                        * TEXTURE_SIZE * TEXTURE_SCALE);
        }
        Sprite_Add(sprite);

        // obj->entity.entitySpriteOffset = { float(-x * TEXTURE_SIZE * TEXTURE_SCALE),
        // float(-y * TEXTURE_SIZE * TEXTURE_SCALE) };
    }
    if (obj->entity.entityType == EntityType::ENEMY)
    {
        // Example: simple enemy AI logic
        int8_t x = (rand() % 3) - 1;  // -1, 0, or 1
        int8_t y = (rand() % 3) - 1;  // -1, 0, or 1
        // if (x == 0 && y == 0)
        // {
        //     return;
        // }
        if (x != 0 || y != 0)
        {
            if (!Stopwatch_IsZero(&obj->entity.entityMovementTimer))
            {
                if (checkCollision({ obj->position.x + x, obj->position.y + y, obj->position.z }))
                {
                    obj->position.x += (rand() % 3) - 1;  // Move randomly left, right, or stay
                    obj->position.y += (rand() % 3) - 1;  // Move randomly up, down, or stay
                    Stopwatch_Start(&obj->entity.entityMovementTimer, 100);
                }
            }
        }
        Sprite* sprite = &gameData.sprites[gameData.spriteCount++];
        Sprite_Initialize(sprite);
        sprite->currentTexture = Texture_GetTextureById(obj->textureId);
        sprite->position       = Utils_GridToWorld(obj->position, TEXTURE_SIZE * TEXTURE_SCALE);
        sprite->scale          = TEXTURE_SCALE;
        sprite->isVisible      = true;
        sprite->tint           = LIGHTGRAY;
        sprite->position.x += float(-x * TEXTURE_SIZE * TEXTURE_SCALE);
        sprite->position.y += float(-y * TEXTURE_SIZE * TEXTURE_SCALE);
        Sprite_Add(sprite);
    }
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

    gameData.chunkCount = LoadChunksFromFile(gameData.chunks);
    for (uint16_t i = 0; i < gameData.chunkCount; i++)
    {
        Chunk* chunk = &gameData.chunks[i];
        for (uint16_t j = 0; j < chunk->objectCount; j++)
        {
            Object* obj = &chunk->objects[j];
            if (obj->type == Type::ENTITY)
            {
                Stopwatch_Stop(&obj->entity.entityMovementTimer);
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
                    UpdateEntity(&visibleChunks[i]->objects[j]);
                    // Update entity logic here
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
    SaveChunksToFile(gameData.chunks, gameData.chunkCount);
    Texture_UnloadTextures();
    Audio_UnloadAudios();
}

void MainMode_OnResume()
{
}
