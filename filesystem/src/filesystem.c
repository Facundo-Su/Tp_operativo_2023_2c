
#include "filesystem.h"

int main(int argc, char* argv[]) {

	config = config_create("./filesystem.config");


    if (config == NULL) {
        printf("No se encontró el archivo de configuración");
         return EXIT_FAILURE;
    }



    logger = log_create("./filesystem.log", "FILESYSTEM", true, LOG_LEVEL_INFO);


    log_info(logger, "Soy el filesystem!");
    //obtener datos de .config
	obtenerConfiguracion();

	//creo la conexion
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	enviar_mensaje("filesystem a memoria", conexion_memoria);

	//realiza la operacion segun lo que necesita el cliente
	iniciarServidor(puerto_escucha);



	terminar_programa(conexion_memoria, logger, config);
    return EXIT_SUCCESS;
}





void obtenerConfiguracion(){
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	puerto_escucha= config_get_string_value(config,"PUERTO_ESCUCHA");
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
