/**
 * Grupo SO-011
 * Autores:
 * - Nuno Dias, fc56330
 * - Francisco Maia, fc55855
 * - Bruna Santos, fc56328
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "main.h"
#include "configuration.h"
#include "memory.h"
#include "process.h"
#include "log.h"
#include "synchronization.h"

void main_args(int argc, char* argv[], struct main_data* data){

  read_configfile(argv[0], data);
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
  buffers->main_rest->ptrs = (int*) create_shared_memory(STR_SHM_MAIN_REST_PTR, data->buffers_size * sizeof(int));
  buffers->main_rest->buffer = (struct operation*) create_shared_memory(STR_SHM_MAIN_REST_BUFFER, data->buffers_size * sizeof(struct operation));
  buffers->rest_driv->ptrs = (struct pointers*) create_shared_memory(STR_SHM_REST_DRIVER_PTR, data->buffers_size * sizeof(struct pointers));
  buffers->rest_driv->buffer = (struct operation*) create_shared_memory(STR_SHM_REST_DRIVER_BUFFER, data->buffers_size * sizeof(struct operation));
  buffers->driv_cli->ptrs = (int*) create_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, data->buffers_size * sizeof(int));
  buffers->driv_cli->buffer = (struct operation*) create_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, data->buffers_size * sizeof(struct operation));
  data->results = (struct operation*) create_shared_memory(STR_SHM_RESULTS, data->max_ops * sizeof(struct operation));
  data->terminate = (int*) create_shared_memory(STR_SHM_TERMINATE, sizeof(int));
}

/* Função que inicia os processos dos restaurantes, motoristas e
* clientes. Para tal, pode usar as funções launch_*,
* guardando os pids resultantes nos arrays respetivos
* da estrutura data.
*/
void launch_processes(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
  int n;
  int nrest = data->n_restaurants;
  int ndriv = data->n_drivers;
  int ncli  = data->n_clients;
  
  // initiating restaurants
  for(n = 0; n < nrest; n++){
    data->restaurant_pids[n] = launch_restaurant(n, buffers, data, sems);
  }
  // initiating drivers
  for(n = 0; n < ndriv; n++){
    data->driver_pids[n] = launch_driver(n, buffers, data, sems);
  }
  // initiating clients
  for(n = 0; n < ncli; n++){
    data->client_pids[n] = launch_client(n, buffers, data, sems);
  }
}

/* Função que faz interação do utilizador, podendo receber 4 comandos:
* request - cria uma nova operação, através da função create_request
* status - verifica o estado de uma operação através da função read_status
* stop - termina o execução do MAGNAEATS através da função stop_execution
* help - imprime informação sobre os comandos disponiveis
*/
void user_interaction(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
  char help_msg[] = "Ações disponíveis:\n\trequest client restaurant dish - criar um novo pedido\n\tstatus id - consultar o estado de um pedido\n\tstop - termina a execução do magnaeats\n\thelp - imprime informação sobre as ações disponíveis\n";
  int op_n = 0;
  printf("%s", help_msg);
  while(*(data->terminate) == 0){
    fflush(stdin);
    printf("Introduzir ação: ");
    char *action_line = calloc(20, sizeof(char));
    scanf("%s", action_line);
    if(strcmp(action_line, "request") == 0){
      fflush(stdin);
      create_request(&op_n, buffers, data, sems);
      continue;
    } else if(strcmp(action_line, "status") == 0){
      read_status(data, sems);
      continue;
    } else if(strcmp(action_line, "help") == 0){
      printf("%s", help_msg);
      FILE* logfile = open_logfile(data->log_filename);
      log(logfile, action_line, strlen(action_line), 0);
      close_logfile(logfile);
      continue;
    } else if(strcmp(action_line, "stop") == 0){
      stop_execution(data, buffers, sems);
      FILE* logfile = open_logfile(data->log_filename);
      log(logfile, action_line, strlen(action_line), 0);
      close_logfile(logfile);
      return;
    } else {
      printf("Ação não reconhecida, insira 'help' para assistência\n");
      continue;
    }
  }
  return;
}

