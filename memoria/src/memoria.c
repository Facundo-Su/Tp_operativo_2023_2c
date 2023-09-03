#include "memoria.h"

//falta hacer que reciba este pueda enviar mensaje a cpu
int main(void) {
	logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);

	config = config_create("./memoria.config");

	obtenerConfiguraciones(puerto_escucha);
	esperandoOperacion();
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
//obtengo las configuraciones de .config
void obtenerConfiguraciones(){
	puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
}


//operacion despues de recibir el mensaje
int esperandoOperacion(char *puerto){
	int memoria_fd = iniciar_servidor(puerto);
	log_info(logger, "Servidor listo para recibir al cliente");
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
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

