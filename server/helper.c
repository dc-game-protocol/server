#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "helper.h"
//#include "tictactoe_trans.h"

#include "../games/rockpaperscissors/rockpaperscissors_trans.h"

int get_port(int argc, const char *argv[]){
    int opt;
    int opt_index;
    int help_flag = 0;
    struct option long_options[] = {
            {"port",           optional_argument, 0,          'p'},
            {"help",           no_argument,       &help_flag, 1}
    };
    while ((opt = getopt_long(argc, argv, "p:", long_options, &opt_index)) != -1) {
        if (help_flag) {
            fprintf(stdout, "%s", "Usage:\n\n");
            fprintf(stdout, "%s", "Command line options:\n");
            fprintf(stdout, "%s", "-p PORT, --port=PORT                 Sets the port to PORT (max 65535).\n\n");
            exit(EXIT_SUCCESS);
        }

        if (optarg == NULL || optarg[0] == '\0') {
            continue;
        }

        switch (opt) {
            case 'p': {
                char *ptr;
                int port = (int) strtoul(optarg, &ptr, 0);
                if (port >= 0 && port <= 65535) {
                    if (*optarg != '\0' && *ptr == '\0') {
                        return port;
                    }
                }
                break;
            }
        }
    }
    return DEFAULT_PORT;
}

void run(GameEnv * env){
    struct state_transition* transitions;
    if(env->game_id == TTT){

    }else if(env->game_id == RPS){
        transitions = rps_transitions;
    }

    int code = dc_fsm_run((struct dc_fsm_environment *) env, &env->common.from_state_id,
                          &env->common.current_state_id, transitions, true);
    if (code != 0) {
        fprintf(stderr, "Cannot move from %d to %d\n", env->common.from_state_id,
                env->common.current_state_id);
        exit(EXIT_FAILURE);
    }

}

int has_won_game(int cfd, int board[9]) {
    // Horizontal
    if (board[0] == cfd && board[1] == cfd && board[2] == cfd)
        return 1;
    if (board[3] == cfd && board[4] == cfd && board[5] == cfd)
        return 1;
    if (board[6] == cfd && board[7] == cfd && board[8] == cfd)
        return 1;
    // Diagonal
    if (board[0] == cfd && board[4] == cfd && board[8] == cfd)
        return 1;
    if (board[2] == cfd && board[4] == cfd && board[6] == cfd)
        return 1;
    // Vertical
    if (board[0] == cfd && board[3] == cfd && board[6] == cfd)
        return 1;
    if (board[1] == cfd && board[4] == cfd && board[7] == cfd)
        return 1;
    if (board[2] == cfd && board[5] == cfd && board[8] == cfd)
        return 1;

    return 0;
}

int is_game_tied(int board[9]) {
    return (board[0] != -1 && board[1] != -1 && board[2] != -1
            && board[3] != -1 && board[4] != -1 && board[5] != -1
            && board[6] != -1 && board[7] != -1 && board[8] != -1);
}