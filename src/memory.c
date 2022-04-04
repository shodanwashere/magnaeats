/**
 * THIS IS A HEADER COMMENT FOR IN CAMPUS DEVELOPMENT
 * THIS CODE WAS DEVELOPED BY FC56330
 * IF YOU SEE ANOTHER PROJECT WITH A FILE WITH VERY SIMILAR FUNCTION DEFINITIONS BUT WITHOUT
 * THIS HEADER, THAT MEANS THIS CODE WAS PLAGIARISED!
 * 
 */
#define STR_SHM_MAIN_REST_PTR 			"SHM_MAIN_REST_PTR"
#define STR_SHM_MAIN_REST_BUFFER 		"SHM_MAIN_REST_BUFFER"
#define STR_SHM_REST_DRIVER_PTR 		"SHM_REST_DRIVER_PTR"
#define STR_SHM_REST_DRIVER_BUFFER 		"SHM_REST_DRIVER_BUFFER"
#define STR_SHM_DRIVER_CLIENT_PTR 		"SHM_DRIVER_CLIENT_PTR"
#define STR_SHM_DRIVER_CLIENT_BUFFER 	"SHM_DRIVER_CLIENT_BUFFER"
#define STR_SHM_RESULTS					"SHM_RESULTS"
#define STR_SHM_TERMINATE				"SHM_TERMINATE"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include "memory.h"

/* Função que reserva uma zona de memória partilhada com tamanho indicado
* por size e nome name, preenche essa zona de memória com o valor 0, e
* retorna um apontador para a mesma. Pode concatenar o resultado da função
* getuid() a name, para tornar o nome único para o processo.
*/
void *create_shared_memory(char *name, int size){
  char proc_name[50];
  sprintf(proc_name, "/%s%d", name, getuid());
  int shm_fd = shm_open(proc_name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
  ftruncate(shm_fd, size);
  void *ptr = mmap(0, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
  close(shm_fd);
  return ptr;
}

/* Função que reserva uma zona de memória dinâmica com tamanho indicado
* por size, preenche essa zona de memória com o valor 0, e retorna um
* apontador para a mesma.
*/
void *create_dynamic_memory(int size){
  void *ret = malloc(size);
  bzero(ret, size);
  return ret;
}

/* Função que liberta uma zona de memória partilhada previamente reservada.
*/
void destroy_shared_memory(char* name, void* ptr, int size){
  char *proc_name;
  sprintf(proc_name, "/%s%d", name, getuid());
  munmap(ptr, size);
  shm_unlink(name);
}

/* Função que liberta uma zona de memória dinâmica previamente reservada.
*/
void destroy_dynamic_memory(void* ptr){
  free(ptr);
}

/* Função que escreve uma operação no buffer de memória partilhada entre a Main
* e os restaurantes. A operação deve ser escrita numa posição livre do buffer,
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_main_rest_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op){
  int n;
  int* gottem = 0;
  for(n = 0; n < buffer_size && !gottem; n++){
    if(buffer->ptrs[n] == 0){
      buffer->buffer[n] = *op;
      buffer->ptrs[n] = 1;
      gottem = 1;
    }
  }
}

/* Função que escreve uma operação no buffer de memória partilhada entre os restaurantes
* e os motoristas. A operação deve ser escrita numa posição livre do buffer,
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op){
  struct operation next_prod = *op;
  while(((buffer->ptrs->in + 1) % buffer_size) == buffer->ptrs->out)
    ; //buffer is full, waits for it to be advanced by the consumer
  buffer->buffer[buffer->ptrs->in] = next_prod;
  buffer->ptrs->in = (buffer->ptrs->in + 1) % buffer_size;
}


/* Função que escreve uma operação no buffer de memória partilhada entre os motoristas
* e os clientes. A operação deve ser escrita numa posição livre do buffer,
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_driver_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op){
  int n;
  int* gottem = 0;
  for(n = 0; n < buffer_size && !gottem; n++){
    if(buffer->ptrs[n] == 0){
      buffer->buffer[n] = *op;
      buffer->ptrs[n] = 1;
      gottem = 1;
    }
  }
}


/* Função que lê uma operação do buffer de memória partilhada entre a Main
* e os restaurantes, se houver alguma disponível para ler que seja direcionada ao restaurante especificado.
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_main_rest_buffer(struct rnd_access_buffer* buffer, int rest_id, int buffer_size, struct operation* op){
  int n;
  int gottem = 0;
  struct operation *oper;
  for(n = 0; n < buffer_size && !gottem; n++){ // cycle stops when we get an operation for the restaurant or if
    if(buffer->ptrs[n] == 1){                  // there are no operations for the restaurant at all
      oper = &(buffer->buffer[n]);
      gottem = (oper->requested_rest == rest_id) ? 1 : 0;
      if(gottem){
        memcpy(op, oper, sizeof(struct operation));
        buffer->ptrs[n] = 0;
      }
    }
  }
  if(!gottem) op->id = -1;
}


/* Função que lê uma operação do buffer de memória partilhada entre os restaurantes e os motoristas,
* se houver alguma disponível para ler (qualquer motorista pode ler qualquer operação).
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op){
  while(buffer->ptrs->in == buffer->ptrs->out)
  ; // do nothing
  *op = buffer->buffer[buffer->ptrs->out];
  buffer->ptrs->out = (buffer->ptrs->out + 1) % buffer_size;
}


/* Função que lê uma operação do buffer de memória partilhada entre os motoristas e os clientes,
* se houver alguma disponível para ler dirijida ao cliente especificado. A leitura deve
* ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver
* nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_driver_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op){
  int n;
  int gottem = 0;
  struct operation *oper;
  for(n = 0; n < buffer_size && !gottem; n++){ // cycle stops when we get an operation for the restaurant or if
    if(buffer->ptrs[n] == 1){                  // there are no operations for the restaurant at all
      oper = &(buffer->buffer[n]);
      gottem = (oper->requesting_client == client_id) ? 1 : 0;
      if(gottem){
        memcpy(op, oper, sizeof(struct operation));
        buffer->ptrs[n] = 0;
      }
    }
  }
  if(!gottem) op->id = -1;
}
