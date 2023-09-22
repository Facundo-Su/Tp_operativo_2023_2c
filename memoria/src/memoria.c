#include "memoria.h"


int main(int argc, char* argv[]) {


	char *rutaConfig = argv[1];

	config = cargarConfig(rutaConfig);

    obtenerConfiguraciones();

    logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);

    log_info(logger, "Soy el Memoria!");

    iniciarConsola();

    // Limpieza y terminación
    terminar_programa(conexion_filesystem, logger, config);

    return EXIT_SUCCESS;
}

void iniciarConsola(){
	logger_consola_memoria = log_create("./memoriaConsola.log", "consola", 1, LOG_LEVEL_INFO);
	char* valor;

	while(1){
		log_info(logger_consola_memoria,"ingrese la operacion que deseas realizar"
				"\n 1. generar conexion"
				"\n 2. enviar mensaje"
				"\n 3. iniciarComoServidor");
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
				iniciarServidor(puerto_escucha);
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

void obtenerConfiguraciones() {
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    puerto_filesystem = config_get_string_value(config, "PUERTO_FILESYSTEM");
    ip_file_system = config_get_string_value(config, "IP_FILESYSTEM");
}

int iniciarServidor(char *puerto) {
    int memoria_fd = iniciar_servidor(puerto);
    log_info(logger, "Servidor listo para recibir al cliente");

    while (1) {
        int cliente_fd = esperar_cliente(memoria_fd);

        t_list* lista;
        while (1) {
            int cod_op = recibir_operacion(cliente_fd);
            switch (cod_op) {
            case MENSAJE:
                recibir_mensaje(cliente_fd);
                break;
            case PAQUETE:
                lista = recibir_paquete(cliente_fd);
                log_info(logger, "Me llegaron los siguientes valores:\n");
                list_iterate(lista, (void*) iterator);
                break;
            case ENVIARRUTAPARAINICIAR:
            	t_list* valorRecibido;
				valorRecibido=recibir_paquete(cliente_fd);
   //         	recibir_estructura_Inicial(cliente_fd);
                log_info(logger, "Me llegaron los siguientes valores: %s",list_get(valorRecibido,0));
                log_info(logger, "Me llegaron los siguientes valores: %i",list_get(valorRecibido,1));
                break;
            case -1:
                log_error(logger, "El cliente se desconectó. Terminando servidor");
                close(cliente_fd);
                return EXIT_FAILURE; // Salir del bucle interno para esperar un nuevo cliente
            default:
                log_warning(logger, "Operación desconocida. No quieras meter la pata");
                break;
            }
        }
    }
}


void recibir_estructura_Inicial(int socket_cliente) {
    int size;
    void* buffer;
    int tamanio;

    buffer = recibir_buffer(&size, socket_cliente);
    int desplazamiento = 0;

    // Deserializar el tamaño
    memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    // Deserializar la ruta
    char* valor = malloc(tamanio);
    memcpy(valor, buffer + desplazamiento, tamanio);
    desplazamiento += tamanio;

    // Crear la estructura_Inicial y asignar la ruta
    estrctura_inicial = malloc(sizeof(estructura_Inicial));
    estrctura_inicial->ruta = valor;

    // Deserializar el segundo valor
    int segundo_valor;
    memcpy(&segundo_valor, buffer + desplazamiento, sizeof(int));
    estrctura_inicial->size = segundo_valor;

    // Realizar cualquier operación adicional necesaria

    // Liberar el buffer
    free(buffer);

    // Importante: No olvides liberar la estructura_Inicial cuando ya no la necesites
    //free(auxiliar);
}

