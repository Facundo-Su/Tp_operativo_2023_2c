/*
 * filesystem.h
 *
 *  Created on: Sep 2, 2023
 *      Author: utnso
 */

#ifndef SRC_FILESYSTEM_H_
#define SRC_FILESYSTEM_H_

#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <utils/conexion.h>
#include<readline/readline.h>
#include<commons/txt.h>
#include<fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <dirent.h>

#define RESERV_BOOT UINT32_MAX;
#define MARCA_ASIG UINT32_MAX;
#define EOFF UINT32_MAX;
t_log* logger_file_system;
t_config* config_file_system;
//estructuras de FS
typedef struct{
	char* nombre_archivo;
	uint32_t tamanio_archivo;
	uint32_t bloq_inicial_archivo;
}t_fcb;

typedef struct{
	int tamanio_fat;
	uint32_t* entradas;
}t_fat;

typedef struct{
	bool libre;
	uint32_t valor;
}t_swap;
typedef struct{
	t_list* fcb_list;
	t_fat *fat;
	void *bloques;
	t_swap* array_swap;
}t_FS;

t_FS *fs;
char* ruta_fcbs;
int conexion_memoria;
char *ip_memoria;
char *puerto_memoria;
char *puerto_escucha;
char* ruta_fat;
char* ruta_bloques;
int tam_bloque;
int cant_total_bloq;
int cant_bloq_swap;
int retardo_acceso_bloq;
int retardo_acceso_fat;


//metodos de FS
void inicializar_fat();

void inicializar_fcb();
void inicializar_fs();


t_fcb* devolver_fcb(char* nombre);
char* recibir_nombre_archivo(int socket_cliente);

void crear_archivo_fcb(char*nombre);
int abrir_archivo_fcb(char*);
void truncar_archivo(char*nombre,int tamanio);
void escribir_fcb_en_archivo(t_fcb *fcb);
//swap
void escribir_bloque_swap(int puntero,void *a_escribir);
void asignar_bloques_swap(t_list *bloques_asignados, int cant_bloques);
int buscar_bloq_libre_swap();
void finalizar_proceso(t_list *lista_liberar);
t_list* iniciar_proceso(int cant_bloques);
void reemplazar_bloq_swap(int num_bloque,void *a_escribir) ;

//fat
void truncar_archivo(char *nombre, int nuevo_tamanio_bytes);
void levantar_fat() ;
void levantar_fcbs();
void* leer_archivo_bloques_fat(int puntero, char *nombre);
int obtener_ultimo_bloq_fcb(t_fcb* fcb);
uint32_t obtener_bloque_por_indice(t_fcb *fcb, uint32_t indice_bloque);
void reducir_tam_archivo(t_fcb* fcb_para_modif,int nuevo_tam);
void ampliar_tam_archivo(t_fcb *fcb, int tamanio_nuevo_bytes);
void asignar_entradas_fat(t_fcb *fcb_a_guardar);
void enviar_respuesta_truncar(int socket_cliente);
void escribir_bloque_fat(int puntero, char* nombre,void* a_escribir);
//bloques
void levantar_archivo_bloques();
void* leer_bloque_swap(int );
void enviar_bloque_para_memoria(void* ,int );

uint32_t buscar_entrada_libre_fat();
//respuestas conxiones
void enviar_tamanio_archivo(int tamanio, int cliente_fd);
void enviar_respuesta_crear_archivo(int);

int prueba;

void obtener_configuracion();
void terminar_programa();

t_config* iniciar_config();
void* connection_handler(void* socket_conexion);
int iniciar_servidor_file_system(char*);

void * procesar_conexion(void*);

#endif /* SRC_FILESYSTEM_H_ */
