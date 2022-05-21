#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "memory.h"

void register_start_time(struct operation* op){
    clock_gettime(CLOCK_REALTIME,  &op.start_time);
}

void register_rest_time(struct operation* op){
    clock_gettime(CLOCK_REALTIME,  &op.rest_time);
}

void register_driver_time(struct operation* op){
    clock_gettime(CLOCK_REALTIME,  &op.driver_time);
}

void register_client_end_time(struct operation* op){
    clock_gettime(CLOCK_REALTIME,  &op.client_end_time);
}