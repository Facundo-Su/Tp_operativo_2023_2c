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
<<<<<<< HEAD
int conexion_cpu_interrupt;
=======

>>>>>>> parent of ca1d82c (borre todo para mergear rama)
//estructura de algoritmo

typedef enum{
	FIFO,
	ROUND_ROBIN,
	PRIORIDADES
}t_planificador;

<<<<<<< HEAD

=======
bool hay_proceso_en_ejecucion;
int prioridad_de_proceso_en_ejecucion;
>>>>>>> parent of ca1d82c (borre todo para mergear rama)

//ruta de archivo test
//variable global
char * ruta_archivo_test = "./test/";

int contador_pid =0;
int contador_pc =0;

typedef struct{
    char* nombre;
    int instancias;
    t_queue* cola_bloqueados;
<<<<<<< HEAD
}t_recurso;

typedef struct{
    char* nombre;
    int instancias;
    int pid;
}t_recurso_pcb;

t_list *lista_recursos_pcb;
=======
    sem_t sem_recurso;
}t_recurso;

>>>>>>> parent of ca1d82c (borre todo para mergear rama)
//configuraciones del archibo kernel.config

char *ip_memoria;
char *ip_filesystem;
char *ip_cpu;
char *puerto_fileSystem;
<<<<<<< HEAD
t_list * lista_bloqueados;
=======

>>>>>>> parent of ca1d82c (borre todo para mergear rama)
t_log *logger_consola;
t_list* lista_pcb;
t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_ejecucion;
<<<<<<< HEAD
t_list *lista_recursos;

t_list * pcb_en_ejecucion;


//SEMAFORO=================================================================================================
sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t mutex_lista_ejecucion;
=======

sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t mutex_cola_ejecucion;
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
sem_t contador_ejecutando_cpu;
//sem_t sem_new;
//sem_t sem_ready;
t_planificador tipo_planificador;
sem_t grado_multiprogramacion;
<<<<<<< HEAD
sem_t contador_agregando_new;
sem_t contador_cola_ready;
sem_t proceso_desalojo;



=======
>>>>>>> parent of ca1d82c (borre todo para mergear rama)

char *puerto_memoria;
char *puerto_filesystem;
char *puerto_cpu_dispatch;
char *puerto_cpu_interrupt;
int quantum;
int grado_multiprogramacion_ini;
t_planificador planificador;

<<<<<<< HEAD
=======
char **recursos;
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
int *instancias_recursos;

void obtener_configuracion();
void asignar_algoritmo(char *algoritmo);
int* string_to_int_array(char** array_de_strings);
void paquete(int);
void mostrar_paquete(t_paquete*);


void iterator(char*);
void interactuar_con_modulo();
t_config* iniciar_config();
void iniciar_consola();
void iniciar_proceso(char*,int,int,int);
void finalizar_proceso(int);
void iniciar_planificacion();
void detener_planificacion();
void modificar_grado_multiprogramacion();
void listar_proceso_por_estado();
void generar_conexion();
void enviar_mensaje();

bool controlador_multiprogramacion();

time_t start_time, end_time;
double elapsed_time;


t_contexto_ejecucion* obtener_contexto(char*);
void mandar_a_memoria(char* , int , int );
void liberar_memoria_pcb(t_pcb*);
int buscar_posicion_pid(int );
<<<<<<< HEAD
=======
t_list *lista_recursos;
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
void agregar_a_cola_new(t_pcb* pcb);
t_pcb* quitar_de_cola_new();
void agregar_a_cola_ready(t_pcb* pcb);
t_pcb* quitar_de_cola_ready();
void agregar_a_cola_ejecucion(t_pcb* pcb);
t_pcb* quitar_de_cola_ejecucion();
void planificador_largo_plazo();
void planificador_corto_plazo();
void de_ready_a_fifo();
void de_ready_a_round_robin();
void de_ready_a_prioridades();
bool comparador_prioridades();
void enviar_contexto_ejecucion(t_contexto_ejecucion * );
<<<<<<< HEAD
t_contexto_ejecucion* crear_contexto();
t_list* obtener_lista_instruccion(char* ruta);
void *manejar_respuesta(void* );
void agregar_recurso_pcb(int , char*);
void quitar_recurso_pcb(int, char*);
t_recurso_pcb*crear_recurso_pcb(char*,int);
void procesar_conexion(void *);
t_recurso_pcb*crear_recurso(char*);
void crear_pcb(int );
t_registro_cpu* crear_registro();
void iniciar_recurso();
void ejecutar_wait(char*,t_pcb*);
t_recurso_pcb* buscar_recurso_pcb(char*,int );
void liberar_recursos(int );
void detect_deadlock();
bool can_allocate(int pid, int work[]);


#endif /* KERNEL_H_ */



=======
void enviar_por_dispatch(t_pcb* pcb);
t_contexto_ejecucion* crear_contexto();
t_list* obtener_lista_instruccion(char* ruta);
void *manejar_respuesta(void* );
t_pcb*agregar_recurso_pcb(t_pcb*, char*);
void procesar_conexion(void *);
t_recurso_pcb*crear_recurso(char*);
void crear_pcb(t_planificador );
t_registro_cpu* crear_registro();

#endif /* KERNEL_H_ */

>>>>>>> parent of ca1d82c (borre todo para mergear rama)
