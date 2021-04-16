#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include "client.h"

void client_destroy(Client* client){
    free(client->addr);
    close(client->fd);
    //clients get freed by game, !free game here
}

Client * client_create(int fd, ServerEnv* serverEnv){
    Client* this = (Client*)malloc(sizeof (Client));
    memset(this, 0, sizeof(Client));
    this->fd = fd;
    this->uid = fd;

    FD_SET(fd, &serverEnv->rfds_master);
    if(fd > serverEnv->max_fd){
        serverEnv->max_fd = fd;
    }

    return this;
}