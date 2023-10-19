#include "memoria.h"


int main(int argc, char* argv[]) {


	char *rutaConfig = "./memoria.config";

	config = cargar_config(rutaConfig);

    obtener_configuraciones();

    iniciar_recursos();

    logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);

    log_info(logger, "Soy el Memoria!");

    iniciar_consola();

    // Limpieza y terminación
    terminar_programa(conexion_filesystem, logger, config);

    return EXIT_SUCCESS;
}


void iniciar_recursos(){
	lista_instrucciones = list_create();
}

void iniciar_consola(){
	logger_consola_memoria = log_create("./memoriaConsola.log", "consola", 1, LOG_LEVEL_INFO);
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
}

void iniciar_servidor_memoria(char *puerto) {

    int memoria_fd = iniciar_servidor(puerto);
    log_info(logger, "Servidor listo para recibir al cliente");

    while (1) {
        int cliente_fd = esperar_cliente(memoria_fd);
		pthread_t atendiendo;
		pthread_create(&atendiendo,NULL,(void*)procesar_conexion,(void *) cliente_fd);
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
	   //         	recibir_estructura_Inicial(cliente_fd);
					//char* ruta = strcat(path_instrucciones,list_get(valorRecibido,0));
					int* size = list_get(valorRecibido,1);
					int* prioridad = list_get(valorRecibido,2);
					int* pid = list_get(valorRecibido,3);
					char *ruta = "./prueba.txt";

	                log_info(logger, "Me llegaron los siguientes valores de ruta: %s",ruta);
	                log_info(logger, "Me llegaron los siguientes valores de size: %i",*size);
	                log_info(logger, "Me llegaron los siguientes valores de prioridad: %i",*prioridad);
	                log_info(logger, "Me llegaron los siguientes valores de pid: %i",*pid);


	                cargar_lista_instruccion(ruta,size,prioridad,pid);
	                break;
	            case FINALIZAR:

	            	t_list* valor_pid;
	            	valor_pid= recibir_paquete(cliente_fd);
	            	int* valor = list_get(valor_pid,0);
	            	log_info(logger,"ME LLEGO EL PID CON EL VALOR %i :",*valor);
	            	//realizar_proceso_finalizar(valor);
	            	break;
	    		case INSTRUCCIONES_A_MEMORIA:

	    			t_list* lista;
	    			lista = recibir_paquete(cliente_fd);
	    			int* pc_recibido = list_get(lista,0);
	    			int* pid_recibido = list_get(lista,1);
	    			log_info(logger_consola_memoria,"me llegaron el siguiente pc %i",*pc_recibido);
	    			log_info(logger_consola_memoria,"me llegaron el siguiente pid %i",*pid_recibido);



	    		    bool encontrar_instrucciones(void * instruccion){
	    		          t_instrucciones* un_instruccion = (t_instrucciones*)instruccion;
	    		          log_info(logger_consola_memoria,"comparando pid %i",*pid_recibido);
	    		          int *valor_comparar =un_instruccion->pid;
	    		          log_info(logger_consola_memoria,"comparando el pid %i",*valor_comparar);
	    		          return *valor_comparar == *pid_recibido;
	    		    }
	    		    t_instrucciones* instrucciones = list_find(lista_instrucciones, encontrar_instrucciones);

	    		    if (instrucciones != NULL) {
	    		        // Se encontró un elemento que cumple con la condición
	    		        log_info(logger, "Se encontraron instrucciones para el PID %i", *pid_recibido);
	    		        log_info(logger, "Cantidad de instrucciones: %i", list_size(instrucciones->instrucciones));

	    		        // Luego puedes realizar las operaciones que necesites con 'instrucciones'
	    		    } else {
	    		        // No se encontraron instrucciones que cumplan con la condición
	    		        log_info(logger, "No se encontraron instrucciones para el PID %i", *pid_recibido);
	    		    }
	    			char*valor_obtenido = list_get(instrucciones->instrucciones,*pc_recibido);
    		        log_info(logger, "el instruccion que se envio es  %s", valor_obtenido);
	    			enviar_mensaje_instrucciones(valor_obtenido,cliente_fd,INSTRUCCIONES_A_MEMORIA);
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


void cargar_lista_instruccion(char *ruta,int size,int prioridad,int pid){
	t_instrucciones * instruccion = malloc(sizeof(t_instruccion));
	instruccion->pid = pid;
	instruccion->instrucciones = list_create();
	FILE * archivo = fopen(ruta,"r");
	t_list* auxiliar = leer_pseudocodigo(archivo);

	list_add_all(instruccion->instrucciones,auxiliar);
	list_add(lista_instrucciones,instruccion);
	int cantidad = list_size(auxiliar);

	t_instrucciones* instruuu = list_get(lista_instrucciones,0);
	int cantidad2 = list_size(lista_instrucciones);
	int *auxiliar2 = instruuu->pid;
	log_info(logger_consola_memoria,"la lista total total de general es %i",cantidad2);
	log_info(logger_consola_memoria,"el valor de la pid primero es  %i",*auxiliar2);
	fclose(archivo);

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
    char *instruc_aux_nose3 = list_get(instrucciones_del_pcb,2);
    log_info(logger_consola_memoria,"el instruccion es  %s",instruc_aux_nose3);
    return instrucciones_del_pcb;

}


void realizar_proceso_finalizar(int valor){

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



