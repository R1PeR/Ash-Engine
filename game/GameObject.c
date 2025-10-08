#include "GameObject.h"
#include "raylib.h"
#include "utils/SpriteDefines.h"

void GameObject_Initialize(GameObject * gameObject)
{
    gameObject->color = WHITE;
    gameObject->position = {0, 0};
    gameObject->tag = GameObjectTag::GameObjectNone;
    gameObject->textureId = FONT_BLANK_SPACE;
}
