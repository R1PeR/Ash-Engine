#include "engine/context/Context.h"
#include "engine/io/Window.h"
#include "engine/misc/Debug.h"
#include "engine/misc/DeltaTime.h"
#include "engine/misc/Logger.h"
#include "engine/components/Sprite.h"
#include "engine/components/Collider2D.h"
#include "engine/components/AnimatedSprite.h"
#include "modes/MainMode.h"

int main() 
{
    const int screenWidth = 1280;
    const int screenHeight = 720;
    Window_Init(screenWidth, screenHeight, "DontYouDareGoHollow");
    Logger_Init();
    
    Context_AddUpdatable(Debug_GetUpdatable());
    Context_AddUpdatable(Sprite_GetUpdatable());
    Context_AddUpdatable(Collider2D_GetUpdatable());
    Context_AddUpdatable(DeltaTime_GetUpdatable());
    Context_AddUpdatable(AnimatedSprite_GetUpdatable());

    Context_SetMode(&mainMode);
    
    CloseWindow();
    return 0;
}
