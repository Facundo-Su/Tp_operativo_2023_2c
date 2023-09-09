#ifndef KERNEL_H_
#define KERNEL_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <utils/conexion.h>
#include <pthread.h>

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
}t_planificador;

typedef struct{
	uint32_t ax;
	uint32_t bx;
	uint32_t cx;
	uint32_t dx;
}t_registro_cpu;

typedef struct{
	int pc;
	t_registro_cpu registros_cpu;
}t_contexto_ejecucion;

typedef enum{
	NEW,
	READY,
	RUNNING,
	WAITING,
	TERMINATED,
}t_estado;



typedef struct{
	int pid;
	int prioridad;
	t_contexto_ejecucion* contexto;
	t_registro_cpu registro_cpu;
	t_list* tabla_archivo_abierto;
	t_estado estado;
}t_pcb;

//ruta de archivo test
char * ruta_archivo_test = "./test/";

int contador_pid =0;
int contador_pc =0;
//configuraciones del archibo kernel.config

char *ip_memoria;
char *ip_filesystem;
char *ip_cpu;
char *puerto_fileSystem;


// variables de tipo Numérico
char *puerto_memoria;
char *puerto_filesystem;
char *puerto_cpu_dispatch;
char *puerto_cpu_interrupt;
int quantum;
int grado_multiprogramacion_ini;
t_planificador planificador;

// variables de tipo Lista (supongamos que se refiere a un arreglo)
char **recursos;
int *instancias_recursos;

void obtenerConfiguracion();
void asignarAlgoritmo(char *algoritmo);
int* string_to_int_array(char** array_de_strings);
void paquete(int);
void mostrar_paquete(t_paquete*);
//prueba
void iterator(char*);
void interactuarConModulo();
t_config* iniciar_config();

void iniciarConsola();
void iniciarProceso(char*,int,t_planificador);
void finalizarProceso(t_log*);
void iniciarPlanificacion(t_log*);
void detenerPlanificacion(t_log*);
void modificarGradoMultiprogramacion(t_log*);
void listarProcesoPorEstado(t_log*);


t_contexto_ejecucion* obtenerContexto(char*);
void mandarAMemoria(char*,int );


#endif /* KERNEL_H_ */

