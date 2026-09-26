
#ifndef MINESWEEPER_MODE_H
#define MINESWEEPER_MODE_H
#include "ashes/ash_context.h"

extern Mode minesweeperMode;

void MinesweeperMode_OnStart();
void MinesweeperMode_OnPause();
void MinesweeperMode_Update();
void MinesweeperMode_Draw();
void MinesweeperMode_OnStop();
void MinesweeperMode_OnResume();

#endif
