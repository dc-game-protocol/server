#ifndef TIC_TAC_TOE_CLIENT_H
#define TIC_TAC_TOE_CLIENT_H

#include <netinet/in.h>
#include <dc_fsm/fsm.h>
#include "server_env.h"

typedef struct GameEnv_S;
struct Client_S{
    int fd;//uid
    struct sockaddr_in * addr;
    struct GameEnv_S* gameEnv;
};
typedef struct Client_S Client;

Client * client_create(int fd, ServerEnv* serverEnv);
void client_destroy(Client* client);
#endif //TIC_TAC_TOE_CLIENT_H
