#ifndef KERNEL_H_
#define KERNEL_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <utils/conexion.h>

t_log* logger;
t_config* config;
t_log * logger_consola_memoria;

typedef enum{
	FIFO,
	LRU
}t_algoritmo;

typedef struct {
    uint32_t pos_en_swap;
    bool P;
    bool M;
} t_marco;

typedef struct {
    t_marco* marcos;
    uint32_t num_marcos;
}t_tabla_paginas;


typedef struct{
	int pid;
	t_list* instrucciones;
}t_instrucciones;



int conexion_filesystem;

t_list *lista_instrucciones;


char *puerto_escucha;
char *ip_file_system;
char *puerto_filesystem;
int tam_memoria;
int tam_pagina;
char *path_instrucciones;
int retraso_respuesta;
t_algoritmo algoritmo;
useconds_t retardo_respuesta;

void iterator(char* value);
void obtener_configuraciones();
void iniciar_servidor_memoria(char*);
void iniciar_consola();
void recibir_estructura_inicial(int);
void procesar_conexion(int);
void iniciar_recursos();
void cargar_lista_instruccion(char*,int ,int,int);
t_list* leer_pseudocodigo(FILE*);
char** parsear_instruccion(char*);
op_instrucciones asignar_cod_instruccion(char* );
bool encontrar_instrucciones(void * );
char* obtener_nombre_instruccion(op_instrucciones ) ;
void enviar_tam_pagina(int  , int );
enviar_registro_leido_mov_in(int  , op_code ,int );


#endif /* KERNEL_H_ */

