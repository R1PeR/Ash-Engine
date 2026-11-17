#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_memory.h"
#include "modes/game2/MenuMode.h"

#include <cstdint>

int main()
{
    const uint32_t screenWidth  = 1280;
    const uint32_t screenHeight = 720;

    Arena globalArena;
    Arena debugArena;
    Arena_Init(&globalArena, MB(64));
    Arena_Init(&debugArena, MB(16));

    globalArena.debug = &debugArena;

    Window*   window   = Arena_Push(Window, &globalArena);
    Context*  context  = Arena_Push(Context, &globalArena);
    MenuMode* menuMode = Arena_Push(MenuMode, &globalArena);

    context->window  = window;
    menuMode->window = window;
    menuMode->arena  = &globalArena;

    Window_Init(window, screenWidth, screenHeight, "Test");
    Logger_Init();
    Audio_Init();

    Context_Init(context, 8, 8, &globalArena);
    Context_SetMode(context, MODE_FROM_CLASSNAME(MenuMode), menuMode);

    Logger_Deinit();
    Audio_Deinit();

    Arena_Free(&globalArena);
    Arena_Free(&debugArena);

    return 0;
}
