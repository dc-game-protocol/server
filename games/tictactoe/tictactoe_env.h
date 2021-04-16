#ifndef TIC_TAC_TOE_TICTACTOE_ENV_H
#define TIC_TAC_TOE_TICTACTOE_ENV_H

#include <dc_fsm/fsm.h>
#include "../../server/game_env.h"
#include "../../server/client.h"

#define TITLESIZE 100
#define NUMPLAYERS 2

typedef enum
{
    ACCEPTING_CONNECTIONS = FSM_APP_STATE_START,
    CONNECTED_CLIENTS,
    X_TURN,
    X_FAIL,
    X_SUCCESS,
    X_WIN,
    O_TURN,
    O_FAIL,
    O_SUCCESS,
    O_WIN,
    TIE,
    COMPLETE
} TICTACTOE_STATE;

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
