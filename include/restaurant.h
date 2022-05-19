#ifndef RESTAURANT_H_GUARD
#define RESTAURANT_H_GUARD

#include "memory.h"
#include "main.h"

/* Função principal de um Restaurante. Deve executar um ciclo infinito onde em 
* cada iteração lê uma operação da main e se for dirijida ao restaurante rest_id 
* e data->terminate ainda for igual a 0, processa-a e
* escreve-a para os motoristas. Operações com id igual a -1 são ignoradas
* (op inválida) e se data->terminate for igual a 1 é porque foi dada ordem
* de terminação do programa, portanto deve-se fazer return do número de 
* operações processadas. Para efetuar estes passos, pode usar os outros
* métodos auxiliares definidos em restaurant.h.
*/
int execute_restaurant(int rest_id, struct communication_buffers* buffers, struct main_data* data);


/* Função que lê uma operação do buffer de memória partilhada entre
* a Main e os restaurantes que seja direcionada ao restaurante rest_id.
* Antes de tentar ler a operação, o processo deve
* verificar se data->terminate tem valor 1.
* Em caso afirmativo, retorna imediatamente da função.
*/
void restaurant_receive_operation(struct operation* op, int rest_id, struct communication_buffers* buffers, struct main_data* data);


/* Função que processa uma operação, alterando o seu campo receiving_rest para o id
* passado como argumento, alterando o estado da mesma para 'R', e 
* incrementando o contador de operações. Atualiza também a operação na estrutura data.
*/
void restaurant_process_operation(struct operation* op, int rest_id, struct main_data* data, int* counter);


/* Função que escreve uma operação no buffer de memória partilhada entre
* restaurantes e motoristas, efetuando a necessária sincronização antes e
* depois de escrever.
*/
void restaurant_forward_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data);

#endif
