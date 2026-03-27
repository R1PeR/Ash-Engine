#include "ash_misc.h"

#include "ash_components.h"
#include "ash_io.h"
#include "imgui.h"

#include <math.h>
#include <stdio.h>
#include <time.h>

AStar_Node* AStar_CalucalatePath(AStar_Node* nodeArray, size_t nodeArraySize, const Vector2Int startPos,
                                 const Vector2Int targetPos, HeuristicFuncPtr hFunc)
{
    LOG_INF("A* Pathfinding from (%d, %d) to (%d, %d)", startPos.x, startPos.y, targetPos.x, targetPos.y);
    uint16_t nodeListCount = 0;

    AStar_Node* currentNode = NULL;
    // Initialize start node
    AStar_Node startNode;
    startNode.position = startPos;
    startNode.gCost    = 0;
    hFunc(startPos, targetPos, startNode.hCost);
    // startNode.hCost            =  ? UINT16_MAX : startNode.hCost;
    startNode.fCost            = startNode.gCost + startNode.hCost;
    startNode.parent           = NULL;
    startNode.closed           = false;
    startNode.valid            = true;
    nodeArray[nodeListCount++] = startNode;
    while (nodeListCount < nodeArraySize)
    {
        // Find node with lowest fCost
        uint16_t lowerstFCost = UINT16_MAX;
        uint16_t lowestIndex  = 0;
        bool     anyOpen      = false;
        for (uint16_t i = 0; i < nodeListCount; i++)
        {
            if (nodeArray[i].closed == true)
            {
                continue;
            }
            anyOpen = true;
            if (nodeArray[i].fCost < lowerstFCost)
            {
                lowerstFCost = nodeArray[i].fCost;
                lowestIndex  = i;
            }
        }
        if (!anyOpen)
        {
            // #if 1
            //             for (uint16_t i = 0; i < nodeListCount; i++)
            //             {
            //                 Vector3Int pos3D    = { nodeArray[i].position.x, nodeArray[i].position.y, 0 };
            //                 Vector2    worldPos = Utils_GridToWorld(pos3D, TEXTURE_SIZE * TEXTURE_SCALE);
            //                 DrawRectangleLines(int(worldPos.x), int(worldPos.y), int(TEXTURE_SIZE * TEXTURE_SCALE),
            //                                    int(TEXTURE_SIZE * TEXTURE_SCALE), BLUE);
            //             }
            // #endif
            LOG_WRN("No path found to target (%d, %d)", targetPos.x, targetPos.y);
            return NULL;
        }
        // LOG_INF("Current Node (%d, %d) fCost: %d", openList[lowestIndex].position.x,
        //         openList[lowestIndex].position.y, openList[lowestIndex].fCost);
        currentNode = &nodeArray[lowestIndex];
        // Check if reached target
        if (currentNode->position.x == targetPos.x && currentNode->position.y == targetPos.y)
        {
            LOG_INF("Reached target at (%d, %d)", currentNode->position.x, currentNode->position.y);
            return currentNode;
        }
        if (nodeListCount >= 256)
        {
            LOG_ERR("Closed list overflow in A* pathfinding");
            return NULL;
        }
        // Move current node from open to closed list
        currentNode->closed = true;
        if (currentNode->valid == false)
        {
            continue;
        }
        // Check neighbors
        Vector2Int neighbors[4] = {
            { currentNode->position.x + 1, currentNode->position.y },
            { currentNode->position.x - 1, currentNode->position.y },
            { currentNode->position.x, currentNode->position.y + 1 },
            { currentNode->position.x, currentNode->position.y - 1 },
        };
        for (uint8_t i = 0; i < 4; i++)
        {
            Vector2Int neighborPos  = neighbors[i];
            bool       skipNeighbor = false;
            for (uint16_t j = 0; j < nodeListCount; j++)
            {
                if (nodeArray[j].position.x == neighborPos.x && nodeArray[j].position.y == neighborPos.y)
                {
                    skipNeighbor = true;
                    break;
                }
            }
            if (skipNeighbor)
            {
                continue;
            }
            nodeArray[nodeListCount].closed   = false;
            nodeArray[nodeListCount].valid    = hFunc(neighborPos, targetPos, nodeArray[nodeListCount].hCost);
            nodeArray[nodeListCount].position = neighborPos;
            nodeArray[nodeListCount].gCost    = currentNode->gCost + 1;
            nodeArray[nodeListCount].fCost    = nodeArray[nodeListCount].gCost + nodeArray[nodeListCount].hCost;
            nodeArray[nodeListCount].parent   = currentNode;
            nodeArray[nodeListCount].closed   = false;
            nodeListCount++;
            if (nodeListCount >= 256)
            {
                LOG_ERR("Open list overflow in A* pathfinding when adding neighbor");
                return NULL;
            }
        }
    }
    return NULL;
}

