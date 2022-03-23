#include "memory.h"
#include "main.h"
#include "restaurant.h"
#include "driver.h"
#include "client.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int launch_restaurant(int restaurant_id, struct communication_buffers* buffers, struct main_data* data){
  // TODO preemptive forking action
  pid_t restaurant_process;
  if((restaurant_process = fork()) == -1){
    // do what? please elaborate on discord, i actually dont know
    exit(-1);
  }
  if(restaurant_process == 0)
    execute_restaurant(0, buffers, data);

  return restaurant_process;
}

int launch_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data){
  // TODO preemptive forking action
  pid_t driver_process;
  if((driver_process = fork()) == -1){
    // do what? please elaborate on discord, i actually dont know
    exit(-1);
  }
  if(driver_process == 0)
    execute_driver(0, buffers, data);

  return driver_process;
}

int launch_client(int client_id, struct communication_buffers* buffers, struct main_data* data){
  // TODO preemptive forking action
  pid_t client_process;
  if((client_process = fork()) == -1){
    // do what? please elaborate on discord, i actually dont know
    exit(-1);
  }
  if(client_process == 0)
    execute_driver(0, buffers, data);

  return client_process;
}

int wait_process(int process_id){
  int status = 0;
  /* receive from */ waitpid(process_id, &status, 0);

  if(WIFEXITED(status))
    // return item from waitpid
  else
    return -1;
}
