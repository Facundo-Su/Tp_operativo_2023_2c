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
	//procesos que tienen abierto el archivo
	uint32_t cant_aperturas;
}t_fcb;
typedef struct{
	int tamanio_fat;
	uint32_t* entradas;
}t_fat;
typedef struct {
	uint32_t * datos;
}t_archivo_bloques;
typedef struct {
	uint32_t pid_proceso;
	t_list* bloq_asignados;
}t_bloq_asignados;
typedef struct{
	bool libre;
	uint32_t valor;
}t_swap;
typedef struct{
	t_list* fcb_list;
	t_fat *fat;
	t_archivo_bloques *bloques;
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
t_fat* inicializar_fat();
t_archivo_bloques* inicializar_boques();
void inicializar_fcb();
void inicializar_fs();

void crear_archivo_bloque();
void crear_archivo_fcb(char*nombre,t_fcb* fcb_creado);
int abrir_archivo_fcb(char*);
void truncar_archivo_fcb(t_fcb*);
void* connection_handler(void* socket_conexion);
void ampliar_tam_archivo(t_fcb* fcb_para_modif,int tamanio_nuevo);
void guardar_tam_fcb(t_fcb* fcb);
void reducir_tam_archivo(t_fcb *fcb_para_modif,int nuevo_tamanio);
t_list* iniciar_proceso(uint32_t cant_bloques);

void obtener_configuracion();
void terminar_programa();
t_config* iniciar_config();
int iniciar_servidor_file_system(char*);
void iniciar_consola();
void iterator(char* value);
void * procesar_conexion(int);

#endif /* SRC_FILESYSTEM_H_ */
