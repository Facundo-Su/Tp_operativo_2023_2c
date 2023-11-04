#include "memoria.h"


int main(int argc, char* argv[]) {


	char *rutaConfig = "./memoria.config";

	config = cargar_config(rutaConfig);

    obtener_configuraciones();

    iniciar_recursos();

    logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);

    log_info(logger, "Soy el Memoria!");

    //iniciar_consola();
	iniciar_servidor_memoria(puerto_escucha);
    // Limpieza y terminación
    terminar_programa(conexion_filesystem, logger, config);

    return EXIT_SUCCESS;
}


void iniciar_recursos(){
	lista_instrucciones = list_create();
	lista_tabla_paginas = list_create();
	logger_consola_memoria = log_create("./memoriaConsola.log", "consola", 1, LOG_LEVEL_INFO);
}

void iniciar_consola(){

	char* valor;

	while(1){
		log_info(logger_consola_memoria,"ingrese la operacion que deseas realizar"
				"\n 1. generar conexion"
				"\n 2. enviar mensaje"
				"\n 3. iniciar como servidor"
				"\n 4. obtener todas las instrucciones del archivos");
		valor = readline("<");
		switch (*valor) {
			case '1':
				log_info(logger_consola_memoria, "generar conexion con filesystem\n");
				conexion_filesystem = crear_conexion(ip_file_system, puerto_filesystem);
				break;
			case '2':
				log_info(logger_consola_memoria, "enviar mensaje a memoria\n");
				enviar_mensaje("memoria a fylesystem", conexion_filesystem);
				break;
			case '3':
				log_info(logger_consola_memoria, "se inicio el servidor\n");
				//TODO
				//pthread_t atendiendo;
				//pthread_create(&atendiendo,NULL,(void*)iniciar_servidor_memoria,(void *) puerto_escucha);
				iniciar_servidor_memoria(puerto_escucha);
				break;
			case '4':

				char * ruta = "./prueba.txt";
				FILE * archivo = fopen(ruta,"r");

				t_list* lista_aux_prueba =leer_pseudocodigo(archivo);
				log_info(logger_consola_memoria, "hola\n");
				log_info(logger_consola_memoria,"el resultado final de la lista es %i",list_size(lista_aux_prueba));

				t_instruccion * inst_prueba = list_get(lista_aux_prueba,0);
				char* valor_prueba = list_get(inst_prueba->parametros,0);
				log_info(logger_consola_memoria,"el resultado del cod_op es %d",inst_prueba->nombre);
				//log_info(logger_consola_memoria,"el pid es ");
				break;
			default:
				log_info(logger_consola_memoria,"no corresponde a ninguno");
				exit(2);
		}
		free(valor);
	}

}

void iterator(char* value) {
    log_info(logger, "%s", value);
}

void obtener_configuraciones() {
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    puerto_filesystem = config_get_string_value(config, "PUERTO_FILESYSTEM");
    ip_file_system = config_get_string_value(config, "IP_FILESYSTEM");
    path_instrucciones =config_get_string_value(config,"PATH_INSTRUCCIONES");
    tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    tam_pagina = config_get_int_value(config,"TAM_PAGINA");

    int auxiliar = config_get_int_value(config,"RETARDO_RESPUESTA");
    strcat(path_instrucciones,"/");
    auxiliar = auxiliar* 1000;
    retardo_respuesta = (useconds_t) auxiliar;
}

