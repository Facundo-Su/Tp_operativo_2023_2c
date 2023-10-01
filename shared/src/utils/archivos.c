//archivos
#include "archivos.h"

// el problema esta en esta recibiendo un char y tiene que ser un op_instrucciones
t_list* leer_pseudocodigo(FILE* pseudocodigo){
	t_list* instrucciones = list_create();
    // Creo las variables para parsear el archivo
    char* instruccion = NULL;
    size_t len = 0;
    t_instruccion *instruct;
    // Recorro el archivo de pseudocodigo y parseo las instrucciones
    while (getline(&instruccion, &len, pseudocodigo) != -1){

        // Parseo la instruccion

        char** instruccion_parseada = parsear_instruccion(instruccion);
        if (strcmp(instruccion_parseada[0], "SET") == 0) {
            instruct->nombre = SET;
        } else if (strcmp(instruccion_parseada[0], "SUB") == 0) {
            instruct->nombre = SUB;
        } else if (strcmp(instruccion_parseada[0], "SUM") == 0) {
            instruct->nombre = SUM;
        } else if (strcmp(instruccion_parseada[0], "EXIT") == 0) {
            instruct->nombre = EXIT;
        } else {
            // Manejar instrucción desconocida
            free(instruct);
            return NULL;
        }

        instruct->parametros = list_create();
        for(int i=1;i<sizeof(instruccion_parseada);i++){
        	list_add(instruct->parametros,instruccion_parseada[i]);
        }

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
