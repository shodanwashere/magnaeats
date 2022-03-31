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
  int restaurant_process;
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
  int driver_process;
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
  int client_process;
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
  waitpid(process_id, &status, 0);

  if(WIFEXITED(status))
    return WEXITSTATUS(status);
  else
    return -1;
}
