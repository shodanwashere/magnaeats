#ifndef MESIGNAL_H_GUARD
#define MESIGNAL_H_GUARD

#include "main.h"

void save_struct_pointers(struct main_data *data, struct communication_buffers *buffers, struct semaphores *sems);

// MESIGNAL -- Alarms section

void inc();

void print_ops();

void set_alarm(struct main_data* data);

// MESIGNAL -- Signals section

void sigint_stop_execution();

void arm_exit_signal();

#endif