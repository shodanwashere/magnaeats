/**
 * THIS IS A HEADER COMMENT FOR IN CAMPUS DEVELOPMENT
 * THIS CODE WAS DEVELOPED BY FC56330
 * IF YOU SEE ANOTHER PROJECT WITH A FILE WITH VERY SIMILAR FUNCTION DEFINITIONS BUT WITHOUT
 * THIS HEADER, THAT MEANS THIS CODE WAS PLAGIARISED!
 * 
 */

#include "memory.h"
#include "main.h"

int execute_client(int client_id, struct communication_buffers* buffers, struct main_data* data){
    int nops = 0;
    while(*(data->terminate) == 0){
        struct operation op;
        client_get_operation(&op, client_id, buffers, data);
        if(*(data->terminate) == 0 && op.id != -1){
            client_process_operation(&op, client_id, data, &nops);
        }
    }
    return nops;
}

void client_get_operation(struct operation* op, int client_id, struct communication_buffers* buffers, struct main_data* data){
    if(*(data->terminate) == 0)
        read_driver_client_buffer(buffers->driv_cli, client_id, data->buffers_size, op);
    return;
}

void client_process_operation(struct operation* op, int client_id, struct main_data* data, int* counter){
    op->receiving_client = client_id;
    op->status = 'C';
    data->results[op->id] = *op;
    *counter += 1;
}