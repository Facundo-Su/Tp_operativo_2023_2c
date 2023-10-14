#include "memoria.h"


int main(int argc, char* argv[]) {


	char *rutaConfig = "./memoria.config";

	config = cargar_config(rutaConfig);

    obtener_configuraciones();

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
				pthread_t atendiendo;
				pthread_create(&atendiendo,NULL,(void*)iniciar_servidor_memoria,(void *) puerto_escucha);
				iniciar_servidor_memoria(puerto_escucha);
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
}

int iniciar_servidor_memoria(char *puerto) {

    int memoria_fd = iniciar_servidor(puerto);
    log_info(logger, "Servidor listo para recibir al cliente");

    while (1) {
        int cliente_fd = esperar_cliente(memoria_fd);
		pthread_t atendiendo;
		pthread_create(&atendiendo,NULL,(void*)atendiendo_pedido,(void *) cliente_fd);
		pthread_detach(atendiendo);

    }
}

void atendiendo_pedido(int cliente_fd){
	while (1) {
	            int cod_op = recibir_operacion(cliente_fd);
	            t_list * lista;
	            switch (cod_op) {
	            case MENSAJE:
	                recibir_mensaje(cliente_fd);
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
					char* ruta = list_get(valorRecibido,0);
					int* size = list_get(valorRecibido,1);
					int* prioridad = list_get(valorRecibido,2);
					int* pid = list_get(valorRecibido,3);

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
	    		case OBTENER_INSTRUCCION:
	    			t_list* lista = list_create();
	    			lista = recibir_paquete(cliente_fd);
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
	t_list* lista_instrucciones_parseada = list_create();

	instruccion->instrucciones = lista_instrucciones_parseada;

}
void realizar_proceso_finalizar(int valor){

}

