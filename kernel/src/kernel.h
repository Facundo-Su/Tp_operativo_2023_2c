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
int conexion_cpu_interrupt;
char ** recursos_config;
char ** instancias_recursos_config;
//estructura de algoritmo
bool detener;

typedef enum{
	FIFO,
	ROUND_ROBIN,
	PRIORIDADES
}t_planificador;

typedef struct{
	t_pcb * pcb;
	int tiempo;
}t_datos_sleep;

//ruta de archivo test
//variable global
char * ruta_archivo_test = "./test/";

int contador_pid =0;
int contador_pc =0;

typedef struct{
    char* nombre;
    int instancias;
    t_queue* cola_bloqueados;
}t_recurso;

typedef struct{
    char* nombre;
    int instancias;
    int pid;
}t_recurso_pcb;

typedef struct{
	int nro_pag;
	int pid_enviar;
	op_code operacion;
	int cliente_fd;
	t_pcb *pcb_remplazo;
}t_page_fault;

typedef struct{
	char* nombre_archivo;
	bool lock_escritura_activo;
	int contador_lectura;
	pthread_rwlock_t lock;
	t_queue* cola_bloqueados;
}t_archivo;

typedef struct{
	char*nombre;
	int puntero;
	int tamanio;
	char * modo;
}t_archivo_pcb;

t_list * tabla_archivo_general;

t_list *lista_recursos_pcb;
//configuraciones del archibo kernel.config

char *ip_memoria;
char *ip_filesystem;
char *ip_cpu;
char *puerto_fileSystem;
t_queue * list_bloqueado_page_fault;
t_log *logger_consola;
t_list* lista_pcb;
t_queue* cola_new;
t_queue* cola_ready;
t_list* lista_sleep;
t_queue* cola_ejecucion;
t_list *lista_recursos;
t_queue * cola_bloqueado_fs;
t_list * pcb_en_ejecucion;



//SEMAFORO=================================================================================================
sem_t mutex_cola_new;
sem_t mutex_cola_ready;
sem_t mutex_lista_ejecucion;
sem_t contador_ejecutando_cpu;
//sem_t sem_new;
//sem_t sem_ready;
t_planificador tipo_planificador;
sem_t grado_multiprogramacion;
sem_t contador_agregando_new;
sem_t contador_cola_ready;
sem_t proceso_desalojo;
sem_t cont_detener_planificacion;
sem_t contador_bloqueado_fs_fopen;
pthread_t deadlock_hilo;
sem_t sem_deadlock;

char *puerto_memoria;
char *puerto_filesystem;
char *puerto_cpu_dispatch;
char *puerto_cpu_interrupt;
int quantum;
int grado_multiprogramacion_ini;
t_planificador planificador;
bool detener;

int *instancias_recursos;
int tam_archivo;

void obtener_configuracion();
void asignar_algoritmo(char *algoritmo);
int* string_to_int_array(char** array_de_strings);
void paquete(int);
void mostrar_paquete(t_paquete*);
char * listar_procesos(t_queue *);
char * listar_procesos_bloqueados();
void deteccion_automatica();
void iterator(char*);
void interactuar_con_modulo();
t_config* iniciar_config();
void iniciar_consola();
void iniciar_proceso(char*,int,int,int);
void finalizar_proceso(int);
void terminar_proceso(t_pcb*);
void iniciar_planificacion();
void detener_planificacion();
void modificar_grado_multiprogramacion();
void listar_proceso_por_estado();
void generar_conexion();
void enviar_mensaje();
void * ejecutar_sleep(void *);
void terminar_proceso(t_pcb * );
bool controlador_multiprogramacion();

time_t start_time, end_time;
double elapsed_time;
t_pcb * buscar_lista(int ,t_list *);
void ejecutar_fclose(char* , t_pcb* );
bool archivo_abierto_para_escritura(t_archivo* , t_pcb* );
void eliminar_entrada_pcb (t_pcb * , char * );
bool archivo_abierto_para_lectura(t_archivo* , t_pcb* );
void eliminar_entrada_pcb (t_pcb * , char * );
t_contexto_ejecucion* obtener_contexto(char*);
void mandar_a_memoria(char* , int , int );
void liberar_memoria_pcb(t_pcb*);
int buscar_posicion_pid(int );
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
void detener_planificacion_corto_largo();
void deadlock();
void mostrar_recursos_pcb(int);
t_pcb * buscar_pcb_colas(int,t_queue*);
t_pcb*buscar_pcb_bloqueados(int );
t_pcb * encontrar_pcb(int );
void envio_page_fault_a_memoria(t_page_fault* );
char* estado_a_string(t_estado );
void ejecutar_fopen(char*, char*, t_pcb *);
void enviar_truncate_fs(char * , int );
t_archivo * buscar_en_tabla_archivo_general(char*);
char * listar_procesos_bloqueados_archivos();


#endif /* KERNEL_H_ */



