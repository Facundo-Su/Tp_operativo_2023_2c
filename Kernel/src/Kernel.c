/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <Kernel.h>

int main() {
	int conexion_memoria=0, conexion_cpu=0,conexion_file_system=0;
	obtenerConfiguracion();
	logger = log_create("kernel_log.log", "kernel", true, LOG_LEVEL_INFO);

	if(!generar_conexion(logger,conexion_memoria,conexion_cpu,conexion_file_system)){
		cerrarPrograna(logger);
		return EXIT_FAILURE;
	}



	return EXIT_SUCCESS;
}

void obtenerConfiguracion(){
    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    ip_filesystem = config_get_string_value(config, "IP_FILESYSTEM");
    ip_cpu = config_get_string_value(config, "IP_CPU");
    char *algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    asignar_algoritmo(algoritmo);
    puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
    puerto_filesystem = config_get_int_value(config, "PUERTO_FILESYSTEM");
    puerto_cpu_dispatch = config_get_int_value(config, "PUERTO_CPU_DISPATCH");
    puerto_cpu_interrupt = config_get_int_value(config, "PUERTO_CPU_INTERRUPT");
    quantum = config_get_int_value(config, "QUANTUM");
    grado_multiprogramacion_ini = config_get_int_value(config, "GRADO_MULTIPROGRAMACION_INI");

    recursos = config_get_array_value(config, "RECURSOS");
    char **instancias = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    instancias_recursos = string_to_int_array(instancias);
    string_array_destroy(instancias);

}

bool generarConexion(t_log* logger,int* conexion_memoria,int *conexion_cpu,int *conexion_file_system){
	*conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	*conexion_cpu = crear_conexion(ip_cpu,puerto_cpu_dispatch);
	*conexion_file_system= crear_conexion(ip_filesystem, puerto_filesystem);
	return
}
