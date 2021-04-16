
#include <memory.h>
#include <dc/sys/socket.h>
#include <stdio.h>
#include "server_env.h"

#define BACKLOG 1

void serverEnv_create(ServerEnv* serverEnv, int port){
    int enable;

    //define server socket address
    memset(&serverEnv->addr, 0, sizeof (struct sockaddr_in));
    serverEnv->addr.sin_family = AF_INET;
    serverEnv->addr.sin_port = htons(port);
    serverEnv->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //create tcp socket
    enable = 1;
    serverEnv->tcp_fd = dc_socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(serverEnv->tcp_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR)");
    }
    dc_bind(serverEnv->tcp_fd, (struct sockaddr *) &serverEnv->addr, sizeof(struct sockaddr_in));
    dc_listen(serverEnv->tcp_fd, BACKLOG);
    //create udp socket
    enable = 1;
    serverEnv->udp_fd = dc_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (setsockopt(serverEnv->udp_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR)");
    }
    dc_bind(serverEnv->udp_fd, (struct sockaddr *) &serverEnv->addr, sizeof(struct sockaddr_in));

    //initialize fd sets
    FD_ZERO(&serverEnv->rfds_master);
    FD_SET(serverEnv->tcp_fd, &serverEnv->rfds_master);
    FD_SET(serverEnv->udp_fd, &serverEnv->rfds_master);
    serverEnv->max_fd = (serverEnv->tcp_fd>serverEnv->udp_fd)?serverEnv->tcp_fd:serverEnv->udp_fd;

}