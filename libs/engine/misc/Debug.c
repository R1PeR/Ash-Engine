#include "Debug.h"

#include "engine/components/AnimatedSprite.h"
#include "engine/components/Audio.h"
#include "engine/components/Collider2D.h"
#include "engine/components/Entity2D.h"
#include "engine/components/Sprite.h"
#include "engine/components/Texture.h"
#include "engine/io/Input.h"
#include "engine/misc/DeltaTime.h"
#include "engine/misc/Logger.h"
#include "imgui.h"

#include <stdbool.h>
#include <stdio.h>
#define MOUSE_BUTTON_COUNT    5
#define KEYBOARD_BUTTON_COUNT 128
Updatable debugUpdatable = { Debug_ShowDebugWindow };

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
            Sprite* current = Sprite_GetSpriteList();
            for (uint32_t i = 0; i < Sprite_GetCount(); i++)
            {
                sprintf(buffer, "Sprite %d", i);
                if (ImGui::CollapsingHeader(buffer))
                {
                    if (ImGui::CollapsingHeader("Sprite parent"))
                    {
                        ImGui::Text("Entity position: {%f, %f }", current->parent->position.x,
                                    current->parent->position.y);
                        ImGui::Text("Entity scale: %f", current->parent->scale);
                        ImGui::Text("Entity rotation: %f", current->parent->rotation);
                        ImGui::Text("Entity id: %d", current->parent->id);
                    }
                    ImGui::Text("Sprite position: {%f, %f }", current->position.x, current->position.y);
                    ImGui::Text("Sprite scale: %f", current->scale);
                    ImGui::Text("Sprite rotation: %f", current->rotation);
                    ImGui::Text("Sprite id: %d", current->id);
                    ImGui::Image((ImTextureID)current->currentTexture, { 128, 128 });
                    ImGui::Text("Sprite z order: %d", current->zOrder);
                    ImGui::Text("Sprite is visible: %d", current->isVisible);
                }
                current = current->next;
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
                    ImGui::Image((ImTextureID)&Texture_GetTextures()[i].texture, { 128, 128 });
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
                    ImGui::Text("AnimatedSprite Sprite id: %d", current->sprite.id);
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
    // We specify a default position/size in case there's no data in the .ini file. Typically this isn't required! We
    // only do it to make the Demo applications a little more welcoming.
    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

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

    // All demo contents
    Debug_ShowMisc();

    // End of ShowDemoWindow()
    ImGui::End();
}

Updatable* Debug_GetUpdatable()
{
    return &debugUpdatable;
}