Vector2Int8 AStar_GetMoveDirection(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea,
                                   HeuristicFuncPtr hFunc)
{
    LOG_INF("A* Pathfinding from (%d, %d) to (%d, %d)", startPos.x, startPos.y, targetPos.x, targetPos.y);
    AStar_Node  nodeList[maxSearchArea];
    AStar_Node* lastNode = NULL;

    lastNode = AStar_CalucalatePath(nodeList, maxSearchArea, startPos, targetPos, hFunc);
    if (lastNode == NULL)
    {
        return { 0, 0 };
    }

    for (int i = 0; i < maxSearchArea; i++)
    {
        // #if 1
        //         Vector3Int pos3D    = { lastNode->position.x, lastNode->position.y, 0 };
        //         Vector2    worldPos = Utils_GridToWorld(pos3D, TEXTURE_SIZE * TEXTURE_SCALE);
        //         DrawRectangleLines(int(worldPos.x), int(worldPos.y), int(TEXTURE_SIZE * TEXTURE_SCALE),
        //                            int(TEXTURE_SIZE * TEXTURE_SCALE), GREEN);
        // #endif
        if (lastNode->parent == NULL
            || (lastNode->parent->position.x == startPos.x && lastNode->parent->position.y == startPos.y))
        {
            break;
        }
        lastNode = lastNode->parent;
    }
    Vector2Int8 direction;
    direction.x = int8_t(lastNode->position.x - startPos.x);
    direction.y = int8_t(lastNode->position.y - startPos.y);
    return direction;
}

bool AStar_IsPathAvailable(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea,
                           HeuristicFuncPtr hFunc)
{
    LOG_INF("A* Pathfinding from (%d, %d) to (%d, %d)", startPos.x, startPos.y, targetPos.x, targetPos.y);
    AStar_Node  nodeList[maxSearchArea];
    AStar_Node* currentNode = NULL;

    return AStar_CalucalatePath(nodeList, maxSearchArea, startPos, targetPos, hFunc) != NULL;
}

uint16_t AStar_GetPath(const Vector2Int startPos, const Vector2Int targetPos, uint16_t maxSearchArea,
                       Vector2Int* outPathBuffer, size_t outPathBufferSize, HeuristicFuncPtr hFunc)
{

    LOG_INF("A* Pathfinding from (%d, %d) to (%d, %d)", startPos.x, startPos.y, targetPos.x, targetPos.y);
    AStar_Node  nodeList[maxSearchArea];
    AStar_Node* lastNode = NULL;

    lastNode = AStar_CalucalatePath(nodeList, maxSearchArea, startPos, targetPos, hFunc);
    if (lastNode == NULL)
    {
        return 0;
    }
    // Reruct path
    uint16_t pathLength = 0;
    for (int i = 0; i < maxSearchArea; i++)
    {
        if (pathLength >= outPathBufferSize)
        {
            LOG_WRN("Output path buffer too small, truncating path");
            return 0;
        }
        outPathBuffer[pathLength] = lastNode->position;
        pathLength++;
        if (lastNode->parent == NULL
            || (lastNode->parent->position.x == startPos.x && lastNode->parent->position.y == startPos.y))
        {
            return pathLength;
        }
        lastNode = lastNode->parent;
    }
    return 0;
}

#define MOUSE_BUTTON_COUNT    5
#define KEYBOARD_BUTTON_COUNT 128
Updatable debugUpdatable = { Debug_ShowDebugWindow };
bool      debugVisible   = false;

