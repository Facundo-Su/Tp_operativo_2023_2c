//archivos
#include "archivos.h"


t_list* leer_pseudocodigo(FILE* pseudocodigo){
	t_list* instrucciones = list_create();
    // Creo las variables para parsear el archivo
    char* instruccion = NULL;
    size_t len = 0;

    // Recorro el archivo de pseudocodigo y parseo las instrucciones
    while (getline(&instruccion, &len, pseudocodigo) != -1){

        // Parseo la instruccion
        char** instruccion_parseada = parsear_instruccion(instruccion);

        // Añado la instruccion parseada a la lista de instrucciones
        list_add(instrucciones, instruccion_parseada);
    }
    return instrucciones;
}

char** parsear_instruccion(char* instruccion){

    // Parseo la instruccion
    char** instruccion_parseada = string_split(instruccion, " ");

    // Retorno la instruccion parseada
    return instruccion_parseada;
}
