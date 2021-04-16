#ifndef TIC_TAC_TOE_ROCKPAPERSCISSORS_TRANS_H
#define TIC_TAC_TOE_ROCKPAPERSCISSORS_TRANS_H

#include <dc_fsm/fsm.h>
#include "rockpaperscissors_env.h"

extern struct state_transition rps_transitions[];

int rps_start(struct dc_fsm_environment *env);
int rps_validate(struct dc_fsm_environment *env);
int rps_complete(struct dc_fsm_environment *env);

#endif //TIC_TAC_TOE_ROCKPAPERSCISSORS_TRANS_H
