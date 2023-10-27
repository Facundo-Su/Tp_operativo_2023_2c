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
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <utils/conexion.h>
#include<readline/readline.h>
#include<commons/txt.h>

t_log* logger_file_system;
t_config* config;
//estructuras de FS
typedef struct{
	char* nombre_archivo;
	uint32_t tamanio_archivo;
	uint32_t bloq_inicial_archivo;
}t_fcb;
typedef struct{
	uint32_t cant_bloq_total;
	uint32_t cant_bloq_swap;
	uint32_t tam_fat_bytes;//(CANT_BLOQUES_TOTAL - CANT_BLOQUES_SWAP)*sizeof(uint32_t).
	uint32_t entrada;
}t_fat;
typedef struct archivo_bloques{
	char* ruta_archivo;
	t_list* lista_bloques;

};
char* ruta_fcbs;
int conexion_memoria;
char *ip_memoria;
char *puerto_memoria;
char *puerto_escucha;

int abrir_archivo_fcb(char*);
void crear_archivo_fcb(char*);
void truncar_archivo_fcb(t_fcb*);

void obtener_configuracion();
void terminar_programa();
t_config* iniciar_config();
int iniciar_servidor_file_system(char*);
void iniciar_consola();
void iterator(char* value);

#endif /* SRC_FILESYSTEM_H_ */
