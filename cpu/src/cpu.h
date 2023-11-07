/*
 * cpu.h
 *
 *  Created on: Sep 2, 2023
 *      Author: utnso
 */

#ifndef SRC_CPU_H_
#define SRC_CPU_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <utils/conexion.h>

t_log* logger;
t_config* config;
t_log* logger_instruciones;
t_list* instrucciones ;
bool recibi_archivo;

int tamanio_pagina;

typedef struct{
	int marco;
	int desplazamiento;
	int nro_pagina;
}t_traduccion;

int conexion_memoria;
char *ip_memoria;
char *puerto_memoria;
char *puerto_escucha;
char *puerto_escucha_interrupt;

bool instruccion_ejecutando;

int registro_por_mov;

t_log *logger_consola_cpu;
bool hayInterrupcion;

t_instruccion* instruccion_a_realizar;

t_pcb* pcb;
bool hay_desalojo;

sem_t contador_recibiendoArchivo;
sem_t contador_instruccion;
sem_t contador_esperando_mov;
sem_t contador_marco_obtenido;

int marco_obtenido;


void obtener_configuracion();
void terminar_programa();
t_config* iniciar_config();
void iniciar_servidor_cpu(char*);
void iterator(char*);
void iniciar_consola();
t_pcb* ejecutar_instruccion(int );
uint32_t obtener_valor( t_estrucutra_cpu );
void setear(t_estrucutra_cpu , uint32_t );
t_estrucutra_cpu devolver_registro(char* );
void fetch( int);
void decode(t_instruccion*,int);
void solicitar_instruccion_ejecutar_segun_pc(int,int);
void generar_conexion_memoria();
char** parsear_instruccion(char* );
void transformar_en_instrucciones(char*);
void imprimir_valores_registros(t_registro_cpu* );
void iniciar_servidor_interrupt(char * );

void enviar_sleep(int ,int ,op_code );

void atendiendo_pedido(int);
//archivos
void leer_pseudocodigo(FILE* );
char** parsear_instruccion(char* );


uint32_t obtener_el_valor_de_memoria(t_traduccion* );

void enviar_f_open(char* archivo, char* modo, int conexion, op_code operacion);
void enviar_f_close(char* archivo, int conexion, op_code operacion);
void enviar_f_seek(char* archivo, int offset, int conexion, op_code operacion);
void enviar_f_read(char* archivo, int size, int conexion, op_code operacion);
void enviar_f_write(char* archivo, int size, int conexion, op_code operacion);
void enviar_f_truncate(char* archivo, int tamanio, int conexion, op_code operacion);
void enviar_traduccion(t_traduccion* ,op_code );
void enviar_traduccion_mov_out(t_traduccion* ,op_code ,int);
void enviar_recurso_a_kernel(char* ,op_code ,int );
void enviar_pagina_a_kernel(t_traduccion*  ,op_code  , int );

#endif /* SRC_CPU_H_ */
