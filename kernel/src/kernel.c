#include <stdio.h>
#include <stdlib.h>

#include <kernel.h>

int main(int argc, char* argv[]) {

	t_log * logger = log_create("kernel.log", "kernel_main", true,LOG_LEVEL_INFO);
	obtenerConfiguracion();


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
