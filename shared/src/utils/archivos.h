#ifndef ARCHIVOS_H_
#define ARCHIVOS_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <utils/conexion.h>

t_list* leer_pseudocodigo(FILE*);
char** parsear_instruccion(char*);


#endif /* ARCHIVOS_H_*/
