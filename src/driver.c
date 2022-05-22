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
#include "driver.h"
#include "synchronization.h"

/* Função principal de um Motorista. Deve executar um ciclo infinito onde em 
* cada iteração lê uma operação dos restaurantes e se a mesma tiver id 
* diferente de -1 e se data->terminate ainda for igual a 0, processa-a e
* escreve a resposta para os clientes. Operações com id igual a -1 são 
* ignoradas (op inválida) e se data->terminate for igual a 1 é porque foi 
* dada ordem de terminação do programa, portanto deve-se fazer return do
* número de operações processadas. Para efetuar estes passos, pode usar os
* outros métodos auxiliares definidos em driver.h.
*/
int execute_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores *sems){
    int ops = 0;
    while(*(data->terminate) != 1){
        struct operation op;
        // Begin Consoomption
        driver_receive_operation(&op, buffers, data, sems);
        // Consoomption finished
        if(op.id != -1 && *(data->terminate) == 0){
            printf("Motorista recebeu pedido!\n");
            driver_process_operation(&op, driver_id, data, &ops, sems);
            // Begin Production
            driver_send_answer(&op, buffers, data, sems);
            // End Production
        }
    }
    return ops;
}

void driver_receive_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    if(*(data->terminate) != 1)
        produce_begin(sems->rest_driv);
        read_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
        produce_end(sems->rest_driv);
    return;
}

void driver_process_operation(struct operation* op, int driver_id, struct main_data* data, int* counter, struct semaphores* sems){
    op->receiving_driver = driver_id;
    op->status = 'D';
    // begin results mutex
    semaphore_mutex_lock(sems->results_mutex);
    data->results[op->id] = *op;
    semaphore_mutex_unlock(sems->results_mutex);
    // results mutex finished
    *counter += 1;
}

void driver_send_answer(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    consume_begin(sems->driv_cli); // consumption begins here
    write_driver_client_buffer(buffers->driv_cli, data->buffers_size, op);
    consume_end(sems->driv_cli);   // consumption ends here
}