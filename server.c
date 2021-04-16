#include <dc/unistd.h>
#include <dc/sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server/server_env.h"
#include "server/game_env.h"
#include "server/client.h"
#include "server/helper.h"
#include "TCP/request.h"

#include "TCP/response_status.h"
#include "TCP/response.h"



int confirm_ruleset(Request* req, GameEnv** new_games, Client* client);

int main(int argc, const char *argv[]) {
    //get port
    int port = get_port(argc, argv);

    //initialize server environment
    ServerEnv serverEnv;
    serverEnv_create(&serverEnv, port);

    Request req;
    GameEnv* new_games[NUMGAMES];

    //init fd sets
    fd_set fds_temp;

    //server env created
    printf("Server starting: listening on port %d\n", port);
    while (1) {
        //init fd set for select
        fds_temp = serverEnv.rfds_master;
        //listen to connections in fd set until one is ready
        int retval = select(serverEnv.max_fd + 1, &fds_temp, NULL, NULL, NULL);
        if (retval == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if(FD_ISSET(serverEnv.udp_fd, &fds_temp)){
            //parse user id
            //find client with user id
            //if they don't have a socket, create socket
            //transmit data to all sockets in their game
        }else if (FD_ISSET(serverEnv.tcp_fd, &fds_temp)) {
            //add to clients
            int fd = dc_accept(serverEnv.tcp_fd, NULL, NULL);
            serverEnv.clients[fd] = client_create(fd, &serverEnv);
        }else {
            //check if each fd is available
            for (int i = 0; i <= serverEnv.max_fd; i++) {
                //process fd if read available
                if (FD_ISSET(i, &fds_temp)) {
                    //read message into buffer
                    int res_status = get_request(i, &req);
                    if(res_status == PARSE_DISCONNECT){
                        //handle disconnect
                        if(serverEnv.clients[i]->gameEnv != NULL)
                            gameEnv_endGame(serverEnv.clients[i]->gameEnv, &serverEnv, i);
                        else
                            FD_CLR(i, &serverEnv.rfds_master);
                    }else if(res_status == PARSE_VALID){
                        //get client (parse request uid byte)
                        Client* curr_client = serverEnv.clients[i];
                        if(curr_client == NULL){
                            res_status = RESPONSE_ERROR_REQUEST;
                        }else{
                            switch (req.type) {
                                case REQTYPE_CONFIRMATION:
                                    //create new game
                                    res_status = confirm_ruleset(&req, new_games, curr_client);
                                    break;
                                case REQTYPE_META:
                                    //handle quit
                                    if(req.context == 1)
                                        gameEnv_endGame(curr_client->gameEnv, &serverEnv, i);
                                    else
                                        res_status = RESPONSE_ERROR_CONTEXT;
                                    break;
                                case REQTYPE_GAME:
                                    //game runs move
                                    curr_client->gameEnv->buffer = &req;
                                    run(curr_client->gameEnv);
                                    if (curr_client->gameEnv->complete) {
                                        gameEnv_destroy(curr_client->gameEnv, &serverEnv);
                                    }
                                    break;
                                default:
                                    res_status = RESPONSE_ERROR_TYPE;
                            }
                        }
                    }
                    if(res_status != PARSE_VALID && res_status != PARSE_DISCONNECT){
                        //handle invalid
                        write_response(i, res_status, req.type, 0 , NULL);
                    }
                }
            }
        }
    }
    delete_request(req);
    return EXIT_SUCCESS;
}

int confirm_ruleset(Request* req, GameEnv** new_games, Client* client){
    //invalid context (33)
    if(req->context != 1){
        return RESPONSE_ERROR_CONTEXT;
    }

    uint8_t game_id = req->payload[0];
    uint8_t protocol_v = req->payload[1];
    //validate request
    if(game_id > NUMGAMES || protocol_v != 1) {
        return RESPONSE_ERROR_PAYLOAD;
    }

    //create new game if there isn't one in progress
    if(new_games[game_id-1]==NULL){
        new_games[game_id-1] = gameEnv_create(game_id);
        if(new_games[game_id-1] == NULL)
            return RESPONSE_SERVER_ERROR;
    }

    GameEnv* curr_env = new_games[game_id-1];
    //add client to game
    curr_env->clients[curr_env->player_count++] = client;
    //add game to client
    client->gameEnv = curr_env;

    //update client
    write_response(client->fd, RESPONSE_SUCCESS_SUCCESS, req->type, 0, NULL);
    //run game if sufficent players
    if(curr_env->player_count == curr_env->num_players){
        run(curr_env);
        new_games[game_id-1] = NULL;
    }

    return PARSE_VALID;
}