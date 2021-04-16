#ifndef TIC_TAC_TOE_RESPONSE_H
#define TIC_TAC_TOE_RESPONSE_H

#include <stdint-gcc.h>

void write_response(int fd, uint8_t status, uint8_t context, uint8_t len_payload, uint8_t* payload);

#endif //TIC_TAC_TOE_RESPONSE_H