/* Se o limite de operações ainda não tiver sido atingido, cria uma nova
* operação identificada pelo valor atual de op_counter e com os dados passados em
* argumento, escrevendo a mesma no buffer de memória partilhada entre main e restaurantes.
* Imprime o id da operação e incrementa o contador de operações op_counter.
*/
void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
  if(*op_counter < data->max_ops){
    int client_id;
    printf("Insira o id do cliente: ");
    scanf("%d", &client_id);
    
    int rest_id;
    printf("Insira o id do restaurante: ");
    scanf("%d", &rest_id);
    
    char *dish = calloc(20, sizeof(char));
    printf("Escreva o nome do prato (sem espaços): ");
    scanf("%s", dish);

    struct operation op;
    op.id = *op_counter;
    op.requested_rest = rest_id;
    op.requesting_client = client_id;
    op.requested_dish = calloc(20, sizeof(char));
    strcpy(op.requested_dish, dish);
    op.status = 'I';
    // Beginning Results Mutual Exclusivity
    // -- Waiting for permission to write
    semaphore_mutex_lock(sems->results_mutex);
    // -- Writing
    data->results[*op_counter] = op;
    // -- Posting the lock
    semaphore_mutex_unlock(sems->results_mutex);
    // Results Mutual Exclusivity Operation Finished

    // begin production
    produce_begin(sems->main_rest);
    write_main_rest_buffer(buffers->main_rest, data->buffers_size, &op);
    produce_end(sems->main_rest);
    // production finished

    printf("O processo #%d foi criado!", *op_counter);
    *op_counter += 1;
    char cli[3];
    itoa(client_id, cli, 10);
    char rest[3];
    itoa(rest_id, rest, 10);

    FILE* logfile = open_logfile(data->log_filename);
    log(logfile, "request", strlen("request"), 3, cli, rest, dish);
    close_logfile(logfile);
  
    free(dish);
  }
}

/* Função que lê um id de operação do utilizador e verifica se a mesma
* é valida. Em caso afirmativo,
* imprime informação da mesma, nomeadamente o seu estado, o id do cliente
* que fez o pedido, o id do restaurante requisitado, o nome do prato pedido
* e os ids do restaurante, motorista, e cliente que a receberam e processaram.
*/
void read_status(struct main_data* data, struct semaphores* sems){
  int op_id; printf("Introduza o id da operação: "); scanf("%d", &op_id);
  if(op_id < data->max_ops && op_id >= 0){
    // check if it exists
      // results mutex begin
      semaphore_mutex_lock(sems->results_mutex);
      struct operation op = data->results[op_id];
      semaphore_mutex_unlock(sems->results_mutex);
      // results mutex end
      if(op.status != NULL){
        printf("Pedido %d com estado %c requisitado pelo cliente %d ao restaurante %d com o prato %s, ", op.id, op.status, op.requesting_client, op.requested_rest, op.requested_dish);
        switch(op.status){
          case 'I' : printf("ainda não foi recebido no restaurante!\n"); break;
          case 'R' : printf("foi tratado pelo restaurante %d, ainda não foi encaminhado para um motorista!\n", op.receiving_rest); break;
          case 'D' : printf("foi tratado pelo restaurante %d, encaminhado pelo motorista %d, mas ainda não foi recebido pelo cliente!", op.receiving_rest, op.receiving_driver); break;
          case 'C' : printf("foi tratado pelo restaurante %d, encaminhado pelo motorista %d, e enviado ao cliente %d!\n", op.receiving_rest, op.receiving_driver, op.receiving_client); break;
        }
      } else {
        printf("O pedido %d ainda não é valido!\n", op_id);
      }
  } else {
    printf("O id fornecido não é valido!\n");
  }
  char oper[3];
  itoa(op_id, oper, 10);
  FILE* logfile = open_logfile(data->log_filename);
  log(logfile, "status", strlen("status"), 0, oper);
  close_logfile(logfile);
}

/* Função que termina a execução do programa MAGNAEATS. Deve começar por
* afetar a flag data->terminate com o valor 1. De seguida, e por esta
* ordem, deve esperar que os processos filho terminem, deve escrever as
* estatisticas finais do programa, e por fim libertar
* as zonas de memória partilhada e dinâmica previamente
* reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
*/
void stop_execution(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems){
  *(data->terminate) = 1;

  printf("Terminando o MAGNAEATS!\n");
  wakeup_processes(data, sems);
  wait_processes(data);
  printf("Imprimindo estatísticas:\n");
  write_statistics(data);
  destroy_memory_buffers(data, buffers);
  destroy_semaphores(sems);
}

