#include <dc/unistd.h>
#include <malloc.h>
#include "response.h"
void write_response(int fd, uint8_t status, uint8_t context, uint8_t len_payload, uint8_t* payload){
    uint8_t* buffer = (uint8_t*) malloc(len_payload+3*sizeof(uint8_t));
    buffer[0] = status;
    buffer[1] = context;
    buffer[3] = len_payload;
    buffer[4] = payload;
    dc_write(fd, buffer, len_payload+3);
}