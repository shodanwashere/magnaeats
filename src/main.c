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
#include "memory.h"
#include "process.h"

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
void launch_processes(struct communication_buffers* buffers, struct main_data* data){
  int n;
  int nrest = data->n_restaurants;
  int ndriv = data->n_drivers;
  int ncli  = data->n_clients;
  
  // initiating restaurants
  for(n = 0; n < nrest; n++){
    printf("Starting restaurant no. %d\n", n);
    data->restaurant_pids[n] = launch_restaurant(n, buffers, data);
  }
  // initiating drivers
  for(n = 0; n < ndriv; n++){
    printf("Starting driver no. %d\n", n);
    data->driver_pids[n] = launch_driver(n, buffers, data);
  }
  // initiating clients
  for(n = 0; n < ncli; n++){
    printf("Starting client no. %d\n", n);
    data->client_pids[n] = launch_client(n, buffers, data);
  }
}

/* Função que faz interação do utilizador, podendo receber 4 comandos:
* request - cria uma nova operação, através da função create_request
* status - verifica o estado de uma operação através da função read_status
* stop - termina o execução do MAGNAEATS através da função stop_execution
* help - imprime informação sobre os comandos disponiveis
*/
void user_interaction(struct communication_buffers* buffers, struct main_data* data){
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
      create_request(&op_n, buffers, data);
      continue;
    } else if(strcmp(action_line, "status") == 0){
      read_status(data);
      continue;
    } else if(strcmp(action_line, "help") == 0){
      printf("%s", help_msg);
      continue;
    } else if(strcmp(action_line, "stop") == 0){
      stop_execution(data, buffers);
      continue;
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
void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data){
  if(*op_counter < data->max_ops){
    int client_id;
    printf("Insira o id do cliente: ");
    scanf("%d", &client_id);
    printf("Confirmado! client_id :: %d\n", client_id);
    
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
    data->results[*op_counter] = op;
    write_main_rest_buffer(buffers->main_rest, data->buffers_size, &op);
    printf("O processo #%d foi criado!", *op_counter);
    *op_counter += 1;
  }
}

/* Função que lê um id de operação do utilizador e verifica se a mesma
* é valida. Em caso afirmativo,
* imprime informação da mesma, nomeadamente o seu estado, o id do cliente
* que fez o pedido, o id do restaurante requisitado, o nome do prato pedido
* e os ids do restaurante, motorista, e cliente que a receberam e processaram.
*/
void read_status(struct main_data* data){
  int op_id; printf("Introduza o id da operação: "); scanf("%d", &op_id);
  if(op_id < data->max_ops && op_id >= 0){
    // check if it exists
      struct operation op = data->results[op_id];
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
    printf("O id fornecido não é valido!\n", op_id);
  }
}

/* Função que termina a execução do programa MAGNAEATS. Deve começar por
* afetar a flag data->terminate com o valor 1. De seguida, e por esta
* ordem, deve esperar que os processos filho terminem, deve escrever as
* estatisticas finais do programa, e por fim libertar
* as zonas de memória partilhada e dinâmica previamente
* reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
*/
void stop_execution(struct main_data* data, struct communication_buffers* buffers){
  *(data->terminate) = 1;

  printf("Terminando o MAGNAEATS!\n");
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

  //execute main code
  main_args(argc, argv, data);
  printf("Command line arguments siphoned...\n");
  create_dynamic_memory_buffers(data);         // TODO
  create_shared_memory_buffers(data, buffers); // TODO
  *(data->terminate) = 0;
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
