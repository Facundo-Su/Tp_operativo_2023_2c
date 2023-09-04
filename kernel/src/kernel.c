#include "kernel.h"
#include<readline/readline.h>
int main(int argc, char **argv){

	config = config_create("./kernel.config");

    logger = log_create("./kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el Kernel!");

    obtenerConfiguracion();

    // conexiones
    conexion_memoria =crear_conexion(ip_memoria, puerto_memoria);
    conexion_file_system = crear_conexion(ip_filesystem, puerto_filesystem);
    conexion_cpu = crear_conexion(ip_cpu, puerto_cpu_dispatch);

    //envio de mensajes
    //enviar_mensaje("kernel a memoria", conexion_memoria);
    //enviar_mensaje("kernel a cpu", conexion_cpu);
    //enviar_mensaje("kernel a filesystem", conexion_file_system);

    //error
    paquete(conexion_memoria);


    terminar_programa(conexion_memoria, logger, config);
    terminar_programa(conexion_cpu, logger, config);
    terminar_programa(conexion_file_system, logger, config);


    return EXIT_SUCCESS;
}


void obtenerConfiguracion(){

    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    ip_filesystem = config_get_string_value(config, "IP_FILESYSTEM");
    ip_cpu = config_get_string_value(config, "IP_CPU");
    char *algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    asignarAlgoritmo(algoritmo);
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    puerto_filesystem = config_get_string_value(config, "PUERTO_FILESYSTEM");
    puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    quantum = config_get_int_value(config, "QUANTUM");
    grado_multiprogramacion_ini = config_get_int_value(config, "GRADO_MULTIPROGRAMACION_INI");
    recursos = config_get_array_value(config, "RECURSOS");
    char **instancias = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    instancias_recursos = string_to_int_array(instancias);
    string_array_destroy(instancias);
}

void asignarAlgoritmo(char *algoritmo){
	if (strcmp(algoritmo, "FIFO") == 0) {
		algoritmo_planificacion = FIFO;
	} else if (strcmp(algoritmo, "HRRN") == 0) {
		algoritmo_planificacion = ROUND_ROBIN;
	}else if(strcmp(algoritmo, "PRIORIDADES")==0){
		algoritmo_planificacion = PRIORIDADES;
	}else{
		log_error(logger, "El algoritmo no es valido");
	}
}

int* string_to_int_array(char** array_de_strings){
	int count = string_array_size(array_de_strings);
	int *numbers = malloc(sizeof(int) * count);
	for(int i = 0; i < count; i++){
		int num = atoi(array_de_strings[i]);
		numbers[i] = num;
	}
	return numbers;
}

void paquete(int conexion)
{
	char* leido;
	t_paquete* paquete = crear_paquete();
	// Leemos y esta vez agregamos las lineas al paquete
	leido = readline(">");
	while(leido && leido[0] != '\0'){
		agregar_a_paquete(paquete, leido, conexion);
		free(leido);
		leido = readline(">");
	}
	free(leido);
	enviar_paquete(paquete,conexion);
	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	eliminar_paquete(paquete);
}



