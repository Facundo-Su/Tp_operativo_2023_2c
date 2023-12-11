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
#include <time.h>

typedef enum
{
	MENSAJE,
	PAQUETE,
	ENVIARRUTAPARAINICIAR,
	ENVIARREGISTROCPU,
	FINALIZAR,
	RECIBIR_PCB,
	INICIAR_PROCESO,
	PLANIFICACION,
	EJECUTAR_WAIT,
	EJECUTAR_SIGNAL,
	EJECUTAR_SLEEP,
	EJECUTAR_F_TRUNCATE,
	EJECUTAR_F_OPEN,
	EJECUTAR_F_CLOSE,
	EJECUTAR_F_SEEK,
	EJECUTAR_F_READ,
	EJECUTAR_F_WRITE,
	OBTENER_INSTRUCCION,
	INSTRUCCIONES_A_MEMORIA,
	CPU_ENVIA_A_MEMORIA,// BORRAR
	ENVIAR_DESALOJAR,
	MANDAME_PAGINA,
	ENVIO_MOV_IN,
	ENVIO_MOV_OUT,
	PAGE_FAULT,
	RESERVAR_SWAP,
	OBTENER_MARCO,
	DATOS_SWAP,
	CREAR_ARCHIVO,
	LEER_ARCHIVO,
	ABRIR_ARCHIVO,
	ESCRIBIR_ARCHIVO,
	FINALIZAR_PROCESO,
	TRUNCAR_ARCHIVO,
	OK_PAG_CARGADA,
	TRUNCATE_FS,
	REMPLAZAR_PAGINA,
	OK_FREAD,
	OK_FWRITE,
	RESPUESTA_INICIAR_PROCESO_FS,
	RESPUESTA_CREAR_ARCHIVO,
	RESPUESTA_ABRIR_ARCHIVO,
	RESPUESTA_SOLICITUD_BLOQUES,
	OK_TRUNCAR_ARCHIVO,
	ESCRIBIR_EN_MEMORIA,
	DIRECCION_FISICA,
	RESPUESTA_F_READ,
	RESPUESTA_MOV_IN,
	RESPUESTA_MOV_OUT
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
	uint32_t ax;
	uint32_t bx;
	uint32_t cx;
	uint32_t dx;
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
}t_pcb;


time_t tiempo_inicial, tiempo_final;
double tiempo_transcurrido;

t_log * logger_consola;

typedef enum
{
    SET,
	SUB,
	SUM,
	JNZ,
	SLEEP,
	WAIT,
	SIGNAL,
	MOV_IN,
	MOV_OUT,
	F_OPEN,
	F_CLOSE,
	F_SEEK,
	F_READ,
	F_WRITE,
	F_TRUNCATE,
	EXIT
}op_instrucciones;

typedef struct{
	op_instrucciones nombre;
    t_list* parametros;
}t_instruccion;

int crear_conexion(char* ip, char* puerto);
t_paquete* crear_paquete(op_code);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void terminar_programa(int , t_log* , t_config* );
t_pcb * transformar_en_pcb(t_list* );


//servidores
extern t_log* logger;

void* recibir_buffer(int*, int);

int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
t_config* cargar_config(char *);
void mandar_a_memoria(char* , int , int );
char* recibir_instruccion(int socket_cliente);

//empaquetar pcb
void empaquetar_pcb(t_paquete* , t_pcb* );
void empaquetar_contexto_ejecucion(t_paquete* , t_contexto_ejecucion* );
void empaquetar_registro(t_paquete* , t_registro_cpu* );
void empaquetar_instrucciones(t_paquete* , t_instruccion*);
char* obtener_nombre_instruccion(op_instrucciones );
void enviar_mensaje_instrucciones(char* , int ,op_code );
void enviar_interrupciones(int ,op_code );
void log_pcb_info(t_pcb* );
t_list* desempaquetar_recursos(t_list* ,int* );
void empaquetar_recursos(t_paquete* ,t_list *);
//enviar pcb
void enviar_pcb(t_pcb* , int conexion, op_code);
void empaquetar_tabla_archivo_abierto(t_paquete *  ,t_list * );
t_list* desempaquetar_archivos_abiertos(t_list* , int );


//desempaquetar
t_pcb* desempaquetar_pcb(t_list* );
t_contexto_ejecucion *desempaquetar_contexto(t_list *,int* );
t_registro_cpu * desempaquetar_registros(t_list * ,int* );
t_instruccion * desempaquetar_instrucciones(t_list*);
t_list* desempaquetar_parametros(t_list* ,int );
op_instrucciones convertir_a_op_instrucciones(char* );
void atendiendo_pedido(int);

#endif /* CONEXION_SERVIDOR_H_*/
