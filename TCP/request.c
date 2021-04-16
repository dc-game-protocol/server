#include <dc/unistd.h>
#include <stdlib.h>
#include "request.h"
#include "response_status.h"

int get_request(int fd, Request* req){
    int retval = dc_read(fd, &req->uid , 1);
    if(retval == 0){
        return PARSE_DISCONNECT;
    }
    retval = dc_read(fd, &req->type, 1);
    if(retval == 0){
        return RESPONSE_ERROR_REQUEST;
    }
    retval = dc_read(fd, &req->context, 1);
    if(retval == 0){
        return RESPONSE_ERROR_REQUEST;
    }
    retval = dc_read(fd, &req->len_payload, 1);
    if(retval == 0){
        return RESPONSE_ERROR_REQUEST;
    }

    if(!req->payload){
        req->payload = (uint8_t*) malloc(req->len_payload * sizeof(uint8_t));
    }else{
        req->payload = (uint8_t*) realloc(req->payload, req->len_payload * sizeof(uint8_t));
    }

    retval = dc_read(fd, req->payload, req->len_payload);
    if(retval != req->len_payload){
        return RESPONSE_ERROR_PAYLOAD;
    }else if(req->uid != fd){
        //invalid uid(31)
        return RESPONSE_ERROR_UID;
    }else{
        return PARSE_VALID;
    }
}

void delete_request(Request req){
    free(req.payload);
}