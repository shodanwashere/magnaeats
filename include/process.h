#ifndef PROCESS_H_GUARD
#define PROCESS_H_GUARD

#include "memory.h"
#include "main.h"

/* Função que inicia um novo processo restaurante através da função fork do SO. O novo
* processo irá executar a função execute_restaurant respetiva, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_restaurant(int restaurant_id, struct communication_buffers* buffers, struct main_data* data);


/* Função que inicia um novo processo motorista através da função fork do SO. O novo
* processo irá executar a função execute_driver, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data);


/* Função que inicia um novo processo cliente através da função fork do SO. O novo
* processo irá executar a função execute_client, fazendo exit do retorno.
* O processo pai devolve o pid do processo criado.
*/
int launch_client(int client_id, struct communication_buffers* buffers, struct main_data* data);


/* Função que espera que um processo termine através da função waitpid. 
* Devolve o retorno do processo, se este tiver terminado normalmente.
*/
int wait_process(int process_id);

#endif
