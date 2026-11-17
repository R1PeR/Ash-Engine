#include "MainMode.h"

#include "MapEditorMode.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"
#include "utils/UI.h"

#include <stdio.h>
#include <string.h>

void UpdateDebug(MainMode* state)
{
    char fps[32];
    snprintf(fps, sizeof(fps), "FPS: %d", (int)(1.0f / state->gameData.dt));
    char deltaTime[32];
    snprintf(deltaTime, sizeof(deltaTime), "Delta Time: %.4f", state->gameData.dt);
    char playerPos[32];
    snprintf(playerPos, sizeof(playerPos), "Player Pos: (%.2f, %.2f)", state->gameData.player.entity.position.x,
             state->gameData.player.entity.position.y);
    char playerVel[32];
    snprintf(playerVel, sizeof(playerVel), "Player Vel: (%.2f, %.2f)", state->gameData.player.velocity.x,
             state->gameData.player.velocity.y);
    char playerOnGround[32];
    snprintf(playerOnGround, sizeof(playerOnGround), "Player onGround: %s",
             state->gameData.player.onGround ? "true" : "false");
    char playerOnWall[32];
    snprintf(playerOnWall, sizeof(playerOnWall), "Player onWall: %s",
             (state->gameData.player.onWall != 0) ? "true" : "false");
    char playerJumpCoyoteTime[32];
    snprintf(playerJumpCoyoteTime, sizeof(playerJumpCoyoteTime), "Player jumpCoyoteTime: %s",
             Stopwatch_IsRunning(&state->gameData.player.jumpCoyoteTime) ? "true" : "false");
    char playerWallCoyoteTime[32];
    snprintf(playerWallCoyoteTime, sizeof(playerWallCoyoteTime), "Player wallCoyoteTime: %s",
             Stopwatch_IsRunning(&state->gameData.player.wallCoyoteTime) ? "true" : "false");
    char cameraPos[32];
    snprintf(cameraPos, sizeof(cameraPos), "Camera Pos: (%.2f, %.2f)", state->camera.position.x,
             state->camera.position.y);


    UI_Begin(&state->uiState, UI_GetBounds(&state->uiState, AnchorTopLeft, { 0, 0, 0.4, 0.3 }));
    UI_Frame(&state->uiState);
    {
        UI_Layout(&state->uiState, LayoutVertical);
        // UI_Center(Center);
        UI_Padding(&state->uiState, UI_GetSize(&state->uiState, { 0.01, 0.01, 0.01, 0.01 }));
        UI_Text(&state->uiState, "DEBUG INFO", 2.0f, state->fontTextures);
        UI_Text(&state->uiState, fps, 1.0f, state->fontTextures);
        UI_Text(&state->uiState, deltaTime, 1.0f, state->fontTextures);
        UI_Text(&state->uiState, playerPos, 1.0f, state->fontTextures);
        UI_Text(&state->uiState, playerVel, 1.0f, state->fontTextures);
        UI_Text(&state->uiState, playerOnGround, 1.0f, state->fontTextures);
        UI_Text(&state->uiState, playerOnWall, 1.0f, state->fontTextures);
        UI_Text(&state->uiState, playerJumpCoyoteTime, 1.0f, state->fontTextures);
        UI_Text(&state->uiState, playerWallCoyoteTime, 1.0f, state->fontTextures);
        UI_Text(&state->uiState, cameraPos, 1.0f, state->fontTextures);
    }
    UI_End(&state->uiState);
}

void UpdateEditorTiles(MainMode* state)
{
    (void)state;
    // if (!g_editorTestMapData.isValid || !editorTilesLoaded)
    //     return;
    // for (int l = 0; l < MAP_MAX_LAYERS; l++)
    // {
    //     TileLayer* layer = &g_editorTestMapData.mapData.layers[l];
    //     for (int i = 0; i < (int)layer->tileCount && renderStackItemCount < renderStackItem_MAX; i++)
    //     {
    //         Tile* tile = &layer->tiles[i];
    //         if (tile->textureId >= MAP_TILESET_COUNT)
    //             continue;
    //         Sprite sprite;
    //         Sprite_Initialize(&sprite);
    //         sprite.currentTexture                         = &editorTileTextures[tile->textureId];
    //         sprite.position.x                             = (float)(tile->position.x * TILE_SIZE);
    //         sprite.position.y                             = (float)(tile->position.y * TILE_SIZE);
    //         sprite.scale                                  = 2.0f;
    //         state->renderStackItems[state->renderStackItemCount].sprite = sprite;
    //         state->renderStackItems[state->renderStackItemCount].type   = renderStackItem_SPRITE;
    //         state->renderStackItemCount++;
    //     }
    // }
}

