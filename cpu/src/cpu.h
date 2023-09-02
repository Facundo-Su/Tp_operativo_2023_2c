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

int conexion_memoria;
char *ip_memoria;
char *puerto_memoria;

void obtenerConfiguracion();
void terminar_programa();
t_config* iniciar_config();

#endif /* SRC_CPU_H_ */
