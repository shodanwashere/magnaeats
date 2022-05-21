#ifndef STATS_H_GUARD
#define STATS_H_GUARD

#include "main.h"

FILE* open_stats_file(struct main_data *data);

void write_inital_stats(struct main_data *data, FILE* stats_file);

void write_operation_stats(struct main_data *data, FILE* stats_file);

int close_stats_file(FILE *stats_file);

#endif