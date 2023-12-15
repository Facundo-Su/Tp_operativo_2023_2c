#ifndef KERNEL_H_
#define KERNEL_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <utils/conexion.h>
#include <limits.h>

t_log* logger;
t_config* config;
t_log * logger_consola_memoria;
sem_t envio_marco;
sem_t sem_memoria_pf;
sem_t sem_reserva_swap;
int contador_fifo;
void * bloque;
sem_t contador_espera_cargar;
void* datos_obtenidos;
sem_t enviar_pagina;
t_list* lista_swap;



typedef enum{
	FIFO,
	LRU
}t_algoritmo;

typedef struct {
	int num_marco;
    int p;
    int m;
    int pos_en_swap;
    int num_pagina;
} t_pagina;

typedef struct {
	int num_marco;
    int base;
    bool is_free;
    int llegada_fifo;
    int last_time_lru;
    int pid;
} t_marco;

typedef struct {
	int pid;
    t_list * paginas;
    int tamanio_proceso;
    int paginas_necesarias;
}t_tabla_paginas;

typedef struct{
	void * espacio_usuario;
	t_list * marcos;
	t_list * lista_tabla_paginas;
	int tamanio_marcos;
	int cantidad_marcos;
}t_memoria;

t_memoria* memoria;

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
void procesar_conexion(void*);
void iniciar_recursos();
void cargar_lista_instruccion(char*,int ,int,int);
t_list* leer_pseudocodigo(FILE*);
char** parsear_instruccion(char*);
op_instrucciones asignar_cod_instruccion(char* );
bool encontrar_instrucciones(void * );
char* obtener_nombre_instruccion(op_instrucciones ) ;
void enviar_tam_pagina(int  , int );
void liberar_paginas(void *);
t_tabla_paginas * inicializar_paginas(int , int );
void finalizar_proceso(int );
void enviar_registro_leido_mov_in(int  , op_code ,int );
char* obtener_ruta(char* );
t_list * crear_paginas(int );
void enviar_marco(int  , op_code ,int );
bool pagina_esta_en_memoria(int , int );
t_pagina * obtener_pagina(int,int );
void generar_conexion_fs();
void procesar_conexion_prueba(void *);
t_pagina * obtener_pagina_en_marco(int ,int );
int ejecutar_lru();
#endif /* KERNEL_H_ */

