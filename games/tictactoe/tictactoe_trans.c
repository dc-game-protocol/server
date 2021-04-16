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

typedef enum
{
    STATUS_GO,
    STATUS_WAIT,
    STATUS_FAIL,
    STATUS_WIN,
    STATUS_LOSE,
    STATUS_TIE
} TICTACTOE_STATUS;

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

}

int x_turn_start(struct dc_fsm_environment *env) {
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;

//    FD_SET(t_env->cfd_x, &(t_env->server->rfds_master));
    t_env->server->games[t_env->cfd_x] = t_env;
    return FSM_EXIT;
}

int x_turn_exec(struct dc_fsm_environment *env){
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;

//    FD_CLR(t_env->cfd_x, &(t_env->server->rfds_master));
    t_env->server->games[t_env->cfd_x] = NULL;

    memset(t_env->buffer, 0, BUFFSIZE);
    int read_status = dc_read(t_env->cfd_x, t_env->buffer, 1);
    printf("%s: %s received from x.\n", t_env->common.name, t_env->buffer);
    t_env->prev_player = 'X';
    t_env->prev_pos = 9; // 9 represents invalid
    if (read_status <= 0) {
        t_env->x_connected = 0;
        printf("%s: X has disconnected\n", t_env->common.name);
        return O_WIN;
    }

    regex_t regex;
    regcomp(&regex, "[0-8]", REG_EXTENDED);
    int status = regexec(&regex, t_env->buffer, 0, NULL, 0);

    if (status == 0) {                  // matches
        int tile = strtol(t_env->buffer, NULL, 10);
        t_env->prev_pos = tile;
        if (t_env->board[tile] == -1) {        // spot is free
            t_env->board[tile] = t_env->cfd_x;
            return X_SUCCESS;
        }
        return X_FAIL;
    }
    return X_FAIL;
}

int x_fail(struct dc_fsm_environment *env) {
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;
    printf("%s: X fail\n", t_env->common.name);
    memset(t_env->buffer, 0, BUFFSIZE);
    sprintf(t_env->buffer, "%c%d%d", t_env->prev_player, t_env->prev_pos, STATUS_FAIL);
    dc_write(t_env->cfd_x, t_env->buffer, 3);
    return X_TURN;
}

int x_success(struct dc_fsm_environment *env) {
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;
    if (has_won_game(t_env->cfd_x, t_env->board)) {
        return X_WIN;
    }
    if (is_game_tied(t_env->board)) {
        return TIE;
    }
    memset(t_env->buffer, 0, BUFFSIZE);
    sprintf(t_env->buffer, "%c%d%d", t_env->prev_player, t_env->prev_pos, STATUS_WAIT);
    send(t_env->cfd_x, t_env->buffer, 3, MSG_NOSIGNAL);

    memset(t_env->buffer, 0, BUFFSIZE);
    sprintf(t_env->buffer, "%c%d%d", t_env->prev_player, t_env->prev_pos, STATUS_GO);
    send(t_env->cfd_o, t_env->buffer, 3, MSG_NOSIGNAL);
    return O_TURN;
}

int x_win(struct dc_fsm_environment *env) {
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;
    memset(t_env->buffer, 0, BUFFSIZE);

    sprintf(t_env->buffer, "%c%d%d", t_env->prev_player, t_env->prev_pos, STATUS_WIN);
    dc_write(t_env->cfd_x, t_env->buffer, 3);
    if (t_env->o_connected == 1) {
        sprintf(t_env->buffer, "%c%d%d", t_env->prev_player, t_env->prev_pos, STATUS_LOSE);
        dc_write(t_env->cfd_o, t_env->buffer, 3);
    }
    printf("%s: X wins!\n", t_env->common.name);
    return COMPLETE;
}

int o_turn_start(struct dc_fsm_environment *env) {
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;

//    FD_SET(t_env->cfd_o, &(t_env->server->rfds_master));
    t_env->server->games[t_env->cfd_o] = t_env;
    return FSM_EXIT;
}

