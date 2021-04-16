
#include <stdlib.h>
#include "game_env.h"
#include "../games/tictactoe/tictactoe_env.h"

GameEnv* gameEnv_create(int game_id){
    if(game_id == TTT){
//        return ticTacToeEnv_create();
    }else if(game_id == RPS){
        //rps env here
    }
    return NULL;
}

void gameEnv_endGame(GameEnv* gameEnv, ServerEnv* serverEnv, int quit_fd){
    for(int i = 0; i < gameEnv->num_players; i++){
        if(gameEnv->clients[i]->fd != quit_fd){
            //TODO: send end message to each client
        }
    }
    gameEnv_destroy(gameEnv, serverEnv);
}

void gameEnv_destroy(GameEnv* gameEnv, ServerEnv* serverEnv){
    //delete clients
    for(int i = 0; i < gameEnv->num_players; i++){
        FD_CLR(gameEnv->clients[i]->fd, &serverEnv->rfds_master);
        serverEnv->clients[gameEnv->clients[i]->fd] = NULL;
        client_destroy(gameEnv->clients[i]);
    }
    //delete game specific attributes
    if(gameEnv->game_id == TTT){
//        ticTacToeEnv_destroy(gameEnv);
    }else if(gameEnv->game_id  == RPS){
        //rps env here
    }

    free(gameEnv->clients);
    free(gameEnv->common.name);
    free(gameEnv);
}