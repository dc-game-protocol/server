#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "rockpaperscissors_env.h"

RockPaperScissorsEnv* rockPaperScissorsEnv_create(){
    RockPaperScissorsEnv* this = (RockPaperScissorsEnv*)malloc(sizeof (RockPaperScissorsEnv));
    memset(this, 0, sizeof(RockPaperScissorsEnv));
    memset(this->moves, -1, sizeof(this->moves));

    this->common.game_id = RPS;
    this->common.protocol_ver = 1;
    this->common.num_players = rps_num_players;
    this->common.clients = (Client **) calloc(rps_num_players, sizeof(Client*));

    this->common.common.name= (char*)malloc(TITLESIZE*sizeof(char));
    sprintf(this->common.common.name, "RPS");
    this->common.common.from_state_id = FSM_INIT;
    this->common.common.current_state_id  = START_GAME;

    return this;
}
