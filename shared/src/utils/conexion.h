/*
 * conexion.h
 *
 *  Created on: Sep 1, 2023
 *      Author: utnso
 */
#ifndef CONEXION_H_
#define CONEXION_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <strings.h>

typedef enum{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct{
	 int size;
	 void* stream;
}t_buffer;

typedef struct {
	op_code codigo_operacion;
	t_buffer* buffer;
}t_paquete;





int crear_conexion(char *ip, char* puerto);
int realizar_handshakes(char* ip, char* puerto, t_log* logger);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void enviar_mensaje(char* mensaje, int socket_cliente);
int recibir_operacion(int socket_cliente);
void recibir_mensaje(int socket_cliente, t_log* logger);
void* recibir_buffer(int* size, int socket_cliente);
void crear_buffer(t_paquete* paquete);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);
#endif
