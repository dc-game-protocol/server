#include "rockpaperscissors_trans.h"
#include "../../TCP/response.h"
#include "../../TCP/response_status.h"
#include <stdlib.h>

struct state_transition rps_transitions[] =
        {
                {FSM_INIT,              START_GAME,            rps_start},
                {START_GAME,            FSM_EXIT,              rps_validate},
                {FSM_EXIT,              FSM_EXIT,              rps_validate},
                {FSM_EXIT,              COMPLETE,              rps_complete},
                {COMPLETE,              FSM_EXIT,              NULL},
                {FSM_IGNORE,            FSM_IGNORE,            NULL}
        };

int rps_start(struct dc_fsm_environment *env){
    RockPaperScissorsEnv *r_env = (RockPaperScissorsEnv*) env;
    for(int i = 0 ; i < r_env->common.num_players; i++){
        Client* client = r_env->common.clients[i];
        write_response(client->fd, RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_START, 0, NULL);
    }
    return FSM_EXIT;
}

int rps_validate(struct dc_fsm_environment *env){
    RockPaperScissorsEnv *r_env = (RockPaperScissorsEnv*) env;

    Request req = *r_env->common.buffer;
    if(req.context != 1){
        write_response(req.uid, RESPONSE_ERROR_CONTEXT, req.context, 0, NULL);
        return FSM_EXIT;
    }else if(req.len_payload != 1){
        write_response(req.uid, RESPONSE_ERROR_PAYLOAD, req.context, 0, NULL);
        return FSM_EXIT;
    }else if(!(req.payload[0]>0 && req.payload[0]<4)){
        write_response(req.uid, RESPONSE_GAME_ERROR_INVALID, req.context, 0, NULL);
        return FSM_EXIT;
    }

    int complete = 1;
    for (int i = 0; i < r_env->common.num_players; i++) {
        if (r_env->common.clients[i]->uid == r_env->common.buffer->uid) {
            if(r_env->moves[i] == -1)
                r_env->moves[i] = req.payload[0];
            else{
                write_response(req.uid, RESPONSE_GAME_ERROR_TURN, req.context, 0, NULL);
                return FSM_EXIT;
            }
        }
        if(r_env->moves[i] == -1) {
            complete = 0;
        }
    }
    write_response(req.uid, RESPONSE_SUCCESS_SUCCESS, req.context, 0, NULL);
    return complete? COMPLETE:FSM_EXIT;
}

int rps_complete(struct dc_fsm_environment *env){
    RockPaperScissorsEnv *r_env = (RockPaperScissorsEnv*) env;
    Client ** clients = r_env->common.clients;

    uint8_t buffer[2];

    int diff = r_env->moves[0] - r_env->moves[1];
    if(diff == 0){
        //tie
        buffer[0] = 3;

        buffer[1] = r_env->moves[1];
        write_response(clients[0]->fd, RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_END, 2, buffer);

        buffer[1] = r_env->moves[0];
        write_response(clients[1]->fd, RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_END, 2, buffer);
    }else if(diff == -2 || diff == 1){
        //player 0 win
        buffer[0] = 1;
        buffer[1] = r_env->moves[1];
        write_response(clients[0]->fd, RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_END, 2, buffer);

        buffer[0] = 2;
        buffer[1] = r_env->moves[0];
        write_response(clients[1]->fd, RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_END, 2, buffer);
    }else if(diff == -1 || diff == 2){
        //player 1 win
        buffer[0] = 2;
        buffer[1] = r_env->moves[1];
        write_response(clients[0]->fd, RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_END, 2, buffer);

        buffer[0] = 1;
        buffer[1] = r_env->moves[0];
        write_response(clients[1]->fd, RESPONSE_UPDATE_UPDATE, UPDATE_CONTEXT_END, 2, buffer);
    }
    r_env->common.complete = true;
    return FSM_EXIT;
}
