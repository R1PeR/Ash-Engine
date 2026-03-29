#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"
#include "modes/game2/MainMode.h"

#include <cstdint>

int main()
{
    const uint32_t screenWidth  = 1280;
    const uint32_t screenHeight = 720;
    Window_Init(screenWidth, screenHeight, "DontYouDareGoHollow");
    Logger_Init();
    Audio_Init();

    Context_AddUpdatable(Debug_GetUpdatable());
    Context_AddUpdatable(Sprite_GetUpdatable());
    Context_AddUpdatable(Collider2D_GetUpdatable());
    Context_AddUpdatable(DeltaTime_GetUpdatable());
    Context_AddUpdatable(AnimatedSprite_GetUpdatable());

    Context_SetMode(&mainMode);

    Logger_Deinit();
    Audio_Deinit();

    return 0;
}
