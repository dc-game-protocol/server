#ifndef TIC_TAC_TOE_TICTACTOE_ENV_H
#define TIC_TAC_TOE_TICTACTOE_ENV_H

#include <dc_fsm/fsm.h>
#include "../../server/game_env.h"
#include "../../server/client.h"

typedef struct  {
    GameEnv common;
    int board[9];
    int turn; //id of player (or index in array?)
    int prev_player; //id of player (or index in array?)
    int prev_pos;
}TicTacToeEnv;


TicTacToeEnv* ticTacToeEnv_create();
void ticTacToeEnv_destroy(TicTacToeEnv* env);

#endif //TIC_TAC_TOE_TICTACTOE_ENV_H
