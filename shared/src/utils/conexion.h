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
#include <commons/collections/queue.h>
#include <semaphore.h>

typedef enum
{
	MENSAJE,
	PAQUETE,
	ENVIARRUTAPARAINICIAR,
	ENVIARREGISTROCPU,
	FINALIZAR,
	EJECUTARINSTRUCIONES,
	RECIBIR_PCB,
	INICIAR_PROCESO
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

typedef enum{
	NEW,
	READY,
	RUNNING,
	WAITING,
	TERMINATED,
}t_estado;

// VER SI FUNCA SI FUNCA, CAMBIAR A UINT_32
typedef struct{
	char AX[4];
	char BX[4];
	char CX[4];
	char DX[4];
}t_registro_cpu;

typedef enum{
	AX,
	BX,
	CX,
	DX
}t_estrucutra_cpu;

typedef struct{
	int pc;
	t_registro_cpu* registros_cpu;
}t_contexto_ejecucion;


typedef struct{
	int pid;
	int prioridad;
	t_contexto_ejecucion* contexto;
	t_list* tabla_archivo_abierto;
	t_estado estado;
	t_list* listaInstruciones;
}t_pcb;

t_log * loggerConsola;

typedef enum
{
    SET,
	SUB,
	SUM,
	EXIT
}op_instrucciones;

typedef struct{
	op_instrucciones nombre;
    t_list* parametros;
}t_instruccion;

int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(op_code);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void terminar_programa(int , t_log* , t_config* );
t_pcb * transformarEnPcb(t_list* );


//servidores
extern t_log* logger;

void* recibir_buffer(int*, int);

int iniciar_servidor(char*);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
t_config* cargarConfig(char *);
void mandarAMemoria(char* , int , int );


//empaquetar pcb
void empaquetarPcb(t_paquete* , t_pcb* );
void empaquetarContextoEjecucion(t_paquete* , t_contexto_ejecucion* );
void empaquetarRegistro(t_paquete* , t_registro_cpu* );
void empaquetarInstrucciones(t_paquete* , t_list* );

//enviar pcb
void enviar_Pcb(t_pcb* , int conexion, op_code);


//desempaquetar
t_pcb* desempaquetar_pcb(t_list* );
t_contexto_ejecucion *desempaquetar_contexto(t_list *,int *);
t_registro_cpu * desempaquetar_registros(t_list * ,int *);
t_instruccion * desempaquetar_instrucciones(t_list* ,int* );
t_list* desempaquetar_parametros(t_list* ,int* );
op_instrucciones convertir_a_op_instrucciones(char* );

#endif /* CONEXION_SERVIDOR_H_*/
