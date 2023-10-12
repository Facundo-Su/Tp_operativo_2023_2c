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
#include "instrucciones.h"

t_log* logger;
t_config* config;
t_log* logger_instruciones;
t_list* instrucciones ;


int conexion_memoria;
char *ip_memoria;
char *puerto_memoria;
char *puerto_escucha;
t_log *logger_consola_cpu;

void obtener_configuracion();
void terminar_programa();
t_config* iniciar_config();
int iniciar_servidor(char*);
void iterator(char*);
void iniciar_consola();
t_pcb* ejecutar_instruccion(t_pcb* );
char* obtener_valor(t_pcb* , t_estrucutra_cpu );
void setear(t_pcb* , t_estrucutra_cpu , char* );
t_estrucutra_cpu devolver_registro(char* );
void ejecutar(t_pcb* ,t_instruccion*,int);

//archivos
void leer_pseudocodigo(FILE* );

char** parsear_instruccion(char* );

#endif /* SRC_CPU_H_ */
