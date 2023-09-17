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
#include <semaphore.h>
#include <commons/collections/queue.h>

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



//ruta de archivo test
//variable global
char * ruta_archivo_test = "./test/";

int contador_pid =0;
int contador_pc =0;


//configuraciones del archibo kernel.config

char *ip_memoria;
char *ip_filesystem;
char *ip_cpu;
char *puerto_fileSystem;

t_log *loggerConsola;
t_list* lista_pcb;
t_queue* cola_new;
t_queue* cola_ready;

sem_t mutex_cola_new;
sem_t mutex_cola_ready;
//sem_t sem_new;
//sem_t sem_ready;
t_planificador tipoPlanificador;
sem_t gradoMultiprogramacion;

char *puerto_memoria;
char *puerto_filesystem;
char *puerto_cpu_dispatch;
char *puerto_cpu_interrupt;
int quantum;
int grado_multiprogramacion_ini;
t_planificador planificador;

char **recursos;
int *instancias_recursos;

void obtenerConfiguracion();
void asignarAlgoritmo(char *algoritmo);
int* string_to_int_array(char** array_de_strings);
void paquete(int);
void mostrar_paquete(t_paquete*);


void iterator(char*);
void interactuarConModulo();
t_config* iniciar_config();
void iniciarConsola();
void iniciarProceso(char*,int,t_planificador);
void finalizarProceso(int);
void iniciarPlanificacion();
void detenerPlanificacion();
void modificarGradoMultiprogramacion();
void listarProcesoPorEstado();
void generarConexion();
void enviarMensaje();

bool controladorMultiProgramacion();



t_contexto_ejecucion* obtenerContexto(char*);
void mandarAMemoria(char* , int , int );
void liberarMemoriaPcb(t_pcb*);
int buscarPosicionQueEstaElPid(int );

void agregarAColaNew(t_pcb* pcb);
t_pcb* quitarDeColaNew();
void agregarAColaReady(t_pcb* pcb);
t_pcb* quitarDeColaReady();
void planificadorLargoPlazo();

#endif /* KERNEL_H_ */

