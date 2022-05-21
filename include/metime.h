#ifndef METIME_H_GUARD
#define METIME_H_GUARD

#include <stdio.h>

void register_start_time(struct operation* op);

void register_rest_time(struct operation* op);

void register_driver_time(struct operation* op);

void register_client_end_time(struct operation* op);

#endif