#include "Stats.h"

uint16_t Stats_MovementDelay(uint16_t speed)
{
    if (speed == 0)
    {
        return 1000;  // Prevent division by zero, set to max delay
    }
    return 1000 - speed;
}

uint16_t Stats_AttackDelay(uint16_t attackSpeed, uint16_t dexterity)
{
    uint16_t dexterityFactor = dexterity / 10;  // Each 10 points in dexterity reduces delay by 5%
    float    reduction       = 1.0f - (dexterityFactor * 0.05f);
    if (reduction < 0.5f)
    {
        reduction = 0.5f;  // Cap reduction to a minimum of 50% of the original attack speed
    }
    return (uint16_t)(attackSpeed * reduction);
}
