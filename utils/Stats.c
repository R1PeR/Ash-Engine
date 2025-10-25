#include "Stats.h"

uint16_t Stats_MovementDelayFromSpeed(uint16_t speed)
{
    if (speed == 0)
    {
        return 1000;  // Prevent division by zero, set to max delay
    }
    return 1000 - speed;
}