void UpdateGame(MainMode* state)
{
    // get input
    int directionX = { -Input_IsKeyDown(INPUT_KEYCODE_A) + Input_IsKeyDown(INPUT_KEYCODE_D) };
    int directionY = { -Input_IsKeyDown(INPUT_KEYCODE_S) + Input_IsKeyDown(INPUT_KEYCODE_W) };
    (void)directionY;  // unused for now
    bool jump = Input_IsKeyPressed(INPUT_KEYCODE_SPACE);

    // update player state
    if (state->gameData.player.onGround)
    {
        Stopwatch_Start(&state->gameData.player.jumpCoyoteTime, 100);
        Stopwatch_Stop(&state->gameData.player.wallCoyoteTime);
        state->gameData.player.onWall = 0;
        // Apply ground friction
        if (state->gameData.player.velocity.x > 0.0f)
        {
            float deltaFriction = FRICTION_GROUND * state->gameData.dt;
            if (state->gameData.player.velocity.x - deltaFriction < 0.0f)
            {
                state->gameData.player.velocity.x = 0.0f;
            }
            else
            {
                state->gameData.player.velocity.x -= deltaFriction;
            }
            if (state->gameData.player.sprite.currentAnimation != &state->gameData.playerWalkAnimation)
            {
                AnimatedSprite_Play(&state->gameData.player.sprite, &state->gameData.playerWalkAnimation, true);
            }
        }
        if (state->gameData.player.velocity.x < 0.0f)
        {
            float deltaFriction = FRICTION_GROUND * state->gameData.dt;
            if (state->gameData.player.velocity.x + deltaFriction > 0.0f)
            {
                state->gameData.player.velocity.x = 0.0f;
            }
            else
            {
                state->gameData.player.velocity.x += deltaFriction;
            }
            if (state->gameData.player.sprite.currentAnimation != &state->gameData.playerWalkAnimation)
            {
                AnimatedSprite_Play(&state->gameData.player.sprite, &state->gameData.playerWalkAnimation, true);
            }
        }
        if (state->gameData.player.velocity.x == 0.0f)
        {
            if (state->gameData.player.sprite.currentAnimation != &state->gameData.playerIdleAnimation)
            {
                AnimatedSprite_Play(&state->gameData.player.sprite, &state->gameData.playerIdleAnimation, true);
            }
        }
        // Apply ground movement
        if (state->gameData.player.velocity.x < PLAYER_MAX_SPEED
            && state->gameData.player.velocity.x > -PLAYER_MAX_SPEED)
        {
            state->gameData.player.velocity.x += (directionX * DIRECTION_SPEED_GROUND * state->gameData.dt);
        }
        state->gameData.player.velocity.y += GRAVITY * state->gameData.dt;
    }
    else if (state->gameData.player.onWall != 0)
    {
        if (state->gameData.player.sprite.currentAnimation != &state->gameData.playerWallSlideAnimation)
        {
            AnimatedSprite_Play(&state->gameData.player.sprite, &state->gameData.playerWallSlideAnimation, true);
        }

        Stopwatch_Stop(&state->gameData.player.jumpCoyoteTime);
        // Apply gravity
        state->gameData.player.velocity.y += GRAVITY * state->gameData.dt;
        // Apply wall friction
        if (state->gameData.player.velocity.y > 0.0f)
        {
            float deltaFriction = FRICTION_WALL * state->gameData.dt;
            state->gameData.player.velocity.y -= deltaFriction;
        }
        if (state->gameData.player.velocity.y < 0.0f)
        {
            float deltaFriction = FRICTION_WALL * state->gameData.dt;
            state->gameData.player.velocity.y += deltaFriction;
        }
        if (jump)
        {
            state->gameData.player.velocity.y = -PLAYER_JUMP_FORCE / 2;
            if (state->gameData.player.onWall == 1)
            {
                state->gameData.player.velocity.x = -PLAYER_JUMP_FORCE / 2;
                if (Stopwatch_IsRunning(&state->gameData.player.wallCoyoteTime))
                {
                    state->gameData.player.velocity.x -=
                        Utils_AbsFloat(state->gameData.player.velocityBeforeCollision.x) / 2;
                    state->gameData.player.velocity.y -=
                        Utils_AbsFloat(state->gameData.player.velocityBeforeCollision.x) / 2;
                }
            }
            else
            {
                state->gameData.player.velocity.x = PLAYER_JUMP_FORCE / 2;
                if (Stopwatch_IsRunning(&state->gameData.player.wallCoyoteTime))
                {
                    state->gameData.player.velocity.x +=
                        Utils_AbsFloat(state->gameData.player.velocityBeforeCollision.x) / 2;
                    state->gameData.player.velocity.y -=
                        Utils_AbsFloat(state->gameData.player.velocityBeforeCollision.x) / 2;
                }
            }
            state->gameData.player.onWall = false;
        }
        if (directionX != 0)
        {
            if (state->gameData.player.onWall != directionX)
            {
                state->gameData.player.onWall = false;
            }
        }
    }
    else
    {
        Stopwatch_Stop(&state->gameData.player.wallCoyoteTime);
        if (state->gameData.player.velocity.x > 0.0f)
        {
            float deltaFriction = FRICTION_AIR * state->gameData.dt;
            if (state->gameData.player.velocity.x - deltaFriction < 0.0f)
            {
                state->gameData.player.velocity.x = 0.0f;
            }
            else
            {
                state->gameData.player.velocity.x -= deltaFriction;
            }
            if (state->gameData.player.sprite.currentAnimation != &state->gameData.playerAirMoveAnimation)
            {
                AnimatedSprite_Play(&state->gameData.player.sprite, &state->gameData.playerAirMoveAnimation, true);
            }
        }
        if (state->gameData.player.velocity.x < 0.0f)
        {
            float deltaFriction = FRICTION_AIR * state->gameData.dt;
            if (state->gameData.player.velocity.x + deltaFriction > 0.0f)
            {
                state->gameData.player.velocity.x = 0.0f;
            }
            else
            {
                state->gameData.player.velocity.x += deltaFriction;
            }
            if (state->gameData.player.sprite.currentAnimation != &state->gameData.playerAirMoveAnimation)
            {
                AnimatedSprite_Play(&state->gameData.player.sprite, &state->gameData.playerAirMoveAnimation, true);
            }
        }
        if (state->gameData.player.velocity.x == 0.0f)
        {
            if (state->gameData.player.sprite.currentAnimation != &state->gameData.playerAirIdleAnimation)
            {
                AnimatedSprite_Play(&state->gameData.player.sprite, &state->gameData.playerAirIdleAnimation, true);
            }
        }
        // Apply gravity
        state->gameData.player.velocity.y += GRAVITY * state->gameData.dt;
        // Apply air movement
        if ((state->gameData.player.velocity.x < PLAYER_MAX_SPEED || directionX < 0)
            && (state->gameData.player.velocity.x > -PLAYER_MAX_SPEED || directionX > 0))
        {
            state->gameData.player.velocity.x += (directionX * DIRECTION_SPEED_AIR * state->gameData.dt);
        }
    }
    if (Stopwatch_IsRunning(&state->gameData.player.jumpCoyoteTime))
    {
        if (jump)
        {
            state->gameData.player.velocity.y = -PLAYER_JUMP_FORCE;
            Stopwatch_Stop(&state->gameData.player.jumpCoyoteTime);
        }
    }
    state->gameData.player.velocity.y =
        Utils_ClampFloat(state->gameData.player.velocity.y, -PLAYER_MAX_VELOCITY, PLAYER_MAX_VELOCITY);
    state->gameData.player.velocity.x =
        Utils_ClampFloat(state->gameData.player.velocity.x, -PLAYER_MAX_VELOCITY, PLAYER_MAX_VELOCITY);

    // check collisions
    state->gameData.player.onGround = false;

    state->gameData.player.entity.position.x += state->gameData.player.velocity.x * state->gameData.dt;
    for (uint32_t i = 0; i < state->gameData.map.platformCount; i++)
    {
        if (Collider2D_CheckCollider(&state->gameData.player.collider, &state->gameData.map.platforms[i].collider))
        {
            float playerPos    = state->gameData.player.entity.position.x;
            float playerSize   = state->gameData.player.collider.size.x;
            float platformPos  = state->gameData.map.platforms[i].entity.position.x;
            float platformSize = state->gameData.map.platforms[i].collider.size.x;
            float overlapX     = Utils_MinFloat(playerPos + playerSize, platformPos + platformSize)
                             - Utils_MaxFloat(playerPos, platformPos);

            state->gameData.player.velocityBeforeCollision.x = state->gameData.player.velocity.x;

            // Resolve X-overlap
            if (playerPos < platformPos)
            {
                state->gameData.player.entity.position.x -= overlapX;
                state->gameData.player.onWall = 1;
            }
            else
            {
                state->gameData.player.entity.position.x += overlapX;
                state->gameData.player.onWall = -1;
            }
            Stopwatch_Start(&state->gameData.player.wallCoyoteTime, 100);
            state->gameData.player.velocity.x = 0.0f;
        }
    }

    state->gameData.player.entity.position.y += state->gameData.player.velocity.y * state->gameData.dt;
    for (uint32_t i = 0; i < state->gameData.map.platformCount; i++)
    {
        if (Collider2D_CheckCollider(&state->gameData.player.collider, &state->gameData.map.platforms[i].collider))
        {
            float playerPos    = state->gameData.player.entity.position.y;
            float playerSize   = state->gameData.player.collider.size.y;
            float platformPos  = state->gameData.map.platforms[i].entity.position.y;
            float platformSize = state->gameData.map.platforms[i].collider.size.y;
            float overlapY     = Utils_MinFloat(playerPos + playerSize, platformPos + platformSize)
                             - Utils_MaxFloat(playerPos, platformPos);

            state->gameData.player.velocityBeforeCollision.y = state->gameData.player.velocity.y;

            // Resolve X-overlap
            if (playerPos < platformPos)
            {
                state->gameData.player.entity.position.y -= overlapY;
                state->gameData.player.velocity.y = 0.0f;
                state->gameData.player.onGround   = true;
            }
            else
            {
                state->gameData.player.entity.position.y += overlapY;
                state->gameData.player.velocity.y = 0.0f;
            }
        }
    }

    // Draw debug
    Shape2D shape;
    Shape2D_Initialize(&shape);
    shape.type                                                 = SHAPE2D_RECTANGLE_LINES;
    shape.parent                                               = &state->gameData.player.entity;
    shape.position.x                                           = state->gameData.player.collider.position.x;
    shape.position.y                                           = state->gameData.player.collider.position.y;
    shape.rectangle.width                                      = state->gameData.player.collider.size.x;
    shape.rectangle.height                                     = state->gameData.player.collider.size.y;
    shape.color                                                = (Color){ 255, 0, 0, 255 };
    shape.rectangle.outlineThickness                           = 1.0f;
    state->renderStackItems[state->renderStackItemCount].shape = shape;
    state->renderStackItems[state->renderStackItemCount].type  = RENDERCOMMAND_DRAW_SHAPE;
    state->renderStackItemCount++;

    for (uint32_t i = 0; i < state->gameData.map.platformCount; i++)
    {
        Platform* platform = &state->gameData.map.platforms[i];
        Shape2D_Initialize(&shape);
        shape.type                                                 = SHAPE2D_RECTANGLE_LINES;
        shape.parent                                               = &platform->entity;
        shape.position.x                                           = platform->collider.position.x;
        shape.position.y                                           = platform->collider.position.y;
        shape.rectangle.width                                      = platform->collider.size.x;
        shape.rectangle.height                                     = platform->collider.size.y;
        shape.color                                                = (Color){ 0, 255, 0, 255 };
        state->renderStackItems[state->renderStackItemCount].shape = shape;
        state->renderStackItems[state->renderStackItemCount].type  = RENDERCOMMAND_DRAW_SHAPE;
        state->renderStackItemCount++;
    }
    AnimatedSprite_Update(&state->gameData.player.sprite);
}

