/**
 * THIS IS A HEADER COMMENT FOR IN CAMPUS DEVELOPMENT
 * THIS CODE WAS DEVELOPED BY FC56330
 * IF YOU SEE ANOTHER PROJECT WITH A FILE WITH VERY SIMILAR FUNCTION DEFINITIONS BUT WITHOUT
 * THIS HEADER, THAT MEANS THIS CODE WAS PLAGIARISED!
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "main.h"
#include "restaurant.h"

int execute_restaurant(int rest_id, struct communication_buffers* buffers, struct main_data* data){
  int ops_procs = 0;
  struct operation next_consumed;
  while(*(data->terminate) == 0){
    if(*(data->terminate) == 1) printf("Exit should have started but it hasn't");
    restaurant_receive_operation(&next_consumed, rest_id, buffers, data);
    if(next_consumed.id != -1 && *(data->terminate) == 0){
      printf("Restaurante recebeu pedido!\n");
      restaurant_process_operation(&next_consumed, rest_id, data, &ops_procs);
      restaurant_forward_operation(&next_consumed, buffers, data);
    }
  }
  return ops_procs;
}

void restaurant_receive_operation(struct operation* op, int rest_id, struct communication_buffers* buffers, struct main_data* data){
  if(*(data->terminate) == 0){
    read_main_rest_buffer(buffers->main_rest, rest_id, data->buffers_size, op);
  }
}

void restaurant_process_operation(struct operation *op, int rest_id, struct main_data* data, int* counter){
  op->receiving_rest = rest_id;
  op->status = 'R';
  data->results[op->id] = *op;
  *counter += 1;
}

void restaurant_forward_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data){
  write_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
}
