#ifndef TIC_TAC_TOE_REQUEST_H
#define TIC_TAC_TOE_REQUEST_H

#include <stdint-gcc.h>

#define REQTYPE_CONFIRMATION 1
#define REQTYPE_META 3
#define REQTYPE_GAME 4

#define PARSE_DISCONNECT -1
#define PARSE_VALID 0

typedef struct{
    uint32_t uid;
    uint8_t type;
    uint8_t context;
    uint8_t len_payload;
    uint8_t *payload;
}Request;

int get_request(int fd, Request* req);
int validate_request(Request req);
void delete_request(Request req);
#endif //TIC_TAC_TOE_REQUEST_H