int o_turn_exec(struct dc_fsm_environment *env){
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;

//    FD_CLR(t_env->cfd_o, &(t_env->server->rfds_master));
    t_env->server->games[t_env->cfd_o] = NULL;

    memset(t_env->buffer, 0, BUFFSIZE);
    int read_status = dc_read(t_env->cfd_o, t_env->buffer, 1);
    printf("%s: %s received from o.\n", t_env->common.name, t_env->buffer);
    t_env->prev_player = 'O';
    t_env->prev_pos = 9; // 9 represents invalid
    if (read_status <= 0) {
        t_env->o_connected = 0;
        printf("%s: O has disconnected\n", t_env->common.name);
        return X_WIN;
    }

    regex_t regex;
    regcomp(&regex, "[0-8]", REG_EXTENDED);
    int status = regexec(&regex, t_env->buffer, 0, NULL, 0);

    if (status == 0) {                  // matches
        int tile = strtol(t_env->buffer, NULL, 10);
        t_env->prev_pos = tile;
        if (t_env->board[tile] == -1) {        // spot is free
            t_env->board[tile] = t_env->cfd_o;
            return O_SUCCESS;
        }
        return O_FAIL;
    }
    return O_FAIL;
}

int o_fail(struct dc_fsm_environment *env) {
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;
    printf("%s: O fail\n", t_env->common.name);
    memset(t_env->buffer, 0, BUFFSIZE);
    sprintf(t_env->buffer, "%c%d%d", t_env->prev_player, t_env->prev_pos, STATUS_FAIL);
    dc_write(t_env->cfd_o, t_env->buffer, 3);

    return O_TURN;
}

int o_success(struct dc_fsm_environment *env) {
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;
    if (has_won_game(t_env->cfd_o, t_env->board)) {
        return O_WIN;
    }
    if (is_game_tied(t_env->board)) {
        return TIE;
    }
    memset(t_env->buffer, 0, BUFFSIZE);
    sprintf(t_env->buffer, "%c%d%d", t_env->prev_player, t_env->prev_pos, STATUS_GO);
    send(t_env->cfd_x, t_env->buffer, 3, MSG_NOSIGNAL);

    memset(t_env->buffer, 0, BUFFSIZE);
    sprintf(t_env->buffer, "%c%d%d", t_env->prev_player, t_env->prev_pos, STATUS_WAIT);
    send(t_env->cfd_o, t_env->buffer, 3, MSG_NOSIGNAL);
    return X_TURN;
}

int o_win(struct dc_fsm_environment *env) {
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;
    memset(t_env->buffer, 0, BUFFSIZE);

    sprintf(t_env->buffer, "%c%d%d", t_env->prev_player, t_env->prev_pos, STATUS_WIN);
    dc_write(t_env->cfd_o, t_env->buffer, 3);
    if (t_env->x_connected == 1) {
        sprintf(t_env->buffer, "%c%d%d", t_env->prev_player, t_env->prev_pos, STATUS_LOSE);
        dc_write(t_env->cfd_x, t_env->buffer, 3);
    }
    printf("%s: O wins!\n", t_env->common.name);
    return COMPLETE;
}

int tie(struct dc_fsm_environment *env) {
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;
    memset(t_env->buffer, 0, BUFFSIZE);
    sprintf(t_env->buffer, "%c%d%d", t_env->prev_player, t_env->prev_pos, STATUS_TIE);

    dc_write(t_env->cfd_x, t_env->buffer, 3);
    dc_write(t_env->cfd_o, t_env->buffer, 3);

    dc_close(t_env->cfd_x);
    dc_close(t_env->cfd_o);
    printf("%s: Tie game!\n", t_env->common.name);
    return COMPLETE;
}

int complete_game(struct dc_fsm_environment *env){
    TicTacToeEnv *t_env;
    t_env = (TicTacToeEnv *) env;
    printf("%s: Deleting game\n", t_env->common.name);
    t_env->complete = 1;
    return FSM_EXIT;
}