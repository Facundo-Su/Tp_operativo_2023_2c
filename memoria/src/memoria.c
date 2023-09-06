#include "memoria.h"


int main(int argc, char* argv[]) {


	char *rutaConfig = argv[1];

	config = cargarConfig(rutaConfig);

    obtenerConfiguraciones();

    logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);

    log_info(logger, "Soy el Memoria!");

    conexion_filesystem = crear_conexion(ip_file_system, puerto_filesystem);

    enviar_mensaje("de memoria a fileSystem", conexion_filesystem);

    iniciarServidor(puerto_escucha);

    // Limpieza y terminación
    terminar_programa(conexion_filesystem, logger, config);

    return EXIT_SUCCESS;
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