static void Debug_ShowMisc()
{
    char buffer[12];
    if (ImGui::CollapsingHeader("Objects"))
    {
        if (ImGui::TreeNode("Entities"))
        {
            ImGui::Text("Entity count: %d", Entitiy2D_GetCount());
            Entity2D* current = Entitiy2D_GetEntityList();
            for (uint32_t i = 0; i < Entitiy2D_GetCount(); i++)
            {
                // char buffer[12];
                sprintf(buffer, "Entity %d", i);
                if (ImGui::CollapsingHeader(buffer))
                {
                    ImGui::Text("Entity position: {%f, %f }", current->position.x, current->position.y);
                    ImGui::Text("Entity scale: %f", current->scale);
                    ImGui::Text("Entity rotation: %f", current->rotation);
                    ImGui::Text("Entity id: %d", current->id);
                }
                current = current->next;
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Sprites"))
        {
            ImGui::Text("Sprites count: %d", Sprite_GetCount());
            for (uint32_t i = 0; i < Sprite_GetCount(); i++)
            {
                Sprite* current = Sprite_GetSpriteList() + i;
                sprintf(buffer, "Sprite %d", i);
                if (ImGui::CollapsingHeader(buffer))
                {
                    if (ImGui::CollapsingHeader("Sprite parent"))
                    {
                        if (current->parent != NULL)
                        {
                            ImGui::Text("Entity position: {%f, %f }", current->parent->position.x,
                                        current->parent->position.y);
                            ImGui::Text("Entity scale: %f", current->parent->scale);
                            ImGui::Text("Entity rotation: %f", current->parent->rotation);
                            ImGui::Text("Entity id: %d", current->parent->id);
                        }
                    }
                    ImGui::Text("Sprite position: {%f, %f }", current->position.x, current->position.y);
                    ImGui::Text("Sprite scale: %f", current->scale);
                    ImGui::Text("Sprite rotation: %f", current->rotation);
                    // ImGui::Text("Sprite id: %d", current->id);
                    ImGui::Image((ImTextureID)current->currentTexture->id, { 128, 128 });
                    ImGui::Text("Sprite z order: %d", current->zOrder);
                    ImGui::Text("Sprite is visible: %d", current->isVisible);
                }
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Colliders"))
        {
            ImGui::Text("Colliders count: %d", Collider2D_GetCount());
            Collider2D* current = Collider2D_GetCollider2DList();
            for (uint32_t i = 0; i < Collider2D_GetCount(); i++)
            {
                sprintf(buffer, "Colliders %d", i);
                if (ImGui::CollapsingHeader(buffer))
                {
                    ImGui::Text("Collider parent: %p", current->parent);
                    ImGui::Text("Collider position: {%f, %f }", current->position.x, current->position.y);
                    ImGui::Text("Collider size: {%f, %f }", current->size.x, current->size.y);
                    ImGui::Text("Collider is enabled: %d", current->isEnabled);
                    ImGui::Text("Collider id: %d", current->id);
                    ImGui::Text("Collider current collision count: %d", current->collision.collisionCount);
                }
                current = current->next;
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Textures"))
        {
            ImGui::Text("Textures count: %d", Texture_GetCount());
            for (uint32_t i = 0; i < Texture_GetCount(); i++)
            {
                sprintf(buffer, "Textures %d", i);
                if (ImGui::CollapsingHeader(buffer))
                {
                    ImGui::Text("Texture id: %d", Texture_GetTextures()[i].texture.id);
                    ImGui::Text("Texture name: %s", Texture_GetTextures()[i].textureName);
                    ImGui::Image((ImTextureID)Texture_GetTextures()[i].texture.id, { 128, 128 });
                }
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("AnimatedSprites"))
        {
            ImGui::Text("AnimatedSprites count: %d", AnimatedSprite_GetCount());
            AnimatedSprite* current = AnimatedSprite_GetAnimatedSpriteList();
            for (uint32_t i = 0; i < AnimatedSprite_GetCount(); i++)
            {
                sprintf(buffer, "AnimatedSprite %d", i);
                if (ImGui::CollapsingHeader(buffer))
                {
                    // ImGui::Text("AnimatedSprite Sprite id: %d", current->sprite.id);
                    ImGui::Text("AnimatedSprite AnimationData: %p", current->currentAnimation);
                    ImGui::Text("AnimatedSprite id: %d", current->id);
                    ImGui::Text("AnimatedSprite frameTime: %d", current->frameTime);
                    ImGui::Text("AnimatedSprite isPlaying: %d", current->isPlaying);
                    ImGui::Text("AnimatedSprite repeat: %d", current->repeat);
                    ImGui::Text("AnimatedSprite currentFrame: %d", current->currentFrame);
                    // ImGui::Image((ImTextureID)&Texture_GetTextures()[i].texture, {128, 128});
                }
                current = current->next;
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Audio"))
        {
            ImGui::Text("Audio count: %d", Audio_GetCount());
            for (uint32_t i = 0; i < Audio_GetCount(); i++)
            {
                sprintf(buffer, "Audio %d", i);
                if (ImGui::CollapsingHeader(buffer))
                {
                    ImGui::Text("Audio Id: %d", Audio_GetAudios()[i].id);
                    ImGui::Text("Audio Name: %s", Audio_GetAudios()[i].soundName);
                }
            }
            ImGui::TreePop();
        }
    }
    if (ImGui::CollapsingHeader("Inputs, Navigation & Focus"))
    {
        if (ImGui::TreeNode("Keyboard, Mouse & Navigation State"))
        {
            ImGui::Text("Mouse pos: (%d, %d)", Input_GetMouseX(), Input_GetMouseY());
            ImGui::Text("Mouse delta: (%d, %d)", Input_GetMouseDeltaX(), Input_GetMouseDeltaY());
            ImGui::Text("Mouse down:");
            for (uint32_t i = 0; i < MOUSE_BUTTON_COUNT; i++)
            {
                if (Input_IsMouseButtonDown(i))
                {
                    ImGui::SameLine();
                    ImGui::Text("b%d", i);
                }
            }
            // ImGui::Text("Mouse up:");
            // for(uint32_t i = 0; i < MOUSE_BUTTON_COUNT; i++)
            // {
            //     if(Input_IsMouseButtonUp(i))
            //     {
            //         ImGui::SameLine();
            //         ImGui::Text("b%d", i);
            //     }
            // }
            ImGui::Text("Mouse up:");
            for (uint32_t i = 0; i < MOUSE_BUTTON_COUNT; i++)
            {
                if (Input_IsMouseButtonPressed(i))
                {
                    ImGui::SameLine();
                    ImGui::Text("b%d", i);
                }
            }
            ImGui::Text("Mouse pressed:");
            for (uint32_t i = 0; i < MOUSE_BUTTON_COUNT; i++)
            {
                if (Input_IsMouseButtonPressed(i))
                {
                    ImGui::SameLine();
                    ImGui::Text("b%d", i);
                }
            }
            ImGui::Text("Mouse released:");
            for (uint32_t i = 0; i < MOUSE_BUTTON_COUNT; i++)
            {
                if (Input_IsMouseButtonReleased(i))
                {
                    ImGui::SameLine();
                    ImGui::Text("b%d", i);
                }
            }
            // ImGui::Text("Keys up:");
            // for(uint32_t i = 0; i < KEYBOARD_BUTTON_COUNT; i++)
            // {
            //     if(Input_IsKeyUp(i))
            //     {
            //         ImGui::SameLine();
            //         ImGui::Text("b%d", i);
            //     }
            // }
            ImGui::Text("Keys down:");
            for (uint32_t i = 0; i < KEYBOARD_BUTTON_COUNT; i++)
            {
                if (Input_IsKeyDown(i))
                {
                    ImGui::SameLine();
                    ImGui::Text("b%d", i);
                }
            }
            ImGui::Text("Keys pressed:");
            for (uint32_t i = 0; i < KEYBOARD_BUTTON_COUNT; i++)
            {
                if (Input_IsKeyPressed(i))
                {
                    ImGui::SameLine();
                    ImGui::Text("b%d", i);
                }
            }
            ImGui::Text("Keys release:");
            for (uint32_t i = 0; i < KEYBOARD_BUTTON_COUNT; i++)
            {
                if (Input_IsKeyReleased(i))
                {
                    ImGui::SameLine();
                    ImGui::Text("b%d", i);
                }
            }
            ImGui::TreePop();
        }
    }
    if (ImGui::CollapsingHeader("Delta time"))
    {
        ImGui::Text("Delta time: %f", DeltaTime_GetDeltaTime());
    }
}

void Debug_ShowDebugWindow()
{
    if (Input_IsKeyPressed(INPUT_KEYCODE_F3))
    {
        debugVisible = !debugVisible;
    }
    if (!debugVisible)
    {
        return;
    }
    // We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We
    // only do it to make the Demo applications a little more welcoming.
    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_Always);

    ImGuiWindowFlags window_flags = 0;

    // Main body of the Demo window starts here.
    if (!ImGui::Begin("Debug View", nullptr, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    // Most "big" widgets share a common width settings by default.
    // ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);    // Use 2/3 of the space for widgets and 1/3 for labels
    // (default)
    ImGui::PushItemWidth(ImGui::GetFontSize()
                         * -12);  // Use fixed width for labels (by passing a negative value), the rest goes to widgets.
                                  // We choose a width proportional to our font size.

    ImGui::Spacing();

    Debug_ShowMisc();

    // End of ShowDemoWindow()
    ImGui::End();
}

Updatable* Debug_GetUpdatable()
{
    return &debugUpdatable;
}

Updatable deltaTimeUpdatable = { DeltaTime_Update };
long      lastClock          = 0;
long      deltaClock         = 0;
float     deltaTime          = 0.0f;

void DeltaTime_Update()
{
    long currentClock = clock();
    deltaClock        = currentClock - lastClock;
    if (lastClock == 0)
    {
        deltaClock = 0;
    }
    lastClock = currentClock;
    deltaTime = (float)deltaClock / (float)CLOCKS_PER_SEC;
}

float DeltaTime_GetDeltaTime()
{
    return deltaTime;
}

Updatable* DeltaTime_GetUpdatable()
{
    return &deltaTimeUpdatable;
}

void Logger_Init()
{
    SetTraceLogLevel(LOG_ALL);
}

void Logger_Deinit()
{
    SetTraceLogLevel(LOG_NONE);
}

void Stopwatch_Start(Stopwatch* stopwatch, uint32_t milis)
{
    stopwatch->startTime = clock();
    stopwatch->endTime   = stopwatch->startTime + (milis * CLOCKS_PER_MS);
}

void Stopwatch_Stop(Stopwatch* stopwatch)
{
    stopwatch->endTime = 0;
}

uint32_t Stopwatch_GetElapsedTime(Stopwatch* stopwatch)
{
    return (clock() - stopwatch->startTime) * CLOCKS_PER_MS;
}

float Stopwatch_GetPercentElapsedTime(Stopwatch* stopwatch)
{
    if (!Stopwatch_IsRunning(stopwatch))
    {
        return 1.0f;
    }
    uint32_t totalTime   = stopwatch->endTime - stopwatch->startTime;
    uint32_t elapsedTime = Stopwatch_GetElapsedTime(stopwatch);
    return (float)elapsedTime / (float)totalTime;
}

uint32_t Stopwatch_GetRemainingTime(Stopwatch* stopwatch)
{
    return (stopwatch->endTime - clock()) * CLOCKS_PER_MS;
}
float Stopwatch_GetPercentRemainingTime(Stopwatch* stopwatch)
{
    if (!Stopwatch_IsRunning(stopwatch))
    {
        return 0.0f;
    }
    uint32_t totalTime     = stopwatch->endTime - stopwatch->startTime;
    uint32_t remainingTime = Stopwatch_GetRemainingTime(stopwatch);
    return (float)remainingTime / (float)totalTime;
}

bool Stopwatch_IsRunning(Stopwatch* stopwatch)
{
    return stopwatch->endTime != 0;
}

bool Stopwatch_IsElapsed(Stopwatch* stopwatch)
{
    return Stopwatch_IsRunning(stopwatch) ? (uint32_t)(clock()) >= stopwatch->endTime : false;
}

bool Stopwatch_IsZero(Stopwatch* stopwatch)
{
    return !Stopwatch_IsRunning(stopwatch) || Stopwatch_IsElapsed(stopwatch);
}

uint32_t Utils_AbsInt32(int32_t value)
{
    return (value < 0) ? -value : value;
}

uint16_t Utils_AbsInt16(int16_t value)
{
    return (value < 0) ? -value : value;
}

float Utils_AbsFloat(float value)
{
    return (value < 0) ? -value : value;
}

Vector2 Utils_WorldToScreen2D(Vector2 position, Camera2D camera)
{
    float   zoom = camera.zoom;
    Vector2 screenPosition;
    screenPosition.x = (position.x - camera.target.x) * zoom + GetScreenWidth() / 2.0f;
    screenPosition.y = (position.y - camera.target.y) * zoom + GetScreenHeight() / 2.0f;
    return screenPosition;
}

Vector2 Utils_ScreenToWorld2D(Vector2 position, Camera2D camera)
{
    Vector2 worldPosition = GetScreenToWorld2D(position, camera);
    return worldPosition;
}

Vector2 Utils_ScaleWithCamera(Vector2 value, Camera2D camera)
{
    Vector2 scaledValue;
    scaledValue.x = value.x * (1.0f / camera.zoom);
    scaledValue.y = value.y * (1.0f / camera.zoom);
    return scaledValue;
}

Vector3Int Utils_WorldToGrid(Vector2 pos, uint8_t gridSize)
{
    Vector3Int position;
    position.x = (int)(pos.x / gridSize);
    position.y = (int)(pos.y / gridSize);
    position.z = 0;
    if (pos.x < 0)
    {
        position.x -= 1;
    }
    if (pos.y < 0)
    {
        position.y -= 1;
    }
    return position;
}

Vector3Int8 Utils_WorldToChunk(Vector2 pos, uint8_t gridSize, uint8_t chunkSize)
{
    Vector3Int8 position;
    position.x = (pos.x / gridSize) / chunkSize;
    position.y = (pos.y / gridSize) / chunkSize;
    if (pos.x < 0)
    {
        position.x -= 1;
    }
    if (pos.y < 0)
    {
        position.y -= 1;
    }
    position.z = 0;
    return position;
}

Vector2 Utils_GridToWorld(Vector3Int pos, uint8_t gridSize)
{
    Vector2 position;
    position.x = pos.x * gridSize;
    position.y = pos.y * gridSize;
    return position;
}

Vector2 Utils_GridCenterToWorld(Vector3Int pos, uint8_t gridSize)
{
    Vector2 position;
    position.x = pos.x * gridSize + (gridSize / 2.0);
    position.y = pos.y * gridSize + (gridSize / 2.0);
    return position;
}

Vector3Int8 Utils_GridToChunk(Vector3Int pos, uint8_t chunkSize)
{
    Vector3Int8 chunkPos;
    chunkPos.x = pos.x / chunkSize;
    chunkPos.y = pos.y / chunkSize;
    if (pos.x < 0)
    {
        chunkPos.x -= 1;
    }
    if (pos.y < 0)
    {
        chunkPos.y -= 1;
    }
    chunkPos.z = pos.z;
    return chunkPos;
}

bool Utils_IsInGridRadius(Vector2Int center, Vector2Int point, uint16_t radius)
{
    int32_t dx = center.x - point.x;
    int32_t dy = center.y - point.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

float Utils_Vector2Distance(Vector2 a, Vector2 b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}
uint16_t Utils_Vector2DistanceInt(Vector2Int a, Vector2Int b)
{
    int32_t dx = a.x - b.x;
    int32_t dy = a.y - b.y;
    return (uint16_t)sqrtf((float)(dx * dx + dy * dy));
}

uint16_t Utils_ManhattanDistance(Vector2Int a, Vector2Int b)
{
    return Utils_AbsInt16(a.x - b.x) + Utils_AbsInt16(a.y - b.y);
}

int16_t Utils_GetRandomInRange(int16_t min, int16_t max)
{
    return (rand() % (max - min + 1)) + min;
}

bool Utils_PointInRectangle(Vector2 point, Rectangle rect)
{
    return (point.x >= rect.x && point.x <= rect.x + rect.width && point.y >= rect.y
            && point.y <= rect.y + rect.height);
}

bool Utils_RectangleOverlap(Rectangle a, Rectangle b)
{
    return (a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y);
}
