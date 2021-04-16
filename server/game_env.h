#ifndef TIC_TAC_TOE_GAME_ENV_H
#define TIC_TAC_TOE_GAME_ENV_H

#include <dc_fsm/fsm.h>
#include "../TCP/request.h"
#include "server_env.h"
#include "client.h"

#define TITLESIZE 100
#define NUM_GAMES 2
#define TTT 1
#define RPS 2

typedef enum
{
    START_GAME = FSM_APP_STATE_START,
    COMPLETE
} GAME_STATE;

struct GameEnv_S{
    struct dc_fsm_environment common;
    Request* buffer;
    int game_id;
    int protocol_ver;
    int complete;
    int num_players;
    int player_count;
    Client** clients;
};

typedef struct GameEnv_S GameEnv;

GameEnv* gameEnv_create(int game_id);
void gameEnv_endGame(GameEnv* gameEnv, ServerEnv* serverEnv, int quit_fd);
void gameEnv_destroy(GameEnv* gameEnv, ServerEnv* serverEnv);

#endif //TIC_TAC_TOE_GAME_ENV_H
