#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "process.h"

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

void main_args(int argc, char* argv[], struct main_data* data){
  //argv[0] -> prog name, irrelevant
  //argv[1] -> maximum number of operations
  data->max_ops = atoi(argv[1]);
  //argv[2] -> size of memory buffers in bytes
  data->buffers_size = atoi(argv[2]);

  //argv[3] -> number of restaurants
  data->n_restaurants = atoi(argv[3]);
  //argv[4] -> number of delivery drivers
  data->n_drivers = atoi(argv[4]);
  //argv[5] -> number of clients
  data->n_clients = atoi(argv[5]);
}

void create_dynamic_memory_buffers(struct main_data* data){
  data->restaurant_pids = calloc(data->n_restaurants, sizeof(pid_t));
  data->driver_pids = calloc(data->n_drivers, sizeof(pid_t));
  data->client_pids = calloc(data->n_clients, sizeof(pid_t));

  data->restaurant_stats = calloc(data->n_restaurants, sizeof(int));
  data->driver_stats = calloc(data->n_drivers, sizeof(int));
  data->client_stats = calloc(data->n_clients, sizeof(int));
}

/* Função que reserva a memória partilhada necessária para a execução do
* MAGNAEATS. É necessário reservar memória partilhada para todos os buffers da
* estrutura communication_buffers, incluindo os buffers em si e respetivos
* pointers, assim como para o array data->results e variável data->terminate.
* Para tal, pode ser usada a função create_shared_memory.
*/
void create_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers){
  buffers->main_rest->ptrs = create_shared_memory(STR_SHM_MAIN_REST_PTR, data->buffers_size * sizeof(int));
  buffers->main_rest->buffer = create_shared_memory(STR_SHM_MAIN_REST_BUFFER, data->buffers_size * sizeof(struct operation));
  buffers->rest_driv->ptrs = create_shared_memory(STR_SHM_REST_DRIVER_PTR, data->buffers_size * sizeof(struct pointers));
  buffers->rest_driv->buffer = create_shared_memory(STR_SHM_REST_DRIVER_BUFFER, data->buffers_size * sizeof(struct operation));
  buffers->driv_cli->ptrs = create_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, data->buffers_size * sizeof(int));
  buffers->driv_cli->buffer = create_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, data->buffers_size * sizeof(struct operation));
  data->results = create_shared_memory(STR_SHM_RESULTS, data->max_ops * sizeof(struct operation));
  data->terminate = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));
}

void launch_processes(struct communication_buffers* buffers, struct main_data* data){
  //TODO
}

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
void stop_execution(struct main_data* data, struct communication_buffers* buffers){
  data->terminate = 1;

  printf("Terminando o MAGNAEATS!");
  wait_processes(data);
  printf("Imprimindo estatísticas:\n");
  write_statistics(data);
  destroy_memory_buffers(data, buffers);
}

/* Função que espera que todos os processos previamente iniciados terminem,
* incluindo restaurantes, motoristas e clientes. Para tal, pode usar a função
* wait_process do process.h.
*/
void wait_processes(struct main_data* data){
  //TODO
}

/* Função que imprime as estatisticas finais do MAGNAEATS, nomeadamente quantas
* operações foram processadas por cada restaurante, motorista e cliente.
*/
void write_statistics(struct main_data* data){
  int i;
  int* rest_stats = data->restaurant_stats;
  int* driv_stats = data->driver_stats;
  int* cli_stats = data->client_stats;
  //Printing restaurant stats
  for(i = 0; i < data->n_restaurants; rest_stats++){
    printf("Restaurante %d preparou %d pedidos!\n", i, *(rest_stats));
    i++;
  }
  //Printing driver stats
  for(i = 0; i < data->n_drivers; driv_stats++){
    printf("Motorista %d entregou %d pedidos!\n", i, *driv_stats);
    i++;
  }
  //Printing client _stats
  for(i = 0; i < data->n_clients; cli_stats++){
    printf("Cliente %d recebeu %d pedidos!\n", i, *cli_stats);
    i++;
  }
}

/* Função que liberta todos os buffers de memória dinâmica e partilhada previamente
* reservados na estrutura data.
*/
void destroy_memory_buffers(struct main_data* data, struct communication_buffers* buffers){
  //TODO
}

int main(int argc, char* argv[]){
  //init data structures
  struct main_data* data = create_dynamic_memory(sizeof(struct main_data));
  struct communication_buffers* buffers = create_dynamic_memory(sizeof(struct communication_buffers));
  //requests are randomly attributed to their respective restaurant
  //there is no order of delivery
  buffers->main_rest = create_dynamic_memory(sizeof(struct rnd_access_buffer));
  //there must be an available driver to finish the order aka circular buffer
  buffers->rest_driv = create_dynamic_memory(sizeof(struct circular_buffer));
  //drivers come in arbitrary order
  buffers->driv_cli = create_dynamic_memory(sizeof(struct rnd_access_buffer));

  //execute main code
  main_args(argc, argv, data);
  create_dynamic_memory_buffers(data);         // TODO
  create_shared_memory_buffers(data, buffers); // TODO
  launch_processes(buffers, data);             // TODO
  user_interaction(buffers, data);             // TODO

  //release memory before terminating
  destroy_dynamic_memory(data);
  destroy_dynamic_memory(buffers->main_rest);
  destroy_dynamic_memory(buffers->rest_driv);
  destroy_dynamic_memory(buffers->driv_cli);
  destroy_dynamic_memory(buffers);

  return 0;
}
