#ifndef CONEXION_SERVIDOR_H_
#define CONEXION_SERVIDOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<assert.h>
#include <bits/types.h>
#include<readline/readline.h>

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

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
	t_list* tabla_archivo_abierto;
	t_estado estado;
}t_pcb;

t_log *loggerConsola;
t_list* lista_pcb;


int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void terminar_programa(int , t_log* , t_config* );

//servidores
extern t_log* logger;

void* recibir_buffer(int*, int);

int iniciar_servidor(char*);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
t_config* cargarConfig(char *);



#endif /* CONEXION_SERVIDOR_H_*/