void iniciar_servidor_memoria(char *puerto) {

    int memoria_fd = iniciar_servidor(puerto);
    log_info(logger, "Servidor listo para recibir al cliente");

    while (1) {
        int cliente_fd = esperar_cliente(memoria_fd);
		pthread_t atendiendo;
		pthread_create(&atendiendo,NULL,(void*)procesar_conexion,(void *) cliente_fd);
		if (setsockopt(cliente_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
			    error("setsockopt(SO_REUSEADDR) failed");
		pthread_detach(atendiendo);

    }
}

void procesar_conexion(int cliente_fd){

	while (1) {
	            int cod_op = recibir_operacion(cliente_fd);
	            t_list * lista;

	            switch (cod_op) {
	            case MENSAJE:
	                recibir_mensaje(cliente_fd);
	                log_info(logger,"hola como estas capo");
	                enviar_mensaje("hola", cliente_fd);
	                break;
	            case PAQUETE:
	                lista = recibir_paquete(cliente_fd);
	                log_info(logger, "Me llegaron los siguientes valores:\n");
	                list_iterate(lista, (void*) iterator);
	                break;
	            case INICIAR_PROCESO:

	            	t_list* valorRecibido;
					valorRecibido=recibir_paquete(cliente_fd);
					char * aux =list_get(valorRecibido,0);
					//TODO DESBLOQUEAR DESPUES


					int* size = list_get(valorRecibido,1);
					int* prioridad = list_get(valorRecibido,2);
					int* pid = list_get(valorRecibido,3);
					//char *ruta = "./prueba.txt";
					char *ruta =	obtener_ruta(aux);
	                log_info(logger, "Me llegaron los siguientes valores de ruta: %s",ruta);
	                log_info(logger, "Me llegaron los siguientes valores de size: %i",*size);
	                log_info(logger, "Me llegaron los siguientes valores de prioridad: %i",*prioridad);
	                log_info(logger, "Me llegaron los siguientes valores de pid: %i",*pid);

	                cargar_lista_instruccion(ruta,size,prioridad,*pid);
	                crear_proceso(*pid, *size);
	                break;
	            case MANDAME_PAGINA:
	            	recibir_mensaje(cliente_fd);
	            	enviar_tam_pagina(tam_pagina, cliente_fd);
	            	break;
	            case ENVIO_MOV_IN:

	            	lista = recibir_paquete(cliente_fd);
	            	int *nro_pagina = list_get(lista,0);
	            	uint32_t nro_pagina_mov_in = (uint32_t) *nro_pagina;

	            	int *desplazamiento = list_get(lista,1);
	            	uint32_t desplazamiento_mov_in = (uint32_t) *desplazamiento;
	            	log_info(logger, "Me llegaron los siguientes valores de nro_pag: %i",*nro_pagina);
	            	log_info(logger, "Me llegaron los siguientes valores de desplazamiento: %i",*desplazamiento);


	            	enviar_registro_leido_mov_in(9,ENVIO_MOV_IN,cliente_fd);

	            	break;
	            case ENVIO_MOV_OUT:
	            	lista = recibir_paquete(cliente_fd);
	            	int *nro_pagina_out = list_get(lista,0);
	            	int *desplazamiento_out= list_get(lista,1);
	            	int *valor_remplazar = list_get(lista,2);
	            	log_info(logger, "Me llegaron los siguientes valores de nro_pag: %i",*nro_pagina_out);
	            	log_info(logger, "Me llegaron los siguientes valores de desplazamiento: %i",*desplazamiento_out);
	            	log_info(logger, "Me llegaron los siguientes valores de escritura: %i",*valor_remplazar);
	            	//TODO realizar operacion;
	            	break;
	            case FINALIZAR:

	            	t_list * paquete = recibir_paquete(cliente_fd);
	            	t_pcb* pcb_recibido = desempaquetar_pcb(paquete);
	            	//valor_pid= recibir_pcb(cliente_fd);
	            	log_info(logger,"ME LLEGO EL PID CON EL VALOR %i :",pcb_recibido->pid);
	            	finalizar_proceso(pcb_recibido->pid);
	            	//realizar_proceso_finalizar(valor_pid->pid);
	            	break;
	    		case INSTRUCCIONES_A_MEMORIA:
	    			usleep(retardo_respuesta);
	    			t_list* lista;
	    			lista = recibir_paquete(cliente_fd);
	    			int* pc_recibido = list_get(lista,0);
	    			int* pid_recibido = list_get(lista,1);
	    			log_info(logger_consola_memoria,"me llegaron el siguiente pc %i",*pc_recibido);
	    			log_info(logger_consola_memoria,"me llegaron el siguiente pid %i",*pid_recibido);

	    		    bool encontrar_instrucciones(void * instruccion){
	    		          t_instrucciones* un_instruccion = (t_instrucciones*)instruccion;
	    		          int valor_comparar =un_instruccion->pid;
	    		          return valor_comparar == *pid_recibido;
	    		    }
	    		    t_instrucciones* instrucciones = list_find(lista_instrucciones, encontrar_instrucciones);

	    		    if (instrucciones != NULL) {
	    		        // Se encontró un elemento que cumple con la condición
	    		        log_info(logger, "Se encontraron instrucciones para el PID %i", *pid_recibido);
	    		        log_info(logger, "Cantidad de instrucciones: %i", list_size(instrucciones->instrucciones));
	    		        char*valor_obtenido = list_get(instrucciones->instrucciones,*pc_recibido);
	    		        log_info(logger, "el instruccion que se envio es  %s", valor_obtenido);
		    			enviar_mensaje_instrucciones(valor_obtenido,cliente_fd,INSTRUCCIONES_A_MEMORIA);
	    		        // Luego puedes realizar las operaciones que necesites con 'instrucciones'
	    		    } else {
	    		        // No se encontraron instrucciones que cumplan con la condición
	    		        log_info(logger, "No se encontraron instrucciones para el PID %i", *pid_recibido);
	    		    }

	    			//char*valor_obtenido = list_get(instrucciones->instrucciones,*pc_recibido);
	    			//log_info(logger_consola_memoria,"pid encontrado bien ahi es %i",instrucciones->pid);
	    			//t_paquete* paquete = crear_paquete(INSTRUCCIONES_A_MEMORIA);
	    			//empaquetar_instrucciones(paquete,instrucciones->instrucciones);
	    			//enviar_paquete(paquete, cliente_fd);
	    			break;
	            case -1:
	                log_error(logger, "El cliente se desconectó. Terminando servidor");
	                close(cliente_fd);
	                return; // Salir del bucle interno para esperar un nuevo cliente
	            default:
	                log_warning(logger, "Operación desconocida. No quieras meter la pata");
	                break;
	            }
	        }
}

char* obtener_ruta(char* valorRecibido) {
    char* ruta = malloc(strlen(path_instrucciones) + strlen(valorRecibido) + 5); // +5 para ".txt" y el carácter nulo
    strcpy(ruta, path_instrucciones);
    strcat(ruta, valorRecibido);
    strcat(ruta, ".txt");
    return ruta;
}

void enviar_registro_leido_mov_in(int valor_encontrado , op_code operacion,int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &valor_encontrado, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}


void enviar_tam_pagina(int tam , int cliente_fd){
	t_paquete* paquete = crear_paquete(MANDAME_PAGINA);
	agregar_a_paquete(paquete, &tam, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}


void cargar_lista_instruccion(char *ruta, int size, int prioridad, int pid) {
    t_instrucciones* instruccion = malloc(sizeof(t_instruccion));
    log_info(logger_consola_memoria, "%i", pid);
    instruccion->pid = pid;
    instruccion->instrucciones = list_create();
    FILE* archivo = fopen(ruta, "r");

    if (archivo == NULL) {
        log_error(logger_consola_memoria, "El archivo %s no pudo ser abierto.", ruta);
        free(instruccion);  // Liberar la memoria asignada a instruccion
    } else {
        t_list* auxiliar = leer_pseudocodigo(archivo);
        list_add_all(instruccion->instrucciones, auxiliar);
        list_add(lista_instrucciones, instruccion);
        int cantidad = list_size(auxiliar);

        t_instrucciones* instruuu = list_get(lista_instrucciones, 0);
        int cantidad2 = list_size(lista_instrucciones);

        log_info(logger_consola_memoria, "La lista total total de general es %i", cantidad2);
        log_info(logger_consola_memoria, "El valor de la pid primero es %i", instruuu->pid);
        fclose(archivo);
    }
}



t_list* leer_pseudocodigo(FILE* pseudocodigo){
    // Creo las variables para parsear el archivo
    char* instruccion = NULL;
    size_t len = 0;
    int cantidad_parametros;
    t_list* instrucciones_correspondiente_al_archivo = list_create();
	t_list * instrucciones_del_pcb = list_create();
	int j=0;
    // Recorro el archivo de pseudocodigo y parseo las instrucciones
    while (getline(&instruccion, &len, pseudocodigo) != -1){

    	log_info(logger_consola_memoria,"el valor es %s" ,instruccion);
    	char* valor_remplazo = strdup(instruccion);
        list_add(instrucciones_del_pcb,valor_remplazo);
        char *instruc_aux_nose23 = list_get(instrucciones_del_pcb,j);
        j++;
        log_info(logger_consola_memoria,"el instruccion es  %s",instruc_aux_nose23);
    }

    char *instruc_aux_nose = list_get(instrucciones_del_pcb,0);
    log_info(logger_consola_memoria,"el instruccion es  %s",instruc_aux_nose);
    char *instruc_aux_nose2 = list_get(instrucciones_del_pcb,1);
    log_info(logger_consola_memoria,"el instruccion es  %s",instruc_aux_nose2);
    //char *instruc_aux_nose3 = list_get(instrucciones_del_pcb,2);
    //log_info(logger_consola_memoria,"el instruccion es  %s",instruc_aux_nose3);
    return instrucciones_del_pcb;

}


//TODO liberar memoria uszando la funcion void list_remove_and_destroy_by_condition(t_list *, bool(*condition)(void*), void(*element_destroyer)(void*));
void realizar_proceso_finalizar(int pid){

    bool encontrar_instrucciones(void * instruccion){
          t_instrucciones* un_instruccion = (t_instrucciones*)instruccion;
          log_info(logger_consola_memoria,"comparando pid %i",pid);
          int *valor_comparar =un_instruccion->pid;
          log_info(logger_consola_memoria,"comparando el pid %i",*valor_comparar);
          return *valor_comparar == pid;
    }
    t_instrucciones* instrucciones = list_find(lista_instrucciones, encontrar_instrucciones);

    if (instrucciones != NULL) {
        // Se encontró un elemento que cumple con la condición
        log_info(logger, "Se encontraron instrucciones para el PID %i", pid);
        log_info(logger, "Cantidad de instrucciones: %i", list_size(instrucciones->instrucciones));

        // Luego puedes realizar las operaciones que necesites con 'instrucciones'
    } else {
        // No se encontraron instrucciones que cumplan con la condición
        log_info(logger, "No se encontraron instrucciones para el PID %i", pid);
    }
}

op_instrucciones asignar_cod_instruccion(char* instruccion){
	if (strcmp(instruccion, "SET") == 0) {
	        return SET;
	    }
	    if (strcmp(instruccion, "SUB") == 0) {
	        return SUB;
	    }
	    if (strcmp(instruccion, "SUM") == 0) {
	        return SUM;
	    }
	    if (strcmp(instruccion, "JNZ") == 0) {
	        return JNZ;
	    }
	    if (strcmp(instruccion, "SLEEP") == 0) {
	        return SLEEP;
	    }
	    if (strcmp(instruccion, "WAIT") == 0) {
	        return WAIT;
	    }
	    if (strcmp(instruccion, "SIGNAL") == 0) {
	        return SIGNAL;
	    }
	    if (strcmp(instruccion, "MOV_IN") == 0) {
	        return MOV_IN;
	    }
	    if (strcmp(instruccion, "MOV_OUT") == 0) {
	        return MOV_OUT;
	    }
	    if (strcmp(instruccion, "F_OPEN") == 0) {
	        return F_OPEN;
	    }
	    if (strcmp(instruccion, "F_CLOSE") == 0) {
	        return F_CLOSE;
	    }
	    if (strcmp(instruccion, "F_SEEK") == 0) {
	        return F_SEEK;
	    }
	    if (strcmp(instruccion, "F_READ") == 0) {
	        return F_READ;
	    }
	    if (strcmp(instruccion, "F_WRITE") == 0) {
	        return F_WRITE;
	    }
	    if (strcmp(instruccion, "F_TRUNCATE") == 0) {
	        return F_TRUNCATE;
	    }
	    if (strcmp(instruccion, "EXIT") == 0) {
	        return EXIT;
	    }

}
void crear_proceso(int pid, int size){
	t_tabla_paginas * tabla_paginas = inicializar_paginas(pid, size);
	list_add(lista_tabla_paginas,tabla_paginas);


}

t_tabla_paginas *inicializar_paginas(int pid, int size){
	t_tabla_paginas* tabla_paginas = malloc(sizeof(t_tabla_paginas));
	tabla_paginas->pid = pid;
	tabla_paginas->tamanio_proceso = size;
	tabla_paginas->paginas_necesarias = (size + tam_pagina -1)/tam_pagina;
	tabla_paginas->paginas= list_create;
	log_info(logger, "Se creo la tabla de paginas con el PID %i", pid);
	return tabla_paginas;
}

void finalizar_proceso(int pid){
	t_list_iterator* iterador = list_iterator_create(lista_tabla_paginas);
	int j =0;
	while(list_iterator_has_next(iterador)){
		t_tabla_paginas* tabla_paginas = (t_tabla_paginas*)list_iterator_next(iterador);
		if( pid == tabla_paginas->pid){
			list_remove(lista_tabla_paginas,j);
			liberar_tabla_paginas(tabla_paginas);
			log_info(logger, "Se elimino la tabla de paginas con el PID %i", pid);
		}
		j++;
	}
	list_iterator_destroy(iterador);
}
void liberar_tabla_paginas(t_tabla_paginas *tabla){
	//list_destroy_and_destroy_elements(tabla->paginas, liberar_paginas);
	free(tabla);
}

void liberar_paginas(void *pagina) {
    free(pagina);
}

void algoritmo_de_remplazo(){}

void inicializar_memoria(){
	memoria = malloc(sizeof(t_memoria));
	memoria->tam_memoria= malloc(tam_memoria);
	memoria->tamanio_marcos = tam_pagina;
	memoria->cantidad_marcos = tam_memoria/tam_pagina;
	memoria->marcos_asignados = list_create();
}
void finalizar_memoria(){
	free(memoria->tam_memoria);
	list_destroy_and_destroy_elements(memoria->marcos_asignados, free);
	free(memoria);
}



