/**
 * THIS IS A HEADER COMMENT FOR IN CAMPUS DEVELOPMENT
 * THIS CODE WAS DEVELOPED BY FC56330
 * IF YOU SEE ANOTHER PROJECT WITH A FILE WITH VERY SIMILAR FUNCTION DEFINITIONS BUT WITHOUT
 * THIS HEADER, THAT MEANS THIS CODE WAS PLAGIARISED!
 * 
 */

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
  int restaurant_process = 0;
  restaurant_process = fork();
  if(restaurant_process == 0){
    int nops = execute_restaurant(restaurant_id, buffers, data);
    exit(nops);
  }

  return restaurant_process;
}

int launch_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data){
  // TODO preemptive forking action
  int driver_process = 0;
  driver_process = fork();
  if(driver_process == 0){
    int nops = execute_driver(driver_id, buffers, data);
    exit(nops);
  }

  return driver_process;
}

int launch_client(int client_id, struct communication_buffers* buffers, struct main_data* data){
  // TODO preemptive forking action
  int client_process = 0;
  client_process = fork();
  if(client_process == 0){
    int nops = execute_client(client_id, buffers, data);
    exit(nops);
  }

  return client_process;
}

int wait_process(int process_id){
  int status = 0;
  int wait_ret = waitpid(process_id, &status, 0);

  if(wait_ret == -1) return -1;

  if(WIFEXITED(status))
    return WEXITSTATUS(status);
  else
    return -1;
}
