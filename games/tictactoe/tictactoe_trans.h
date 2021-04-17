#ifndef TIC_TAC_TOE_TICTACTOE_TRANS_H
#define TIC_TAC_TOE_TICTACTOE_TRANS_H

#include <dc_fsm/fsm.h>
#include "tictactoe_env.h"

extern struct state_transition ttt_transitions[];

int ttt_start(struct dc_fsm_environment *env);
int ttt_validate(struct dc_fsm_environment *env);
int ttt_complete(struct dc_fsm_environment *env);

#endif //TIC_TAC_TOE_TICTACTOE_TRANS_H
