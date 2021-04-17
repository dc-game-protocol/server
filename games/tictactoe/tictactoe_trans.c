#include <string.h>
#include <stdio.h>
#include <dc/sys/socket.h>
#include <dc/unistd.h>
#include <regex.h>
#include <stdlib.h>
#include "tictactoe_trans.h"
#include "../../server/helper.h"
#include "../../TCP/response.h"
#include "../../TCP/response_status.h"
#include "../../server/helper.h"

struct state_transition ttt_transitions[] =
        {
                {FSM_INIT,              START_GAME,            ttt_start},
                {START_GAME,            FSM_EXIT,              ttt_validate},
                {FSM_EXIT,              FSM_EXIT,              ttt_validate},
                {FSM_EXIT,              COMPLETE,              ttt_complete},
                {COMPLETE,              FSM_EXIT,              NULL},
                {FSM_IGNORE,            FSM_IGNORE,            NULL}
        };

int ttt_start(struct dc_fsm_environment *env) {
    TicTacToeEnv *t_env = (TicTacToeEnv *) env;
    printf("%s: Starting Game\n", t_env->common.common.name);
    t_env->turn = t_env->common.clients[0]->uid;

    uint8_t buffer[1];

    buffer[0] = 1;
    write_response(t_env->common.clients[0]->fd, RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_START, 1, buffer);

    buffer[0] = 2;
    write_response(t_env->common.clients[1]->fd, RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_START, 1, buffer);

    return FSM_EXIT;
}

int ttt_validate(struct dc_fsm_environment *env){
    TicTacToeEnv *t_env = (TicTacToeEnv *) env;
    Request req = *t_env->common.buffer;

    if(req.uid != t_env->turn){
        write_response(req.uid, RESPONSE_GAME_ERROR_TURN, req.type, 0, NULL);
        return FSM_EXIT;
    }else if(req.context != 1){
        write_response(req.uid, RESPONSE_ERROR_CONTEXT, req.type, 0, NULL);
        return FSM_EXIT;
    }else if(req.len_payload != 1){
        write_response(req.uid, RESPONSE_ERROR_PAYLOAD, req.type, 0, NULL);
        return FSM_EXIT;
    }else if(!(req.payload[0]>=0 && req.payload[0]<=8)){
        write_response(req.uid, RESPONSE_GAME_ERROR_INVALID, req.type, 0, NULL);
        return FSM_EXIT;
    }else if(t_env->board[req.payload[0]] != -1){
        write_response(req.uid, RESPONSE_ERROR_PAYLOAD, req.type, 0, NULL);
        return FSM_EXIT;
    }


    t_env->board[req.payload[0]] = t_env->turn;
    write_response(req.uid, RESPONSE_SUCCESS_SUCCESS, req.type, 0, NULL);

    if(has_won_game(t_env->turn, t_env->board)){
        uint8_t buffer[2];
        buffer[1] = req.payload[0];

        for(int i = 0; i < t_env->common.num_players; i++) {
            buffer[0] = (t_env->common.clients[i]->uid == t_env->turn)? 1 : 2;
            write_response(t_env->common.clients[i]->uid , RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_END, 2, buffer);
        }
        return COMPLETE;
    }else if(is_game_tied(t_env->board)){
        uint8_t buffer[2];
        buffer[0] = 3;
        buffer[1] = req.payload[0];

        for(int i = 0; i < t_env->common.num_players; i++){
            write_response(t_env->common.clients[i]->uid,  RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_END, 2, buffer);
        }
        return COMPLETE;
    }else{
        uint8_t buffer[1];
        buffer[0] = req.payload[0];
        for(int i = 0; i < t_env->common.num_players; i++){
            if(t_env->common.clients[i]->uid != t_env->turn){
                t_env->turn = t_env->common.clients[i]->uid;
                write_response(t_env->common.clients[i]->uid , RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_MOVE, 1, buffer);
                break;
            }
        }
        return FSM_EXIT;
    }
}

int ttt_complete(struct dc_fsm_environment *env){
    TicTacToeEnv *t_env = (TicTacToeEnv *) env;
    t_env->common.complete = true;
    return FSM_EXIT;
}