void MainMode_OnStart(Context* context, void* data)
{
    (void)context;
    MainMode* state = (MainMode*)data;
    // editorTilesLoaded                 = false;
    state->gameData.map.platformCount = 0;

    Entity2D_Initialize(&state->gameData.player.entity);
    Collider2D_Initialize(&state->gameData.player.collider);
    state->gameData.player.entity.position = (Vector2Float){ 0.0f, 0.0f };
    state->gameData.player.velocity        = (Vector2Float){ 0.0f, 0.0f };
    state->gameData.player.collider.parent = &state->gameData.player.entity;
    state->gameData.player.collider.size   = (Vector2Float){ 16.0f, 16.0f };
    AnimatedSprite_Initialize(&state->gameData.player.sprite);

    // if (g_editorTestMapData.isValid)
    // {
    //     /* Build platforms from editor SOLID / JUMP_PLATFORM tiles */
    //     for (int l = 0; l < MAP_MAX_LAYERS; l++)
    //     {
    //         TileLayer* layer = &g_editorTestMapData.mapData.layers[l];
    //         for (int i = 0; i < (int)layer->tileCount; i++)
    //         {
    //             Tile* tile = &layer->tiles[i];
    //             if ((tile->type == TILE_TYPE_SOLID || tile->type == TILE_TYPE_JUMP_PLATFORM)
    //                 && state->gameData.map.platformCount < 128)
    //             {
    //                 Platform* p = &state->gameData.map.platforms[state->gameData.map.platformCount++];
    //                 Entity2D_Initialize(&p->entity);
    //                 Collider2D_Initialize(&p->collider);
    //                 Sprite_Initialize(&p->sprite);
    //                 p->entity.position.x = (float)(tile->position.x * TILE_SIZE);
    //                 p->entity.position.y = (float)(tile->position.y * TILE_SIZE);
    //                 p->collider.parent   = &p->entity;
    //                 p->collider.size     = (Vector2Float){ (float)TILE_SIZE, (float)TILE_SIZE };
    //             }
    //             if (tile->type == TILE_TYPE_PLAYER_SPAWN)
    //             {
    //                 state->gameData.player.entity.position.x = (float)(tile->position.x * TILE_SIZE);
    //                 state->gameData.player.entity.position.y = (float)(tile->position.y * TILE_SIZE);
    //             }
    //         }
    //     }
    //     /* Load tileset for visual rendering */
    //     editorTileAtlasBase = Texture_LoadTexture("resources/sprites/tileset.png", NULL);
    //     if (Texture_CreateTextureAtlas(editorTileAtlasBase, MAP_TILESET_COLS, MAP_TILESET_ROWS, editorTileTextures))
    //         editorTilesLoaded = true;
    // }
    // else
    // {
    Entity2D_Initialize(&state->gameData.map.platforms[0].entity);
    Collider2D_Initialize(&state->gameData.map.platforms[0].collider);
    Sprite_Initialize(&state->gameData.map.platforms[0].sprite);
    state->gameData.map.platforms[0].entity.position = (Vector2Float){ -100.0f, 100.0f };
    state->gameData.map.platforms[0].collider.parent = &state->gameData.map.platforms[0].entity;
    state->gameData.map.platforms[0].collider.size   = (Vector2Float){ 200.0f, 20.0f };
    state->gameData.map.platformCount++;

    Entity2D_Initialize(&state->gameData.map.platforms[1].entity);
    Collider2D_Initialize(&state->gameData.map.platforms[1].collider);
    Sprite_Initialize(&state->gameData.map.platforms[1].sprite);
    state->gameData.map.platforms[1].entity.position = (Vector2Float){ 50.0f, -400.0f };
    state->gameData.map.platforms[1].collider.parent = &state->gameData.map.platforms[1].entity;
    state->gameData.map.platforms[1].collider.size   = (Vector2Float){ 20.0f, 500.0f };
    state->gameData.map.platformCount++;

    Entity2D_Initialize(&state->gameData.map.platforms[2].entity);
    Collider2D_Initialize(&state->gameData.map.platforms[2].collider);
    Sprite_Initialize(&state->gameData.map.platforms[2].sprite);
    state->gameData.map.platforms[2].entity.position = (Vector2Float){ -50.0f, -100.0f };
    state->gameData.map.platforms[2].collider.parent = &state->gameData.map.platforms[2].entity;
    state->gameData.map.platforms[2].collider.size   = (Vector2Float){ 20.0f, 200.0f };
    state->gameData.map.platformCount++;
    // }

    state->tileAtlasBase = Texture_LoadTexture("resources/sprites/tileset.png", state->arena);
    LOG_INF("MainMode: tile atlas %dx%d", state->tileAtlasBase.size.x, state->tileAtlasBase.size.y);
    if (!Texture_CreateTextureAtlas(state->tileAtlasBase, TILESET_ATLAS_COLS, TILESET_ATLAS_ROWS, state->tileTextures))
        LOG_ERR("MainMode: failed to create tile atlas");

    state->fontAtlasBase = Texture_LoadTexture("resources/sprites/Anikki_square_8x8.png", state->arena);
    LOG_INF("MainMode: font atlas %dx%d", state->fontAtlasBase.size.x, state->fontAtlasBase.size.y);
    if (!Texture_CreateTextureAtlas(state->fontAtlasBase, FONT_ATLAS_COLS, FONT_ATLAS_ROWS, state->fontTextures))
        LOG_ERR("MainMode: failed to create font atlas");

    Entity2D_Initialize(&state->cameraEntity);

    Camera2D* camera = &state->camera;
    camera->zoom     = 1.0f;
    camera->position = (Vector2Float){ 0.0f, 0.0f };
    state->camera.rotation = 0.0f;
    state->camera.offset   = (Vector2Float){ state->window->width/2.0f, state->window->height/2.0f };

    state->cameraEntity.position.x = camera->position.x;
    state->cameraEntity.position.y = camera->position.y;
    state->cameraEntity.scale      = 1.0f / camera->zoom;

    UI_Initialize(&state->uiState, state->renderStackItems, (size_t*)&state->renderStackItemCount, RENDERSTACKITEM_MAX,
                  state->window, &state->camera);
    UI_SetParentEntity(&state->uiState, &state->cameraEntity);
}

