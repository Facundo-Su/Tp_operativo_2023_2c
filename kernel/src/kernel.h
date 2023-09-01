#ifndef KERNEL_
#define KERNEL_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils.h"

t_log* logger;
t_log* logger_obligatorio;
t_config* config;

//estructura de algoritmo

typedef enum{
	FIFO,
	ROUND_ROBIN,
	PRIORIDADES
}t_algoritmo;

//configuraciones del archibo kernel.config

char *ip_memoria;
char *ip_filesystem;
char *ip_cpu;

//variable enum , uso enum porque esta fijo por el enunciado que el tp que solo tenemos
//que implementar 3 algoritmos
t_algoritmo algoritmo_planificacion;

// variables de tipo Numérico
int puerto_memoria;
int puerto_filesystem;
int puerto_cpu_dispatch;
int puerto_cpu_interrupt;
int quantum;
int grado_multiprogramacion_ini;

// variables de tipo Lista (supongamos que se refiere a un arreglo)
char **recursos;
int *instancias_recursos;

void obtenerConfig();
bool generarConexion(t_log*,int*,int*,int*);


#endif
