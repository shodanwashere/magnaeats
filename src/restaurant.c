#include <stdlib.h>
#include "memory.h"
#include "main.h"

int execute_restaurant(int rest_id, struct communication_buffers* buffers, struct main_data* data){
  int ops_procs = 0;
  struct operation* next_consumed;
  int n;
  while(data->terminate != 1){
    restaurant_receive_operation(next_consumed, rest_id, buffers, data);
    if(next_consumed->id != -1 && data->terminate == 0){
      restaurant_process_operation(next_consumed, rest_id, data, &ops_procs);
      restaurant_forward_operation(next_consumed, buffers, data);
    }
  }
  return ops_procs;
}

void restaurant_receive_operation(struct operation* op, int rest_id, struct communication_buffers* buffers, struct main_data* data){
  if(data->terminate == 0){
    read_main_rest_buffer(buffers->main_rest, rest_id, data->buffers_size, op);
  }
}

void restaurant_process_operation(struct operation *op, int rest_id, struct main_data* data, int* counter){
  op->receiving_rest = rest_id;
  op->status = 'R';
  *counter += 1;
}

void restaurant_forward_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data){
  write_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
}
