#ifndef TIC_TAC_TOE_TICTACTOE_ENV_H
#define TIC_TAC_TOE_TICTACTOE_ENV_H

#include <dc_fsm/fsm.h>
#define BUFFSIZE 100
#define TITLESIZE 100

typedef enum
{
    ACCEPTING_CONNECTIONS = FSM_APP_STATE_START,
    CONNECTED_CLIENTS,
    X_TURN,
    X_FAIL,
    X_SUCCESS,
    X_WIN,
    O_TURN,
    O_FAIL,
    O_SUCCESS,
    O_WIN,
    TIE,
    COMPLETE
} TICTACTOE_STATE;


struct ServerEnv_S;

typedef struct {
    struct dc_fsm_environment common;
    char buffer[BUFFSIZE];
    int x_connected;
    int o_connected;
    int cfd_x;
    int cfd_o;
    int board[9];
    char prev_player;
    int prev_pos;
    int complete;
    struct ServerEnv_S *server;
} TicTacToeEnv;

struct ServerEnv_S{
    fd_set rfds_master;
    int sfd, max_fd;
    TicTacToeEnv *games[1024];
};
typedef struct ServerEnv_S ServerEnv;

TicTacToeEnv* ticTacToeEnv_create(ServerEnv* server);
void ticTacToeEnv_destroy(TicTacToeEnv* env);

#endif //TIC_TAC_TOE_TICTACTOE_ENV_H