/* Função que espera que todos os processos previamente iniciados terminem,
* incluindo restaurantes, motoristas e clientes. Para tal, pode usar a função
* wait_process do process.h.
*/
void wait_processes(struct main_data* data){
  int n;
  for(n = 0; n < data->n_restaurants; n++) data->restaurant_stats[n] =  wait_process(data->restaurant_pids[n]); // waits for the restaurants
  for(n = 0; n < data->n_drivers; n++) data->driver_stats[n] =  wait_process(data->driver_pids[n]);  // waits for the drivers
  for(n = 0; n < data->n_clients; n++) data->client_stats[n] = wait_process(data->client_pids[n]);  // waits for the clients
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

  printf("Estatísticas mais detalhadas podem ser encontradas no ficheiro de estatísticas %s\n", data->statistics_filename);
}

/* Função que inicializa os semáforos da estrutura semaphores. Semáforos
* *_full devem ser inicializados com valor 0, semáforos *_empty com valor
* igual ao tamanho dos buffers de memória partilhada, e os *_mutex com
* valor igual a 1. Para tal pode ser usada a função semaphore_create.*/
void create_semaphores(struct main_data* data, struct semaphores* sems){
  // semaphores for main_rest buffer
  sems->main_rest->mutex = semaphore_create(STR_SEM_MAIN_REST_MUTEX, 1);
  sems->main_rest->full = semaphore_create(STR_SEM_MAIN_REST_FULL, 0);
  sems->main_rest->empty = semaphore_create(STR_SEM_MAIN_REST_EMPTY, data->buffers_size);
  // semaphores for rest_driv buffer
  sems->rest_driv->mutex = semaphore_create(STR_SEM_REST_DRIV_MUTEX, 1);
  sems->rest_driv->full = semaphore_create(STR_SEM_REST_DRIV_FULL, 0);
  sems->rest_driv->empty = semaphore_create(STR_SEM_REST_DRIV_EMPTY, data->buffers_size);
  // semaphores for driv_cli buffer
  sems->driv_cli->mutex = semaphore_create(STR_SEM_DRIV_CLI_MUTEX, 1);
  sems->driv_cli->full = semaphore_create(STR_SEM_DRIV_CLI_FULL, 0);
  sems->driv_cli->empty = semaphore_create(STR_SEM_DRIV_CLI_EMPTY, data->buffers_size);
  // semaphore for results
  sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);
}

/* Função que acorda todos os processos adormecidos em semáforos, para que
* estes percebam que foi dada ordem de terminação do programa. Para tal,
* pode ser usada a função produce_end sobre todos os conjuntos de semáforos
* onde possam estar processos adormecidos e um número de vezes igual ao
* máximo de processos que possam lá estar.*/
void wakeup_processes(struct main_data* data, struct semaphores* sems){
  // waking up main_rest sems
  for(int i = 0; i < data->n_restaurants; i++)
    produce_end(sems->main_rest);

  // waking up rest_driv sems
  for(int i = 0; i < data->n_drivers; i++){
    produce_end(sems->rest_driv);
  }

  // waking up driv_cli sems
  for(int i = 0; i < data->n_clients; i++){
    produce_end(sems->driv_cli);
  }
}

/*Função que liberta todos os semáforos da estrutura semaphores. */
void destroy_semaphores(struct semaphores* sems){
  // destroying main_rest sems
  semaphore_destroy(STR_SEM_MAIN_REST_MUTEX, sems->main_rest->mutex);
  semaphore_destroy(STR_SEM_MAIN_REST_FULL, sems->main_rest->full);
  semaphore_destroy(STR_SEM_MAIN_REST_EMPTY, sems->main_rest->empty);
  // destroying rest_driv sems
  semaphore_destroy(STR_SEM_REST_DRIV_MUTEX, sems->rest_driv->mutex);
  semaphore_destroy(STR_SEM_REST_DRIV_FULL, sems->rest_driv->full);
  semaphore_destroy(STR_SEM_REST_DRIV_EMPTY, sems->rest_driv->empty);
  // destroying driv_cli sems
  semaphore_destroy(STR_SEM_DRIV_CLI_MUTEX, sems->driv_cli->mutex);
  semaphore_destroy(STR_SEM_DRIV_CLI_FULL, sems->driv_cli->full);
  semaphore_destroy(STR_SEM_DRIV_CLI_EMPTY, sems->driv_cli->empty);
  // destroying results_mutex sem
  semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);
}

