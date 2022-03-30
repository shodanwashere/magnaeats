#include <stdlib.h>
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
  strcat(name, itoa(getuid()));
  int shm_fd = shm_open(name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
  ftruncate(shm_fd, size);
  void *ptr = mmap(0, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
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
  // TODO
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
  // TODO
}

/* Função que escreve uma operação no buffer de memória partilhada entre os restaurantes
* e os motoristas. A operação deve ser escrita numa posição livre do buffer,
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op){
  // TODO
}


/* Função que escreve uma operação no buffer de memória partilhada entre os motoristas
* e os clientes. A operação deve ser escrita numa posição livre do buffer,
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_driver_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op){
  // TODO
}


/* Função que lê uma operação do buffer de memória partilhada entre a Main
* e os restaurantes, se houver alguma disponível para ler que seja direcionada ao restaurante especificado.
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_main_rest_buffer(struct rnd_access_buffer* buffer, int rest_id, int buffer_size, struct operation* op){
  // TODO
}


/* Função que lê uma operação do buffer de memória partilhada entre os restaurantes e os motoristas,
* se houver alguma disponível para ler (qualquer motorista pode ler qualquer operação).
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op){
  // TODO
}


/* Função que lê uma operação do buffer de memória partilhada entre os motoristas e os clientes,
* se houver alguma disponível para ler dirijida ao cliente especificado. A leitura deve
* ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver
* nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_driver_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op){
  // TODO
}
