#ifndef MAIN_H_GUARD
#define MAIN_H_GUARD

#include "memory.h"

//Estrutura que agrega a informação necessária pela main do MAGNAEATS.
struct main_data {
	int max_ops;			//número máximo de operações
	int buffers_size;		//tamanho máximo dos buffers de mem. partilhada
	
	int n_restaurants;		//número de restaurantes
	int n_drivers;			//número de motoristas
	int n_clients;			//número de clientes
	
	int *restaurant_pids;	//process ids de restaurantes
	int *driver_pids;		//process ids de motoristas
	int *client_pids;		//process ids de clientes
	
	int* restaurant_stats;	//nº de operações encaminhadas por cada restaurante
	int* driver_stats;		//nº de operações respondidas por cada motorista
	int* client_stats;		//nº de operações recebidas por cada cliente
	
	struct operation* results;	//array com histórico de ops executadas
	
	int* terminate; //flag booleana, valor 1 indica que MAGNAEATS deve terminar a sua execução
};


/* Função que lê os argumentos da aplicação, nomeadamente o número
* máximo de operações, o tamanho dos buffers de memória partilhada
* usados para comunicação, e o número de clientes, de motoristas e de
* restaurantes. Guarda esta informação nos campos apropriados da
* estrutura main_data.
*/
void main_args(int argc, char* argv[], struct main_data* data);

/* Função que reserva a memória dinâmica necessária para a execução
* do MAGNAEATS, nomeadamente para os arrays *_pids e *_stats da estrutura 
* main_data. Para tal, pode ser usada a função create_dynamic_memory.
*/
void create_dynamic_memory_buffers(struct main_data* data);

/* Função que reserva a memória partilhada necessária para a execução do
* MAGNAEATS. É necessário reservar memória partilhada para todos os buffers da
* estrutura communication_buffers, incluindo os buffers em si e respetivos
* pointers, assim como para o array data->results e variável data->terminate.
* Para tal, pode ser usada a função create_shared_memory.
*/
void create_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers);

/* Função que inicia os processos dos restaurantes, motoristas e
* clientes. Para tal, pode usar as funções launch_*,
* guardando os pids resultantes nos arrays respetivos
* da estrutura data.
*/
void launch_processes(struct communication_buffers* buffers, struct main_data* data);

/* Função que faz interação do utilizador, podendo receber 4 comandos:
* request - cria uma nova operação, através da função create_request
* status - verifica o estado de uma operação através da função read_status
* stop - termina o execução do MAGNAEATS através da função stop_execution
* help - imprime informação sobre os comandos disponiveis
*/
void user_interaction(struct communication_buffers* buffers, struct main_data* data);

/* Se o limite de operações ainda não tiver sido atingido, cria uma nova
* operação identificada pelo valor atual de op_counter e com os dados passados em
* argumento, escrevendo a mesma no buffer de memória partilhada entre main e restaurantes.
* Imprime o id da operação e incrementa o contador de operações op_counter.
*/
void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data);

/* Função que lê um id de operação do utilizador e verifica se a mesma
* é valida. Em caso afirmativo,
* imprime informação da mesma, nomeadamente o seu estado, o id do cliente
* que fez o pedido, o id do restaurante requisitado, o nome do prato pedido
* e os ids do restaurante, motorista, e cliente que a receberam e processaram.
*/
void read_status(struct main_data* data);

/* Função que termina a execução do programa MAGNAEATS. Deve começar por 
* afetar a flag data->terminate com o valor 1. De seguida, e por esta
* ordem, deve esperar que os processos filho terminem, deve escrever as
* estatisticas finais do programa, e por fim libertar
* as zonas de memória partilhada e dinâmica previamente 
* reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
*/
void stop_execution(struct main_data* data, struct communication_buffers* buffers);

/* Função que espera que todos os processos previamente iniciados terminem,
* incluindo restaurantes, motoristas e clientes. Para tal, pode usar a função 
* wait_process do process.h.
*/
void wait_processes(struct main_data* data);

/* Função que imprime as estatisticas finais do MAGNAEATS, nomeadamente quantas
* operações foram processadas por cada restaurante, motorista e cliente.
*/
void write_statistics(struct main_data* data);

/* Função que liberta todos os buffers de memória dinâmica e partilhada previamente
* reservados na estrutura data.
*/
void destroy_memory_buffers(struct main_data* data, struct communication_buffers* buffers);


#endif
