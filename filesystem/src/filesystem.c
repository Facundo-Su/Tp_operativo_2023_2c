#include <stdlib.h>
#include <stdio.h>
#include "filesystem.h"

int main(int argc, char* argv[]) {
	config = iniciar_config();
    logger = log_create("./filesystem.log", "FILESYSTEM", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el filesystem!");
	obtenerConfiguracion();
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	enviar_mensaje("filesystem a memoria", conexion_memoria);
	terminar_programa(conexion_memoria, logger, config);
    return 0;
}
void obtenerConfiguracion(){
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
}

t_config* iniciar_config(){
    t_config * config_nuevo = config_create("./filesystem.config");
    if (config == NULL) {
        fprintf(stderr, "No se encontró el archivo de configuración");
         return EXIT_FAILURE;
    }
    return config_nuevo;
}
