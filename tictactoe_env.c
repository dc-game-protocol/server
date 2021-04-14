#include <stdlib.h>
#include <string.h>
#include "tictactoe_env.h"

TicTacToeEnv* ticTacToeEnv_create(ServerEnv* server){
    TicTacToeEnv* this = (TicTacToeEnv*)malloc(sizeof(TicTacToeEnv));
    memset(this, 0, sizeof(TicTacToeEnv));
    memset(this->board, -1, sizeof(this->board));

    this->common.name = (char*)malloc(TITLESIZE*sizeof(char));
    this->common.from_state_id = FSM_INIT;
    this->common.current_state_id  = ACCEPTING_CONNECTIONS;

    this->server = server;

    return this;
}

void ticTacToeEnv_destroy(TicTacToeEnv* env){
    FD_CLR(env->cfd_o, &(env->server->rfds_master));
    FD_CLR(env->cfd_x, &(env->server->rfds_master));
    free(env->common.name);
    free(env);
}