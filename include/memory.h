#ifndef MEMORY_H_GUARD
#define MEMORY_H_GUARD

#define STR_SHM_MAIN_REST_PTR 			"SHM_MAIN_REST_PTR"
#define STR_SHM_MAIN_REST_BUFFER 		"SHM_MAIN_REST_BUFFER"
#define STR_SHM_REST_DRIVER_PTR 		"SHM_REST_DRIVER_PTR"
#define STR_SHM_REST_DRIVER_BUFFER 		"SHM_REST_DRIVER_BUFFER"
#define STR_SHM_DRIVER_CLIENT_PTR 		"SHM_DRIVER_CLIENT_PTR"
#define STR_SHM_DRIVER_CLIENT_BUFFER 	"SHM_DRIVER_CLIENT_BUFFER"
#define STR_SHM_RESULTS					"SHM_RESULTS"
#define STR_SHM_TERMINATE				"SHM_TERMINATE"

struct pointers { 																					
	int in;  																						
	int out; 																						
};																								

//estrutura que representa um buffer circular
struct circular_buffer { 	
	struct pointers *ptrs;
	struct operation* buffer;
};

//estrutura que representa um buffer de acesso aleatório
struct rnd_access_buffer {
	int* ptrs;
	struct operation* buffer;
};


//Estrutura que representa uma operação (pedido/resposta)
struct operation {
	int id; 					//id da operação
	int requested_rest;			//id do restaurante requisitado
	int requesting_client;		//id do cliente que fez o pedido
	char* requested_dish;		//nome do(s) prato(s) pedido(s)
	
	char status;				//estado da operação
	int receiving_rest;			//id do restaurante que recebeu pedido
	int receiving_driver;		//id do motorista que fez entrega
	int receiving_client;		//id do cliente que recebeu a encomenda

	struct timespec start_time; // quando o pedido foi criado
	struct timespec rest_time;  // quando o restaurante recebeu o pedido
	struct timespec driver_time;// quando o motorista recebeu o pedido
	struct timespec client_end_time;     // quando o cliente recebeu e finalizou o pedido
};


//estrutura que agrega os shared memory buffers necessários para comunicação entre processos
struct communication_buffers {
	struct rnd_access_buffer* main_rest; 		//buffer para main enviar pedidos a restaurantes
	struct circular_buffer* rest_driv;	//buffer para restaurantes encaminharem pedidos a motoristas
	struct rnd_access_buffer* driv_cli;  		//buffer para motoristas entregarem pedidos aos clientes
};


/* Função que reserva uma zona de memória partilhada com tamanho indicado
* por size e nome name, preenche essa zona de memória com o valor 0, e 
* retorna um apontador para a mesma. Pode concatenar o resultado da função
* getuid() a name, para tornar o nome único para o processo.
*/
void* create_shared_memory(char* name, int size);


/* Função que reserva uma zona de memória dinâmica com tamanho indicado
* por size, preenche essa zona de memória com o valor 0, e retorna um 
* apontador para a mesma.
*/
void* create_dynamic_memory(int size);


/* Função que liberta uma zona de memória dinâmica previamente reservada.
*/
void destroy_shared_memory(char* name, void* ptr, int size);


/* Função que liberta uma zona de memória partilhada previamente reservada.
*/
void destroy_dynamic_memory(void* ptr);


/* Função que escreve uma operação no buffer de memória partilhada entre a Main
* e os restaurantes. A operação deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_main_rest_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op);


/* Função que escreve uma operação no buffer de memória partilhada entre os restaurantes
* e os motoristas. A operação deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op);


/* Função que escreve uma operação no buffer de memória partilhada entre os motoristas
* e os clientes. A operação deve ser escrita numa posição livre do buffer, 
* tendo em conta o tipo de buffer e as regras de escrita em buffers desse tipo.
* Se não houver nenhuma posição livre, não escreve nada.
*/
void write_driver_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op);


/* Função que lê uma operação do buffer de memória partilhada entre a Main
* e os restaurantes, se houver alguma disponível para ler que seja direcionada ao restaurante especificado.
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_main_rest_buffer(struct rnd_access_buffer* buffer, int rest_id, int buffer_size, struct operation* op);


/* Função que lê uma operação do buffer de memória partilhada entre os restaurantes e os motoristas,
* se houver alguma disponível para ler (qualquer motorista pode ler qualquer operação).
* A leitura deve ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo.
* Se não houver nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op);


/* Função que lê uma operação do buffer de memória partilhada entre os motoristas e os clientes,
* se houver alguma disponível para ler dirijida ao cliente especificado. A leitura deve
* ser feita tendo em conta o tipo de buffer e as regras de leitura em buffers desse tipo. Se não houver
* nenhuma operação disponível, afeta op->id com o valor -1.
*/
void read_driver_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op);


#endif
