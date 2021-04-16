#ifndef TIC_TAC_TOE_TICTACTOE_TRANS_H
#define TIC_TAC_TOE_TICTACTOE_TRANS_H

#include <dc_fsm/fsm.h>
#include "tictactoe_env.h"

extern struct state_transition ttt_transitions[];

int ttt_start(struct dc_fsm_environment *env);


int accept_connections(struct dc_fsm_environment *env);
int start_game(struct dc_fsm_environment *env);
int x_turn_start(struct dc_fsm_environment *env);
int x_turn_exec(struct dc_fsm_environment *env);
int x_fail(struct dc_fsm_environment *env);
int x_success(struct dc_fsm_environment *env);
int x_win(struct dc_fsm_environment *env);
int o_turn_start(struct dc_fsm_environment *env);
int o_turn_exec(struct dc_fsm_environment *env);
int o_fail(struct dc_fsm_environment *env);
int o_success(struct dc_fsm_environment *env);
int o_win(struct dc_fsm_environment *env);
int tie(struct dc_fsm_environment *env);
int complete_game(struct dc_fsm_environment *env);

#endif //TIC_TAC_TOE_TICTACTOE_TRANS_H
