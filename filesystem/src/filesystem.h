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

t_log* logger;
t_config* config;
t_log* logger_consola_filesystem;

int conexion_memoria;
char *ip_memoria;
char *puerto_memoria;
char *puerto_escucha;
char * archivo_fat = "archivo_fat.dat";
char * archivo_bloques = "archivo_bloques.dat";

typedef struct{
    char* nombre_archivo;
    int tamanio_archivo;
    int bloque_inicial;
}t_fcb;

void obtener_configuracion();
void terminar_programa();
t_config* iniciar_config();
void iterator(char*);
int iniciar_servidor_file_system(char*);
void iniciar_consola();
int levantar_FAT();
int levantar_archivos_binario(char* ruta);

#endif /* SRC_FILESYSTEM_H_ */
