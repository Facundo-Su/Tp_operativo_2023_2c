#ifndef KERNEL_H_
#define KERNEL_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <utils/conexion.h>
#include <utils/conexion_servidor.h>
t_log* logger;
t_config* config;

typedef enum{
	FIFO,
	LRU
}t_algoritmo;


char *puerto_escucha;
char *ip_file_system;
char *puerto_filesystem;
int tam_memoria;
int tam_pagina;
char *path_instrucciones;
int retraso_respuesta;
t_algoritmo algoritmo;


void iterator(char* value);
void obtenerConfiguraciones();
int esperandoOperacion(char*);
#endif /* KERNEL_H_ */

