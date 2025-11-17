#ifndef UTILS_STATS_H
#define UTILS_STATS_H
#include <stdint.h>

uint16_t Stats_MovementDelay(uint16_t speed);
uint16_t Stats_AttackDelay(uint16_t attackSpeed, uint16_t dexterity);

#endif  // UTILS_STATS_H