void MainMode_OnPause(Context* context, void* data)
{
    (void)context;
    (void)data;
}

void MainMode_Update(Context* context, void* data)
{
    (void)context;
    MainMode* state             = (MainMode*)data;
    state->renderStackItemCount = 0;
    DeltaTime_Update(&state->deltaTime);
    state->gameData.dt = DeltaTime_GetDeltaTime(&state->deltaTime);

    state->renderStackItems[state->renderStackItemCount].type              = RENDERCOMMAND_CLEAR_SCREEN;
    state->renderStackItems[state->renderStackItemCount].clearScreen.color = (Color){ 0, 0, 0, 255 };
    state->renderStackItemCount++;

    state->renderStackItems[state->renderStackItemCount].type            = RENDERCOMMAND_BEGIN_CAMERA2D;
    state->renderStackItems[state->renderStackItemCount].camera.camera2D = &state->camera;
    state->renderStackItemCount++;

    state->renderStackItems[state->renderStackItemCount].type = RENDERCOMMAND_BEGIN2D;
    state->renderStackItemCount++;

    if (Input_IsKeyPressed(INPUT_KEYCODE_ESCAPE))
        Context_FinishMode(context);

    /* Camera follows player */
    UpdateGame(state);
    UpdateEditorTiles(state);
    UpdateDebug(state);

    Camera2D* camera               = &state->camera;
    camera->position.x             = state->gameData.player.entity.position.x;
    camera->position.y             = state->gameData.player.entity.position.y;
    state->cameraEntity.position.x = state->camera.position.x;
    state->cameraEntity.position.y = state->camera.position.y;
    state->cameraEntity.scale      = 1.0f / state->camera.zoom;

    state->renderStackItems[state->renderStackItemCount].type = RENDERCOMMAND_END_CAMERA2D;
    state->renderStackItemCount++;
    state->renderStackItems[state->renderStackItemCount].type = RENDERCOMMAND_END2D;
    state->renderStackItemCount++;
}

void MainMode_Draw(Context* context, void* data)
{
    (void)context;
    MainMode* state              = (MainMode*)data;
    state->renderStack.stack     = state->renderStackItems;
    state->renderStack.stackSize = state->renderStackItemCount;
    RenderStack_Draw(&state->renderStack);
}

void MainMode_OnStop(Context* context, void* data)
{
    (void)context;
    (void)data;
    // if (editorTilesLoaded)
    // {
    //     Texture_UnloadTexture(&editorTileAtlasBase);
    // }
    // g_editorTestMapData.isValid = false;
}

void MainMode_OnResume(Context* context, void* data)
{
    (void)context;
    (void)data;
}
