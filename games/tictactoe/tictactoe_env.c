#include <stdlib.h>
#include <string.h>
#include "tictactoe_env.h"

const int ttt_numplayers = 2;

TicTacToeEnv* ticTacToeEnv_create(){
    TicTacToeEnv* this = (TicTacToeEnv*)malloc(sizeof(TicTacToeEnv));
    memset(this, 0, sizeof(TicTacToeEnv));
    memset(this->board, -1, sizeof(this->board));

    this->common.game_id = TTT;
    this->common.protocol_ver = 1;
    this->common.num_players = ttt_numplayers;
    this->common.clients = (Client **) calloc(ttt_numplayers, sizeof(Client*));

    this->common.common.name= (char*)malloc(TITLESIZE*sizeof(char));
    this->common.common.from_state_id = FSM_INIT;
    this->common.common.current_state_id  = ACCEPTING_CONNECTIONS;

    return this;
}

void ticTacToeEnv_destroy(TicTacToeEnv* env){

}