#include <signal.h>
#include <time.h>
#include "main.h"
#include "memory.h"

// Alarms Section

struct main_data *m;
int counter = 0;

void save_struct_pointers(struct main_data *data, struct communication_buffers *buffers, struct semaphores *sems){
	m = data;
	b = buffers;
	s = sems;
}

void inc(){
	counter++;
}

void print_ops(){
	for(int i = 0; (i < counter) && (i < m->max_ops); i++){
		operation op = m->results[i];
		printf("request:%d status:%c", op.id, op.status);
		if(op.status == 'C'){
			printf(" start:%d restaurant:%d rest_time:%d driver:%d driver_time:%d client:%d client_end_time:%d", op.start_time.tv_sec, op.receiving_rest, op.rest_time.tv_sec, op.receiving_driver, op.driver_time.tv_sec, op.receiving_client, op.client_end_time.tv_sec);
		}
		putchar('\n');
	}
	signal(SIGALRM, print_ops);
}

void set_alarm(struct main_data *data){
	signal(SIGALRM, print_ops);
	alarm(data->alarm_time);
}

// Signals Section

struct communication_buffers *b;
struct semaphores *s;

void sigint_stop_execution(){
	stop_execution(m, b, s);
}

void arm_exit_signal(){
	signal(SIGINT, sigint_stop_execution);
}