/* Função que liberta todos os buffers de memória dinâmica e partilhada previamente
* reservados na estrutura data.
*/
void destroy_memory_buffers(struct main_data* data, struct communication_buffers* buffers){
  destroy_dynamic_memory(data->restaurant_pids);
  destroy_dynamic_memory(data->driver_pids);
  destroy_dynamic_memory(data->client_pids);
  destroy_dynamic_memory(data->restaurant_stats);
  destroy_dynamic_memory(data->driver_stats);
  destroy_dynamic_memory(data->client_stats);
  destroy_shared_memory(STR_SHM_MAIN_REST_PTR, buffers->main_rest->ptrs, data->buffers_size * sizeof(int));
  destroy_shared_memory(STR_SHM_MAIN_REST_BUFFER, buffers->main_rest->buffer, data->buffers_size * sizeof(struct operation));
  destroy_shared_memory(STR_SHM_REST_DRIVER_PTR, buffers->rest_driv->ptrs, data->buffers_size * sizeof(struct pointers));
  destroy_shared_memory(STR_SHM_REST_DRIVER_BUFFER, buffers->rest_driv->buffer, data->buffers_size * sizeof(struct operation));
  destroy_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, buffers->driv_cli->ptrs, data->buffers_size * sizeof(int));
  destroy_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER,buffers->driv_cli->buffer, data->buffers_size * sizeof(struct operation));
  destroy_shared_memory(STR_SHM_RESULTS,data->results, data->max_ops * sizeof(struct operation));
  destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));
}

int main(int argc, char* argv[]){
  if(argc < 5){
    printf("Uso: magnaeats max_ops buffers_size n_restaurants n_drivers n_clients\nExemplo: %s 10 10 1 1 1\n", argv[0]);
    exit(-1);
  }

  //init data structures
  struct main_data* data = create_dynamic_memory(sizeof(struct main_data));
  struct communication_buffers* buffers = create_dynamic_memory(sizeof(struct communication_buffers));
  //requests are randomly attributed to their respective restaurant
  //there is no order of delivery
  printf("Spinning up dynamic memory area for communication buffers...\n");
  buffers->main_rest = create_dynamic_memory(sizeof(struct rnd_access_buffer));
  //there must be an available driver to finish the order aka circular buffer
  buffers->rest_driv = create_dynamic_memory(sizeof(struct circular_buffer));
  //drivers come in arbitrary order
  buffers->driv_cli = create_dynamic_memory(sizeof(struct rnd_access_buffer));

  // init semaphore data structure
  struct semaphores* sems = create_dynamic_memory(sizeof(struct semaphores));
  sems->main_rest = create_dynamic_memory(sizeof(struct prodcons));
  sems->rest_driv = create_dynamic_memory(sizeof(struct prodcons));
  sems->driv_cli = create_dynamic_memory(sizeof(struct prodcons));

  //execute main code
  main_args(argc, argv, data);
  printf("Command line arguments siphoned...\n");
  create_dynamic_memory_buffers(data);
  create_shared_memory_buffers(data, buffers);
  create_semaphores(data, sems);
  *(data->terminate) = 0;
  launch_processes(buffers, data, sems);           
  user_interaction(buffers, data, sems);          

  //release memory before terminating
  destroy_dynamic_memory(data);
  destroy_dynamic_memory(buffers->main_rest);
  destroy_dynamic_memory(buffers->rest_driv);
  destroy_dynamic_memory(buffers->driv_cli);
  destroy_dynamic_memory(buffers);
  destroy_dynamic_memory(sems->main_rest);
  destroy_dynamic_memory(sems->rest_driv);
  destroy_dynamic_memory(sems->driv_cli);
  destroy_dynamic_memory(sems);

  return 0;
}
