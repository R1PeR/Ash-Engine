#ifndef GAME_OBJECT_PLAYER
#define GAME_OBJECT_PLAYER
#include "game/GameObject.h"
#include "libs/engine/misc/Stopwatch.h"

enum PlayerState
{
    PlayerIdle = 0,
    PlayerWaiting,
    PlayerMoving,
};

typedef struct Player
{
    GameObject  gameObject;
    uint8_t     health;
    uint8_t     stamina;
    PlayerState state;
    Stopwatch   timer;
} Player;

void Player_Initalize(Player* player);
void Player_Update(Player* player);
void Player_Move(Player* player, int8_t x, int8_t y);

#endif
