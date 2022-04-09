/**
 * Grupo SO-011
 * Autores:
 * - Nuno Dias, fc56330
 * - Francisco Maia, fc55855
 * - Bruna Santos, fc56328
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "memory.h"
#include "client.h"

int execute_client(int client_id, struct communication_buffers* buffers, struct main_data* data){
    int nops = 0;
    while(*(data->terminate) == 0){
        struct operation op;
        client_get_operation(&op, client_id, buffers, data);
        if(*(data->terminate) == 0 && op.id != -1){
            printf("Cliente recebeu pedido!\n");
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