#include <dc/unistd.h>
#include "response.h"
void write_response(int fd, uint8_t status, uint8_t context, uint8_t len_payload, uint8_t* payload){
    uint8_t buffer[len_payload+3];
    buffer[0] = status;
    buffer[1] = context;
    buffer[2] = len_payload;
    buffer[3] = payload;
    dc_write(fd, buffer, len_payload+3);
}