#include <stdio.h>
#include <time.h>
#include "main.h"
#include "memory.h"

FILE* open_stats_file(struct main_data *data){
	return fopen(data->statistics_filename, "w");
}

void write_initial_stats(struct main_data *data, FILE* stats_file){
	int i;
  	int* rest_stats = data->restaurant_stats;
  	int* driv_stats = data->driver_stats;
  	int* cli_stats = data->client_stats;
	fprintf(stats_file, "Process Statistics:\n");
  	//Printing restaurant stats
  	for(i = 0; i < data->n_restaurants; rest_stats++){
    		fprintf(stats_file, "\tRestaurant %d prepared %d requests!\n", i, *(rest_stats));
    		i++;
  	}
  	//Printing driver stats
  	for(i = 0; i < data->n_drivers; driv_stats++){
    		fprintf(stats_file, "\tDriver %d delivered %d requests!\n", i, *driv_stats);
    		i++;
  	}
  	//Printing client _stats
  	for(i = 0; i < data->n_clients; cli_stats++){
    		fprintf(stats_file, "\tClient %d received %d requests!\n", i, *cli_stats);
    	i++;
  	}
	fprintf(stats_file, "\n");
}

void write_operation_stats(struct main_data *data, FILE* stats_file){
	fprintf(stats_file, "Request Statistics:\n");
	int exit = 0;
	for(int i = 0; !exit && i < data->max_ops; i++){
		switch(data->results[i].status){
			case 'I':
			case 'R':
			case 'D':
			case 'C': break;
			default: exit = 1; break;
		}
		if(!exit){
			struct operation op = data->results[i];
			struct tm *op_creation = localtime(&op.start_time.tv_sec);
			struct tm *op_rest_time = localtime(&op.rest_time.tv_sec);
			struct tm *op_driv_time = localtime(&op.driver_time.tv_sec);
			struct tm *op_cli_end_time = localtime(&op.client_end_time.tv_sec);
			struct tm *op_total_time = localtime(&op.client_end_time.tv_sec - op.start_time.tv_sec);
			fprintf(stats_file, "Request: %d\nStatus: %c\nRestaurant id: %d\nDriver id: %d\nClient id:%d\n", op.id, op.status, op.receiving_rest, op.receiving_driver, op.receiving_client);
			fprintf(stats_file, "Created: %04d-%d-%02d %d:%02d:%02d\n", op_creation->tm_year, op_creation->tm_mon, op_creation->tm_mday, op_creation->tm_hour, op_creation->tm_min, op_creation->tm_sec);
			fprintf(stats_file, "Restaurant time: %04d-%d-%02d %d:%02d:%02d\n", op_rest_time->tm_year, op_rest_time->tm_mon, op_rest_time->tm_mday, op_rest_time->tm_hour, op_rest_time->tm_min, op_rest_time->tm_sec);
			fprintf(stats_file, "Driver time: %04d-%d-%02d %d:%02d:%02d\n", op_driv_time->tm_year, op_driv_time->tm_mon, op_driv_time->tm_mday, op_driv_time->tm_hour, op_driv_time->tm_min, op_driv_time->tm_sec);
			fprintf(stats_file, "Client time (end): %04d-%d-%02d %d:%02d:%02d\n", op_cli_end_time->tm_year, op_cli_end_time->tm_mon, op_cli_end_time->tm_mday, op_cli_end_time->tm_hour, op_cli_end_time->tm_min, op_cli_end_time->tm_sec);
			fprintf(stats_file, "Total Time: %d\n\n", op_total_time->tm_sec);
		}
	}
}

int close_stats_file(FILE* stats_file){
	return fclose(stats_file);
}

