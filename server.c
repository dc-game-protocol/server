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


const size_t udp_len = 5008;

int confirm_ruleset(Request* req, GameEnv** new_games, Client* client);

int main(int argc, const char *argv[]) {
    //get port
    int port = get_port(argc, argv);

    //initialize server environment
    ServerEnv serverEnv;
    serverEnv_create(&serverEnv, port);

    Request req;
    GameEnv* new_games[NUM_GAMES];

    uint8_t udp_buffer[udp_len];
    struct sockaddr_in in_addr;
    socklen_t addr_len;

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
            int n_recv= recvfrom(serverEnv.udp_fd, udp_buffer, udp_len, MSG_WAITALL, &in_addr, &addr_len);
            if(n_recv< 64) continue;
            //parse ordering
            uint32_t ordering = udp_buffer[0] | (udp_buffer[1] << 8) | (udp_buffer[2] << 16) | (udp_buffer[3] << 24);
            //parse user id
            uint32_t uid = udp_buffer[4] | (udp_buffer[5] << 8) | (udp_buffer[6] << 16) | (udp_buffer[7] << 24);
            //find client with user id
            Client* client = serverEnv.clients[uid];
            //if they don't have a socket, create socket
            if(client->addr == NULL){
                client->addr = (struct sockaddr_in *) calloc(0, sizeof(struct sockaddr_in));
                *client->addr = in_addr;
                client->addr_len = addr_len;
            }
            //drop packet if it's late
            if(ordering<client->ordering) continue;
            //transmit data to all sockets in their game
            for(int i = 0; i < client->gameEnv->player_count; i++){
                if(client->gameEnv->clients[i]->uid != uid && client->gameEnv->clients[i]->addr != NULL)
                    sendto(serverEnv.udp_fd, udp_buffer, n_recv, 0,client->gameEnv->clients[i]->addr,client->gameEnv->clients[i]->addr_len);
            }
            //update client's ordering
            client->ordering = ordering;
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
                        if(serverEnv.clients[i]->gameEnv != NULL) {
                            if(serverEnv.clients[i]->gameEnv == new_games[serverEnv.clients[i]->gameEnv->game_id-1]){
                                new_games[serverEnv.clients[i]->gameEnv->game_id-1] = NULL;
                            }
                            gameEnv_endGame(serverEnv.clients[i]->gameEnv, &serverEnv, i);
                        }else{
                            FD_CLR(i, &serverEnv.rfds_master);
                            client_destroy(serverEnv.clients[i]);
                            serverEnv.clients[i] = NULL;
                        }

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

    uint8_t protocol_v = req->payload[0];
    uint8_t game_id = req->payload[1];
    //validate request
    if(game_id > NUM_GAMES || protocol_v != 1) {
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
    uint8_t buffer[4] = {0};
    buffer[0] = client->uid;
    write_response(client->fd, RESPONSE_SUCCESS_SUCCESS, req->type, 4, buffer);
    //run game if sufficent players
    if(curr_env->player_count == curr_env->num_players){
        run(curr_env);
        //reset new game holder for game
        new_games[game_id-1] = NULL;
    }

    return PARSE_VALID;
}