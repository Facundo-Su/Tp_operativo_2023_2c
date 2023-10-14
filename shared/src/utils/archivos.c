//archivos
#include "archivos.h"

//TODO

t_list* leer_pseudocodigo(FILE* pseudocodigo){
    t_list* instrucciones = list_create();
    // Creo las variables para parsear el archivo
    char* instruccion = NULL;
    size_t len = 0;
    int cantidad_parametros;
    t_instruccion *instruct;
    // Recorro el archivo de pseudocodigo y parseo las instrucciones
    while (getline(&instruccion, &len, pseudocodigo) != -1){

        // Parseo la instruccion
        char** instruccion_parseada = parsear_instruccion(instruccion);

        if (strcmp(instruccion_parseada[0], "SET") == 0) {
            instruct->nombre = SET;
            cantidad_parametros = 2;
        }
        if (strcmp(instruccion_parseada[0], "SUB") == 0) {
            instruct->nombre = SUB;
            cantidad_parametros = 2;
        }
        if (strcmp(instruccion_parseada[0], "SUM") == 0) {
            instruct->nombre = SUM;
            cantidad_parametros = 2;
        }
        if (strcmp(instruccion_parseada[0], "JNZ") == 0) {
            instruct->nombre = JNZ;
            cantidad_parametros = 2;
        }
        if (strcmp(instruccion_parseada[0], "SLEEP") == 0) {
            instruct->nombre = SLEEP;
            cantidad_parametros = 1;
        }
        if (strcmp(instruccion_parseada[0], "WAIT") == 0) {
            instruct->nombre = WAIT;
            cantidad_parametros = 1;
        }
        if (strcmp(instruccion_parseada[0], "SIGNAL") == 0) {
            instruct->nombre = SIGNAL;
            cantidad_parametros = 1;
        }
        if (strcmp(instruccion_parseada[0], "MOV_IN") == 0) {
            instruct->nombre = MOV_IN;
            cantidad_parametros = 2;
        }
        if (strcmp(instruccion_parseada[0], "MOV_OUT") == 0) {
            instruct->nombre = MOV_OUT;
            cantidad_parametros = 2;
        }
        if (strcmp(instruccion_parseada[0], "F_OPEN") == 0) {
            instruct->nombre = F_OPEN;
            cantidad_parametros = 2;
        }
        if (strcmp(instruccion_parseada[0], "F_CLOSE") == 0) {
            instruct->nombre = F_CLOSE;
            cantidad_parametros = 1;
        }
        if (strcmp(instruccion_parseada[0], "F_SEEK") == 0) {
            instruct->nombre = F_SEEK;
            cantidad_parametros = 2;
        }
        if (strcmp(instruccion_parseada[0], "F_READ") == 0) {
            instruct->nombre = F_READ;
            cantidad_parametros = 2;
        }
        if (strcmp(instruccion_parseada[0], "F_WRITE") == 0) {
            instruct->nombre = F_WRITE;
            cantidad_parametros = 2;
        }
        if (strcmp(instruccion_parseada[0], "F_TRUNCATE") == 0) {
            instruct->nombre = F_TRUNCATE;
            cantidad_parametros = 2;
        }
        if (strcmp(instruccion_parseada[0], "EXIT") == 0) {
            instruct->nombre = EXIT;
            cantidad_parametros = 0;
        }


        t_list* parametros = list_create();

        for(int i=1;i<cantidad_parametros+1;i++){
            list_add(parametros,instruccion_parseada[i]);
        }

        // Añado la instruccion parseada a la lista de instrucciones
        list_add(instruct->parametros, parametros);
    }
    return instrucciones;
}

char** parsear_instruccion(char* instruccion){

    // Parseo la instruccion
    char** instruccion_parseada = string_split(instruccion, " ");

    // Retorno la instruccion parseada
    return instruccion_parseada;
}

