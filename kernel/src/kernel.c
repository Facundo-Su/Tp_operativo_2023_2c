#include "kernel.h"

int main(int argc,char** argv){
    t_log* logger = log_create("./cfg/kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el proceso 1! %d", mi_funcion_compartida());
    log_destroy(logger);
}
