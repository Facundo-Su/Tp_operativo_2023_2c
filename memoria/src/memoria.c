#include "memoria.h"


int main(int argc, char* argv[]) {


	char *rutaConfig = "./memoria.config";

	config = cargar_config(rutaConfig);

    obtener_configuraciones();

    iniciar_recursos();
    inicializar_memoria();

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
	contador_fifo =0;
	//sem_init(&enviar_marco,0,0);
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
				pthread_t hilo_aux;
				pthread_create(&hilo_aux,NULL,(void*) iniciar_servidor_memoria,(void*)puerto_escucha);

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
    char *algoritmo_config = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
    //log_info(logger,"el valor del algoritmo es %s",algoritmo_config);
    asignar_algoritmo(algoritmo_config);
    strcat(path_instrucciones,"/");
    auxiliar = auxiliar* 1000;
    retardo_respuesta = (useconds_t) auxiliar;
}
void asignar_algoritmo(char *algoritmo_config){
	if (strcmp(algoritmo_config, "FIFO") == 0) {
		algoritmo = FIFO;
	} else if (strcmp(algoritmo_config, "LRU") == 0) {
		algoritmo = LRU;
	}else{
		log_error(logger, "El algoritmo no es valido");
	}
}

void iniciar_servidor_memoria(char *puerto) {

    int memoria_fd = iniciar_servidor(puerto);
    log_info(logger, "Servidor listo para recibir al cliente");

    generar_conexion_fs();

    while (1) {
        int cliente_fd = esperar_cliente(memoria_fd);
		pthread_t atendiendo;
		pthread_create(&atendiendo,NULL,(void*)procesar_conexion,(void *) &cliente_fd);
		if (setsockopt(cliente_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
			    error("setsockopt(SO_REUSEADDR) failed");
		pthread_detach(atendiendo);

    }
}

void generar_conexion_fs(){
	log_info(logger, "generar conexion con fs\n");
	pthread_t conexion_memoria_hilo_cpu;
	conexion_filesystem = crear_conexion(ip_file_system, puerto_filesystem);
	pthread_create(&conexion_memoria_hilo_cpu,NULL,(void*) procesar_conexion,(void *)&conexion_filesystem);
}

/*
void procesar_conexion(void *conexion1){
	int *conexion = (int*)conexion1;
	int cliente_fd = *conexion;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		t_pcb* pcb_aux;
		t_list* paquete;
		t_list*paquete2;
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;

		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return;
}*/

void procesar_conexion(void* socket){
	int *conexion = (int*)socket;
	int cliente_fd = *conexion;
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
	            	int *marco = list_get(lista,0);
	            	int *desplazamiento = list_get(lista,1);
	            	int *nro_pag_mov_in = list_get(lista,2);

	            	log_info(logger, "Me llegaron los siguientes valores de marco: %i",*marco);
	            	log_info(logger, "Me llegaron los siguientes valores de desplazamiento: %i",*desplazamiento);
	            	log_info(logger, "Me llegaron los siguientes valores de nro_pag: %i",*nro_pag_mov_in);

	            	//asignar_marco(pid, *nro_pag_mov_in);

	            	uint32_t *valor_leido =malloc(sizeof(uint32_t));
	            	memcpy(valor_leido, memoria->espacio_usuario + (*marco *tam_pagina) + *desplazamiento, sizeof(uint32_t));
	            	enviar_registro_leido_mov_in(valor_leido,ENVIO_MOV_IN,cliente_fd);
	            	break;
	            case ENVIO_MOV_OUT:
	            	lista = recibir_paquete(cliente_fd);
	            	uint32_t *marco_out = list_get(lista,0);
	            	int *desplazamiento_out= list_get(lista,1);
	            	int *pagina_out = list_get(lista,2);
	            	int *pid_out = list_get(lista,3);
	            	uint32_t *valor_remplazar = list_get(lista,4);
	            	log_info(logger, "Me llegaron los siguientes valores de marco: %i",*marco_out);
	            	log_info(logger, "Me llegaron los siguientes valores de desplazamiento: %i",*desplazamiento_out);
	            	log_info(logger, "Me llegaron los siguientes valores de escritura: %i",*valor_remplazar);
	            	//TODO realizar operacion;
	            	memcpy(memoria->espacio_usuario + (1* tam_pagina) + *desplazamiento_out, valor_remplazar, sizeof(uint32_t));
	            	uint32_t *valor_leido2 =malloc(sizeof(uint32_t));
	            	memcpy(valor_leido2, memoria->espacio_usuario + (1 *tam_pagina)+ *desplazamiento_out , sizeof(uint32_t));
	            	log_info(logger,"%u",*valor_leido2);
	            	//t_pagina * pagina = list_get()
	            	//modificar_tabla_pagina(*pid_out,*pagina_out);

	            	break;
	            case FINALIZAR:
	            	t_list * paquete = recibir_paquete(cliente_fd);
	            	t_pcb* pcb_recibido = desempaquetar_pcb(paquete);
	            	//valor_pid= recibir_pcb(cliente_fd);
	            	log_info(logger,"ME LLEGO EL PID CON EL VALOR %i :",pcb_recibido->pid);
	            	finalizar_proceso(pcb_recibido->pid);
	            	//realizar_proceso_finalizar(valor_pid->pid);
	            	break;
	            case OBTENER_MARCO:

	            	lista = recibir_paquete(cliente_fd);
	            	int * pid_proceso = list_get(lista,0);
	            	int * pagina_proceso = list_get(lista,1);
	            	int marco_encontrado= obtener_marco(*pid_proceso,*pagina_proceso);
	            	/*if(marco_encontrado==-1){
	            		sem_wait(&enviar_marco);
	            		enviar_marco(marco_encontrado, OBTENER_MARCO,cliente_fd);
	            	}else{
	            		enviar_marco(marco_encontrado, OBTENER_MARCO,cliente_fd);
	            	}*/
	            	enviar_marco(marco_encontrado, OBTENER_MARCO,cliente_fd);
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
	    		case PAGE_FAULT:
	    			lista = recibir_paquete(cliente_fd);
	    			int* nro_pagina = list_get(lista,0);
	    			int* pid_page_fault = list_get(lista,1);
	    			asignar_marco(*pid_page_fault, *nro_pagina);
	    			enviar_mensaje_instrucciones("ok capo",cliente_fd,OK_PAG_CARGADA);
	    			//int posicion_swap= obtener_posicion_swap(*pid,*nro_pagina);
	    			//cargar_en_espacio_memoria(3);

	    			break;
	    		case DATOS_SWAP:
	    			lista = lista = recibir_paquete(cliente_fd);
	    			datos_obtenidos = list_get(lista,0);
	    			sem_post(&contador_espera_cargar);

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

void cargar_en_espacio_memoria(int posicion_swap,int marco){

	//enviar_fs_cargar(posicion_swap,DATOS_SWAP);
	sem_wait(&contador_espera_cargar);
	memcpy(memoria->espacio_usuario + (marco * tam_pagina), &datos_obtenidos, sizeof(datos_obtenidos));
	log_info(logger,"datos cargados");


}




int obtener_posicion_swap(int pid,int nro_pagina){
    bool encontrar_tabla_pagina(void * tabla_pagina){
          t_tabla_paginas* un_tabla_pagina = (t_tabla_paginas*)tabla_pagina;
          //log_info(logger_consola_memoria,"comparando pid %i",pid);
          int *valor_comparar =un_tabla_pagina->pid;
         // log_info(logger_consola_memoria,"comparando el pid %i",*valor_comparar);
          return *valor_comparar == pid;
    }
    t_tabla_paginas* tabla_pagina = list_find(memoria->lista_tabla_paginas, encontrar_tabla_pagina);

    t_pagina * pagina = list_get(tabla_pagina->paginas,nro_pagina);
    return pagina->pos_en_swap;

}

void modificar_tabla_pagina(int pid , int nro_pagina){

    bool encontrar_tabla_pagina(void * tabla_pagina){
          t_tabla_paginas* un_tabla_pagina = (t_tabla_paginas*)tabla_pagina;
          //log_info(logger_consola_memoria,"comparando pid %i",pid);
          int valor_comparar =un_tabla_pagina->pid;
         // log_info(logger_consola_memoria,"comparando el pid %i",valor_comparar);
          return valor_comparar == pid;
    }
	t_tabla_paginas* tabla_pagina = list_find(memoria->lista_tabla_paginas, encontrar_tabla_pagina);

	    if (tabla_pagina != NULL) {
	            // Se encontró un elemento que cumple con la condición
	            log_info(logger, "Se encontraron la talba de pagina para el PID %i", pid);
	            t_pagina* pagina = list_get(tabla_pagina->paginas,nro_pagina);
	            pagina->m=1;
	        } else {
	            // No se encontraron instrucciones que cumplan con la condición
	            log_info(logger, "No se encontraron la pagina para el PID %i", pid);
	        }
}

int obtener_marco(int pid, int pagina){
    bool encontrar_tabla_pagina(void * tabla_pagina){
          t_tabla_paginas* un_tabla_pagina = (t_tabla_paginas*)tabla_pagina;
         // log_info(logger_consola_memoria,"comparando pid %i",pid);
          int valor_comparar =un_tabla_pagina->pid;
          //log_info(logger_consola_memoria,"comparando el pid %i",valor_comparar);
          return valor_comparar == pid;
    }
    t_tabla_paginas* tabla_pagina = list_find(memoria->lista_tabla_paginas, encontrar_tabla_pagina);

    log_info(logger,"la cantidad de pagina quye hay es %i",list_size(tabla_pagina->paginas));

    if (tabla_pagina != NULL) {
            // Se encontró un elemento que cumple con la condición
            t_pagina* pagina_encontrado = list_get(tabla_pagina->paginas,pagina);
            if(pagina_encontrado->p ==1){
                log_warning(logger, "Se encontraron la en el marco PID %i........marco %i", tabla_pagina->pid,pagina_encontrado->num_marco);
            	return pagina_encontrado->num_marco;

            }else{
            	return -1;
            }
        } else {
            // No se encontraron instrucciones que cumplan con la condición
            log_info(logger, "No se encontraron la pagina para el PID %i", pid);
            return -1;
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


void enviar_marco(int marco , op_code operacion,int cliente_fd){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &marco, sizeof(int));
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
         // log_info(logger_consola_memoria,"comparando pid %i",pid);
          int *valor_comparar =un_instruccion->pid;
          //log_info(logger_consola_memoria,"comparando el pid %i",*valor_comparar);
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
	list_add(memoria->lista_tabla_paginas,tabla_paginas);
}

t_tabla_paginas *inicializar_paginas(int pid, int size){
	t_tabla_paginas* tabla_paginas = malloc(sizeof(t_tabla_paginas));
	int p = (size + tam_pagina -1)/tam_pagina;
	tabla_paginas->pid = pid;
	tabla_paginas->tamanio_proceso = size;
	tabla_paginas->paginas_necesarias = p;
	tabla_paginas->paginas = crear_paginas(p);
	log_info(logger, "Se creo la tabla de paginas con el PID %i", pid);
	return tabla_paginas;
}

t_list * crear_paginas(int paginas_necesarias){
	t_list * paginas = list_create();
	for(int c =0; c<paginas_necesarias;c++){
		t_pagina * pagina = malloc(sizeof(t_pagina));
		pagina->num_marco =-1;
		pagina->m =0;
		pagina->p =0;
		int pos_swap =-1; //= recibir_pos_swap(pid,conexion_filesystem,RESERVAR_SWAP);
		pagina->pos_en_swap = pos_swap;
		list_add(paginas,pagina);
	}
	log_info(logger,"la cantidad de pagina es %i",list_size(paginas));
	return paginas;
}

void finalizar_proceso(int pid){
	t_list_iterator* iterador = list_iterator_create(memoria->lista_tabla_paginas);
	int j =0;
	while(list_iterator_has_next(iterador)){
		t_tabla_paginas* tabla_paginas = (t_tabla_paginas*)list_iterator_next(iterador);
		if( pid == tabla_paginas->pid){
			list_remove(memoria->lista_tabla_paginas,j);
			liberar_tabla_paginas(tabla_paginas);
			log_info(logger, "Se elimino la tabla de paginas con el PID %i", pid);
		}
		j++;
	}
	list_iterator_destroy(iterador);
}
void liberar_tabla_paginas(t_tabla_paginas *tabla){
	list_destroy_and_destroy_elements(tabla->paginas, free);
	free(tabla);
}


void inicializar_memoria(){
	memoria = malloc(sizeof(t_memoria));
	memoria->espacio_usuario= malloc(tam_memoria);
	memoria->tamanio_marcos = tam_pagina;
	memoria->cantidad_marcos = tam_memoria/tam_pagina;
	memoria->marcos = list_create();
	memoria->lista_tabla_paginas = list_create();
	iniciar_particionamiento_memoria();
}
void finalizar_memoria(){
	free(memoria->espacio_usuario);
	list_destroy_and_destroy_elements(memoria->marcos, free);
	free(memoria);
}
void iniciar_particionamiento_memoria(){
    int i, desplazamiento = 0;
    for(i=0; i<memoria->cantidad_marcos; i++) {
        t_marco *marco = malloc(sizeof(t_marco));
        marco->base = desplazamiento;
        marco->is_free = true;
        marco->num_marco = i;
        list_add(memoria->marcos, marco);
        desplazamiento+= memoria->tamanio_marcos;
    }
}


int encontrar_marco_libre() {
    int i;
    for(i=0;i<memoria->cantidad_marcos;i++) {
    	t_marco *marco = list_get(memoria->marcos, i);
		if(marco->is_free) {
            return i;
		}
	}
    return -1;
}

void asignar_marco(int pid, int nro_pagina){
	t_marco * marco;
	t_pagina * pagina = obtener_pagina(pid, nro_pagina);
	if(!pagina_esta_en_memoria(pid,nro_pagina)){
		int i = encontrar_marco_libre();
		if(i!=-1){
			marco = list_get(memoria->marcos,i);
			marco->is_free = false;
			marco->pid = pid;
			marco->llegada_fifo = contador_fifo;
			marco->last_time_lru =0;
			pagina->num_marco = i;
			log_info(logger,"se lleno el marco %i",marco->num_marco);
			list_replace(memoria->marcos,i,marco);
			contador_fifo++;
			actualizar_marcos_lru();
			t_marco * marco2 = list_get(memoria->marcos,i);
			log_info(logger,"SE ASIGNO EL MARCO %i CON PID %i",marco2->num_marco,marco2->pid);
			//actualizar_tablas(pid,i,nro_pagina);
		}else{
			int nro_marco_remplazado = ejecutar_algoritmo();
			log_info(logger,"se remplazo el marco %i",nro_marco_remplazado);
			//t_pagina * pagina = obtener_pagina(nro_marco_remplazado);
			if(pagina->m = 1){
				//hacerswappout
			}
			//actualizar_tablas(pid,nro_marco_remplazado,nro_pagina);
		}
		pagina->p = 1;
	}else{
	    bool encontrar_tabla_pagina(void * tabla_pagina){
	          t_tabla_paginas* un_tabla_pagina = (t_tabla_paginas*)tabla_pagina;
	          //log_info(logger_consola_memoria,"comparando pid %i",pid);
	          int valor_comparar =un_tabla_pagina->pid;
	          //log_info(logger_consola_memoria,"comparando el pid %i",valor_comparar);
	          return valor_comparar == pid;
	    }
	    t_tabla_paginas* tabla_pagina = list_find(memoria->lista_tabla_paginas, encontrar_tabla_pagina);

		actualizar_marcos_lru();
		t_pagina * pagina = list_get(tabla_pagina->paginas,nro_pagina);
		marco = list_get(memoria->marcos,pagina->num_marco);
		marco->last_time_lru =0;
		list_replace(memoria->marcos,marco->num_marco,marco);
	}
	//sem_post(&enviar_marco);
}
bool pagina_esta_en_memoria(int pid, int nro_pagina){
    bool encontrar_tabla_pagina(void * tabla_pagina){
          t_tabla_paginas* un_tabla_pagina = (t_tabla_paginas*)tabla_pagina;
          //log_info(logger_consola_memoria,"comparando pid %i",pid);
          int valor_comparar =un_tabla_pagina->pid;
          //log_info(logger_consola_memoria,"comparando el pid %i",valor_comparar);
          return valor_comparar == pid;
    }
    t_tabla_paginas* tabla_pagina = list_find(memoria->lista_tabla_paginas, encontrar_tabla_pagina);
	t_pagina * pagina = list_get(tabla_pagina->paginas,nro_pagina);

	t_list_iterator* iterador = list_iterator_create(memoria->marcos);
	while(list_iterator_has_next(iterador)){
		t_marco * marco = (t_marco*)list_iterator_next(iterador);
		if(marco->pid == pid && (marco->num_marco == pagina->num_marco)&& pagina->p == 1){
			log_info(logger,"Encontre la pagina");
			return true;
		}
	}
	list_iterator_destroy(iterador);
	log_info(logger,"No encontre la pagina");
	return false;
}
void actualizar_marcos_lru(){
	t_list_iterator* iterador = list_iterator_create(memoria->marcos);
	int j =0;
	while(list_iterator_has_next(iterador)){
		t_marco * marco = (t_marco*)list_iterator_next(iterador);
		if(!marco->is_free){
			marco->last_time_lru++;
			list_replace(memoria->marcos,j,marco);
		}
		j++;
	}
	list_iterator_destroy(iterador);
}
t_pagina * obtener_pagina(int pid,int nro_pagina){
	t_list_iterator* iterador = list_iterator_create(memoria->lista_tabla_paginas);
	int j =0;
	while(list_iterator_has_next(iterador)){
		t_tabla_paginas * tabla = (t_tabla_paginas*)list_iterator_next(iterador);
		if(tabla->pid== pid){
			 t_pagina * pagina  = list_get(tabla->paginas,nro_pagina);
			 return pagina;
		}
	}
	list_iterator_destroy(iterador);
	return NULL;
}

int ejecutar_algoritmo(){
	switch(algoritmo){
	case FIFO:
		return ejecutar_fifo();
		break;
	case LRU:
		return ejecutar_lru();
		break;
	default:
		log_info(logger,"ERROR Planificador");
		return -1;
	break;
	}
}

int ejecutar_fifo(){
	t_list_iterator* iterador = list_iterator_create(memoria->marcos);
	int llegada = INT_MAX;
	int nro_marco;
	while(list_iterator_has_next(iterador)){
		t_marco * marco = (t_marco*)list_iterator_next(iterador);
		if(!marco->is_free && (marco->llegada_fifo < llegada)){
			llegada = marco->llegada_fifo;
			nro_marco = marco->num_marco;
		}
	}
	list_iterator_destroy(iterador);
	return nro_marco;
}
int ejecutar_lru(){
	t_list_iterator* iterador = list_iterator_create(memoria->marcos);
	int tiempo = 0;
	int nro_marco;
	while(list_iterator_has_next(iterador)){
		t_marco * marco = (t_marco*)list_iterator_next(iterador);
		if(!marco->is_free && (marco->last_time_lru > tiempo)){
			tiempo = marco->last_time_lru;
			nro_marco = marco->num_marco;
		}
	}
	list_iterator_destroy(iterador);
	return nro_marco;
}
void actualizar_tablas(int pid, int nro_marco, int nro_pagina){
	t_list_iterator* iterador = list_iterator_create(memoria->lista_tabla_paginas);
	int j =0;

	while(list_iterator_has_next(iterador)){
		t_tabla_paginas* tabla = (t_tabla_paginas*)list_iterator_next(iterador);
		if(pid == tabla->pid){
			t_pagina * pagina = list_get(tabla->paginas,nro_pagina);
			pagina->p = 1;
			pagina->num_marco = nro_marco;
			list_replace(tabla->paginas,nro_pagina,pagina);
			list_replace(memoria->lista_tabla_paginas,j,tabla);
		}
	j++;
	}
	list_iterator_destroy(iterador);
}


