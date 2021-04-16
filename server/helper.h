#ifndef TIC_TAC_TOE_HELPER_H
#define TIC_TAC_TOE_HELPER_H

#include "game_env.h"
#include <dc_fsm/fsm.h>
#define DEFAULT_PORT 3333
int get_port(int argc, const char *argv[]);
void run(GameEnv * env);
int has_won_game(int cfd, int board[9]);
int is_game_tied(int board[9]);

#endif //TIC_TAC_TOE_HELPER_H
