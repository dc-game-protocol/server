#ifndef TIC_TAC_TOE_SERVER_ENV_H
#define TIC_TAC_TOE_SERVER_ENV_H

#include <sys/select.h>
#include <netinet/in.h>

typedef struct Client_S;
struct ServerEnv_S{
    fd_set rfds_master;
    int tcp_fd, udp_fd, max_fd;
    struct Client_S *clients[1024];
    struct sockaddr_in addr;
};
typedef struct ServerEnv_S ServerEnv;

void serverEnv_create(ServerEnv* serverEnv, int port);
#endif //TIC_TAC_TOE_SERVER_ENV_H
