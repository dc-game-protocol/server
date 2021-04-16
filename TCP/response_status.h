#ifndef TIC_TAC_TOE_RESPONSE_STATUS_H
#define TIC_TAC_TOE_RESPONSE_STATUS_H

#define RESPONSE_SUCCESS_SUCCESS 10
#define SUCCESS_CONTEXT_CONFIRMATION 1

#define RESPONSE_UPDATE_UPDATE 20
#define UPDATE_CONTEXT_START 1
#define UPDATE_CONTEXT_MOVE 2
#define UPDATE_CONTEXT_END 3
#define UPDATE_CONTEXT_DISCONNECT 4

#define RESPONSE_ERROR_REQUEST 30
#define RESPONSE_ERROR_UID 31
#define RESPONSE_ERROR_TYPE 32
#define RESPONSE_ERROR_CONTEXT 33
#define RESPONSE_ERROR_PAYLOAD 34

#define RESPONSE_SERVER_ERROR 40

#define RESPONSE_GAME_ERROR_INVALID 50
#define RESPONSE_GAME_ERROR_TURN 51

#endif //TIC_TAC_TOE_RESPONSE_STATUS_H
