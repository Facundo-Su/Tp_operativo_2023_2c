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

int conexion_memoria;
char *ip_memoria;
char *puerto_memoria;
char *puerto_escucha;
t_log *logger_consola_cpu;
bool hayInterrupcion;

t_instruccion* instruccion_a_realizar;

t_pcb* pcb;

sem_t contador_recibiendoArchivo;

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


void atendiendo_pedido(int);
//archivos
void leer_pseudocodigo(FILE* );
char** parsear_instruccion(char* );


#endif /* SRC_CPU_H_ */
