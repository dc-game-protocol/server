#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tictactoe_env.h"

const int ttt_num_players = 2;

TicTacToeEnv* ticTacToeEnv_create(){
    TicTacToeEnv* this = (TicTacToeEnv*)malloc(sizeof(TicTacToeEnv));
    memset(this, 0, sizeof(TicTacToeEnv));
    memset(this->board, -1, sizeof(this->board));

    this->common.game_id = TTT;
    this->common.protocol_ver = 1;
    this->common.num_players = ttt_num_players;
    this->common.clients = (Client **) calloc(ttt_num_players, sizeof(Client*));

    this->common.common.name= (char*)malloc(TITLESIZE*sizeof(char));
    sprintf(this->common.common.name, "TTT");
    this->common.common.from_state_id = FSM_INIT;
    this->common.common.current_state_id  = START_GAME;

    return this;
}