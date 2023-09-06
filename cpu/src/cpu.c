
#include "cpu.h"

int main(int argc, char* argv[]) {

	char *rutaConfig = argv[1];

	config = cargarConfig(rutaConfig);

    logger = log_create("./cpu.log", "CPU", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy la cpu!");

    //iniciar configuraciones
	 obtenerConfiguracion();

	//generar conexion
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);


	enviar_mensaje("cpu a memoria", conexion_memoria);

	//Inicia el cpu como servidor
	iniciarServidor(puerto_escucha);

	terminar_programa(conexion_memoria, logger, config);
    return 0;
}



void obtenerConfiguracion(){
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
}
int iniciarServidor(char *puerto){
	int servidor_fd = iniciar_servidor(puerto);
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(servidor_fd);

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
void iterator(char* value) {
	log_info(logger,"%s", value);
}
