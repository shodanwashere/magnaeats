/**
 * THIS IS A HEADER COMMENT FOR IN CAMPUS DEVELOPMENT
 * THIS CODE WAS DEVELOPED BY FC56330
 * IF YOU SEE ANOTHER PROJECT WITH A FILE WITH VERY SIMILAR FUNCTION DEFINITIONS BUT WITHOUT
 * THIS HEADER, THAT MEANS THIS CODE WAS PLAGIARISED!
 * 
 */

#include "memory.h"
#include "main.h"

/* Função principal de um Motorista. Deve executar um ciclo infinito onde em 
* cada iteração lê uma operação dos restaurantes e se a mesma tiver id 
* diferente de -1 e se data->terminate ainda for igual a 0, processa-a e
* escreve a resposta para os clientes. Operações com id igual a -1 são 
* ignoradas (op inválida) e se data->terminate for igual a 1 é porque foi 
* dada ordem de terminação do programa, portanto deve-se fazer return do
* número de operações processadas. Para efetuar estes passos, pode usar os
* outros métodos auxiliares definidos em driver.h.
*/
int execute_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data){
    int ops = 0;
    while(*data->terminate == 0){
        struct operation op;
        driver_receive_operation(&op, buffers, data);
        if(op.id != -1 && *data->terminate == 0){
            driver_process_operation(&op, driver_id, data, &ops);
            driver_send_answer(&op, buffers, data);
        }
    }
    return ops;
}

void driver_receive_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data){
    if(*data->terminate == 0)
        read_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
    return;
}

void driver_process_operation(struct operation* op, int driver_id, struct main_data* data, int* counter){
    op->receiving_driver = driver_id;
    op->status = 'D';
    data->results[op->id] = *op;
    *counter += 1;
}

void driver_send_answer(struct operation* op, struct communication_buffers* buffers, struct main_data* data){
    write_driver_client_buffer(buffers->driv_cli, data->buffers_size, op);
}