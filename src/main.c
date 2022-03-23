#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

void main_args(int argc, char* argv[], struct main_data* data){
  //argv[0] -> prog name, irrelevant
  //argv[1] -> maximum number of operations
  data->max_ops = atoi(argv[1]);
  //argv[2] -> size of memory buffers in bytes
  data->buffers_size = atoi(argv[2]);

  //argv[3] -> number of restaurants
  data->n_restaurants = atoi(argv[3]);
  //argv[4] -> number of delivery drivers
  data->n_drivers = atoi(argv[4]);
  //argv[5] -> number of clients
  data->n_clients = atoi(argv[5]);
}

void create_dynamic_memory_buffers(struct main_data* data){
  data->restaurant_pids = calloc(data->n_restaurants, sizeof(pid_t));
  data->driver_pids = calloc(data->n_drivers, sizeof(pid_t));
  data->client_pids = calloc(data->n_clients, sizeof(pid_t));

  data->restaurant_stats = calloc(data->n_restaurants, sizeof(int));
  data->driver_stats = calloc(data->n_drivers, sizeof(pid_t));
  data->client_stats = calloc(data->n_clients, sizeof(pid_t));
}

int main(int argc, char* argv[]){
  //init data structures
  struct main_data* data = create_dynamic_memory(sizeof(struct main_data));
  struct communication_buffers* buffers = create_dynamic_memory(sizeof(struct communication_buffers));
  //requests are randomly attributed to their respective restaurant
  //there is no order of delivery
  buffers->main_rest = create_dynamic_memory(sizeof(struct rnd_access_buffer));
  //there must be an available driver to finish the order aka circular buffer
  buffers->rest_driv = create_dynamic_memory(sizeof(struct circular_buffer));
  //drivers come in arbitrary order
  buffers->driv_cli = create_dynamic_memory(sizeof(struct rnd_access_buffer));

  //execute main code
  main_args(argc, argv, data);
  create_dynamic_memory_buffers(data);         // TODO
  create_shared_memory_buffers(data, buffers); // TODO
  launch_processes(buffers, data);             // TODO
  user_interaction(buffers, data);             // TODO

  //release memory before terminating
  destroy_dynamic_memory(data);
  destroy_dynamic_memory(buffers->main_rest);
  destroy_dynamic_memory(buffers->rest_driv);
  destroy_dynamic_memory(buffers->driv_cli);
  destroy_dynamic_memory(buffers);

  return 0;
}
