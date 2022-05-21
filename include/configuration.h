#ifndef CONFIGURATION_H_GUARD
#define CONFIGURATION_H_GUARD

#include "main.h"

/* Função que lê os argumentos a serem utilizados, nomeadamente o número
* máximo de operações, o tamanho dos buffers de memória partilhada
* usados para comunicação, o número de restaurantes, clientes e de motoristas,
* o nome dos ficheiros de log e de estatísticas e a temporização para o alarme.
* Guarda esta informação nos campos apropriados da estrutura main_data.
*/
void read_configfile(char *config, struct main_data* data);

#endif