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
int conexion_memoria;
int conexion_file_system;
int conexion_cpu;

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
char *puerto_memoria;
char *puerto_filesystem;
char *puerto_cpu_dispatch;
char *puerto_cpu_interrupt;
int quantum;
int grado_multiprogramacion_ini;

// variables de tipo Lista (supongamos que se refiere a un arreglo)
char **recursos;
int *instancias_recursos;

void obtenerConfiguracion();
bool generarConexion(t_log*,int*,int*,int*);
void asignarAlgoritmo(char *algoritmo);
int* string_to_int_array(char** array_de_strings);
void paquete(int);
void terminar_programa();
#endif /* KERNEL_H_ */

