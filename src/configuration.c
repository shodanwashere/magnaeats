#include <stdio.h>
#include <stdlib.h>
#include "main.h"

#define MAXLINE 1024

void read_configfile(char *config, struct main_data* data){
    FILE *configfile;
    configfile = fopen(config, "r");
    
    int counter = 0;
    while (fgets(line, MAXLINE, configfile) != NULL){
        switch(counter){
            case 0 : data->max_ops = atoi(line); break;
            case 1 : data->buffers_size = atoi(line); break;
            case 2 : data->n_restaurants = atoi(line); break;
            case 3 : data->n_drivers = atoi(line); break;
            case 4 : data->n_clients = atoi(line); break;
            case 5 : data->log_filename = line; break;
            case 6 : data->statistics_filename = line; break;
            case 7 : data->alarm_time = atoi(line); break;
            default: break; /* do nothing */
        }
    counter++;
    }
  fclose(configfile);
}