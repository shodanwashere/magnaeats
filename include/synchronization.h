#ifndef SYNCHRONIZATION_H_GUARD
#define SYNCHRONIZATION_H_GUARD

#include <semaphore.h>
	
// Nomes usados na criacao dos semaforos
#define STR_SEM_MAIN_REST_FULL 	"sem_main_rest_full"
#define STR_SEM_MAIN_REST_EMPTY "sem_main_rest_empty"
#define STR_SEM_MAIN_REST_MUTEX "sem_main_rest_mutex"

#define STR_SEM_REST_DRIV_FULL 	"sem_rest_driv_full"
#define STR_SEM_REST_DRIV_EMPTY "sem_rest_driv_empty"
#define STR_SEM_REST_DRIV_MUTEX "sem_rest_driv_mutex"

#define STR_SEM_DRIV_CLI_FULL	"sem_driv_cli_full"
#define STR_SEM_DRIV_CLI_EMPTY 	"sem_driv_cli_empty"
#define STR_SEM_DRIV_CLI_MUTEX 	"sem_driv_cli_mutex"

#define STR_SEM_RESULTS_MUTEX	"sem_results_mutex"

//estrutura de 3 semáforos utilizada no modelo produtor/consumidor
struct prodcons {
	sem_t *full, *empty, *mutex;
};

//estrutura que agrega informação de todos os semáforos necessários pelo socps
struct semaphores {
	struct prodcons *main_rest;	// semáforos para acesso ao buffer entre a main e restaurantes
	struct prodcons *rest_driv; // semáforos para acesso ao buffer entre restaurantes e motoristas
	struct prodcons *driv_cli; 	// semáforos para acesso ao buffer entre motoristas e clientes
	sem_t *results_mutex;		// semáforo para exclusão mútua no acesso ao array de resultados
};


/* Função que cria um novo semáforo com nome name e valor inicial igual a
* value. Pode concatenar o resultado da função getuid() a name, para tornar
* o nome único para o processo.
*/
sem_t * semaphore_create(char* name, int value);

/* Função que destroi o semáforo passado em argumento.
*/
void semaphore_destroy(char* name, sem_t* semaphore);

/* Função que inicia o processo de produzir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_begin(struct prodcons* pc);

/* Função que termina o processo de produzir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_end(struct prodcons* pc);

/* Função que inicia o processo de consumir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_begin(struct prodcons* pc);

/* Função que termina o processo de consumir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_end(struct prodcons* pc);

/* Função que faz wait a um semáforo.
*/
void semaphore_mutex_lock(sem_t* sem);

/* Função que faz post a um semáforo.
*/
void semaphore_mutex_unlock(sem_t* sem);


#endif
