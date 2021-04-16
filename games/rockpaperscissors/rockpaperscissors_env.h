#ifndef TIC_TAC_TOE_ROCKPAPERSCISSORS_ENV_H
#define TIC_TAC_TOE_ROCKPAPERSCISSORS_ENV_H

#include "../../server/game_env.h"

#define rps_num_players 2

typedef struct{
    GameEnv common;
    int moves[rps_num_players];
}RockPaperScissorsEnv;

RockPaperScissorsEnv* rockPaperScissorsEnv_create();

#endif //TIC_TAC_TOE_ROCKPAPERSCISSORS_ENV_H
