#include "Player.h"

#include "game/GameObject.h"

#include <stdint.h>


void Player_Initalize(Player* player)
{
    GameObject_Initialize(&player->gameObject);
    player->health               = 100;
    player->stamina              = 100;
    player->gameObject.textureId = 1;
}

void Player_Update(Player* player)
{
    if (player->state == PlayerState::PlayerIdle)
    {
        // just idle, doing nothing
    }
    else if (player->state == PlayerState::PlayerMoving)
    {
        Stopwatch_Start(&player->timer, 100);
        player->state = PlayerState::PlayerWaiting;
    }
    else if (player->state == PlayerState::PlayerWaiting)
    {
        if (Stopwatch_IsZero(&player->timer))
        {
            player->state = PlayerState::PlayerIdle;
        }
    }
}

void Player_Move(Player* player, int8_t x, int8_t y)
{
    if (player->state == PlayerState::PlayerIdle)
    {
        player->gameObject.position.x += x;
        player->gameObject.position.y += y;
        player->state = PlayerState::PlayerMoving;
    }
}
