#include <dc/unistd.h>
#include <dc/sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "tictactoe_trans.h"
#include "helper.h"

#define BACKLOG 1

int main(int argc, const char *argv[]) {
    int port = get_port(argc, argv);

    char buffer[BUFFSIZE];

    ServerEnv serverEnv;
    serverEnv.sfd = dc_socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int enable = 1;
    if (setsockopt(serverEnv.sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR)");
    }

    dc_bind(serverEnv.sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in));
    dc_listen(serverEnv.sfd, BACKLOG);
    printf("Listening on port %d\n", port);

    //init fd sets
    fd_set fds_temp;
    FD_ZERO(&serverEnv.rfds_master);
    // add listening fd to set
    FD_SET(serverEnv.sfd, &serverEnv.rfds_master);
    serverEnv.max_fd = serverEnv.sfd;

    TicTacToeEnv *new_env, *curr_env;
    printf("Server start\n");
    while (1) {
        //init fd set for select
        fds_temp = serverEnv.rfds_master;
        //listen to connections in fd set until one is ready
        int retval = select(serverEnv.max_fd + 1, &fds_temp, NULL, NULL, NULL);
        if (retval == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(serverEnv.sfd, &fds_temp)) {
            if (new_env == NULL) {
                new_env = ticTacToeEnv_create(&serverEnv);
                run(new_env);
            } else {
                run(new_env);
                new_env = NULL;
            }
        } else {
            //check if each fd is available
            for (int i = 0; i <= serverEnv.max_fd; i++) {
                //process fd if read available
                if (FD_ISSET(i, &fds_temp)) {
                    curr_env = serverEnv.games[i];
                    if(curr_env != NULL){
                        run(curr_env);
                        if (curr_env->complete) {
                            ticTacToeEnv_destroy(curr_env);
                        }
                    }else{
                        retval = dc_read(i,buffer,BUFFSIZE);
                        if(retval == 0){
                            printf("disconnect");
                            FD_CLR(i, &(serverEnv.rfds_master));
                        }else{
                            printf("invalid");
                        }
                    }
                }
            }
        }
    }
    return EXIT_SUCCESS;
}