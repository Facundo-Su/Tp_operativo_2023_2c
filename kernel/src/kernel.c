#include "kernel.h"
#include<readline/readline.h>
int main(int argc, char **argv){


	char *rutaConfig = "kernel.config";
	config = cargarConfig(rutaConfig);

    logger = log_create("./kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el Kernel!");

    obtenerConfiguracion();
    iniciarRecurso();
    iniciarConsola();

    //envio de mensajes

    pthread_t servidorKernel;

    pthread_create(&servidorKernel,NULL,(void*) iniciarServidor,NULL);
    //error
    //paquete(conexion_memoria);

    terminar_programa(conexion_memoria, logger, config);
    terminar_programa(conexion_cpu, logger, config);
    terminar_programa(conexion_file_system, logger, config);


    return EXIT_SUCCESS;
}


void* iniciarServidor(char *puerto){
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
		case FINALIZAR:
			t_pcb* pcbAuxiliar = malloc(sizeof(t_pcb));
			pcbAuxiliar = recibir_pcb(cliente_fd);
			enviar_Pcb(pcbAuxiliar,conexion_memoria,FINALIZAR);
			break;

		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

}


void iniciarConsola(){
	loggerConsola = log_create("./kernelConsola.log", "consola", 1, LOG_LEVEL_INFO);
	char* variable;

	while(1){
		log_info(loggerConsola,"ingrese la operacion que deseas realizar"
				"\n 1. iniciar Proceso"
				"\n 2. finalizar proceso"
				"\n 3. iniciar Planificacion"
				"\n 4. detener Planificacion"
				"\n 5. modificar grado multiprogramacion"
				"\n 6. listar procesos por estado"
				"\n 7. generar conexion"
				"\n 8. enviar mensaje");
		variable = readline(">");

		switch (*variable) {
			case '1':
				log_info(loggerConsola, "ingrese la ruta");
				char* ruta = readline(">");
				log_info(loggerConsola, "ingrese el tamanio");
				int* size = atoi(readline(">"));
				iniciarProceso(ruta,&size,planificador);
				break;
			case '2':
				log_info(loggerConsola, "ingrese pid");
				char* valor = readline(">");
				int valorNumero = atoi(valor);
				finalizarProceso(valorNumero);
				break;
			case '3':
				iniciarPlanificacion();
				break;
			case '4':
				detenerPlanificacion();
				break;
			case '5':
				modificarGradoMultiprogramacion();
				break;
			case '6':
				listarProcesoPorEstado();
				break;
			case '7':
				generarConexion();
				break;
			case '8':
				enviarMensaje();
				break;
			case '9':
				crear_pcb(NULL, FIFO);
				/*t_pcb* auxiliar = malloc(sizeof(t_pcb));
				auxiliar =queue_pop(cola_new);
				log_info(loggerConsola,"el pid es %i",auxiliar->pid);*/

				enviar_Pcb(queue_pop(cola_new),conexion_cpu,RECIBIR_PCB);
			default:
				log_info(loggerConsola,"no corresponde a ninguno");
				exit(2);
		}
		free(variable);


	}

}

void iniciarRecurso(){
	lista_pcb=list_create();
	cola_new = queue_create();
	cola_ready = queue_create();
	sem_init(&gradoMultiprogramacion, 0, grado_multiprogramacion_ini);
	sem_init(&mutex_cola_new, 0, 1);
	sem_init(&mutex_cola_ready,0,1);

}

void enviarMensaje() {
	log_info(loggerConsola,"ingrese q que modulos deseas mandar mensaje"
			"\n 1. modulo memoria"
			"\n 2. modulo cpu"
			"\n 3. modulo filesystem");

    char *valor = readline(">");
	switch (*valor) {
		case '1':
	        enviar_mensaje("kernel a memoria", conexion_memoria);
	        log_info(loggerConsola,"mensaje enviado correctamente\n");
			break;
		case '2':
	        enviar_mensaje("kernel a cpu", conexion_cpu);
	        log_info(loggerConsola,"mensaje enviado correctamente\n");
			break;
		case '3':
	        enviar_mensaje("kernel a filesystem", conexion_file_system);
	        log_info(loggerConsola,"mensaje enviado correctamente\n");
			break;
		default:
			log_info(loggerConsola,"no corresponde a ninguno\n");
			break;
	}
}

void generarConexion() {

	log_info(loggerConsola,"ingrese q que modulos deseas conectar"
			"\n 1. modulo memoria"
			"\n 2. modulo filesystem"
			"\n 3. modulo cpu");

    char *valor = readline(">");
	switch (*valor) {
		case '1':

			conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
			setsockopt(conexion_memoria, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	        log_info(loggerConsola,"conexion generado correctamente\n");
			break;
		case '2':
			conexion_file_system = crear_conexion(ip_filesystem, puerto_filesystem);
			setsockopt(conexion_file_system, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	        log_info(loggerConsola,"conexion generado correctamente\n");
			break;
		case '3':
			conexion_cpu = crear_conexion(ip_cpu, puerto_cpu_dispatch);
			setsockopt(conexion_cpu, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	        log_info(loggerConsola,"conexion generado correctamente\n");
			break;
		default:
			log_info(loggerConsola,"no corresponde a ninguno\n");
			break;
	}

}

//hilo que espere consola,
void iniciarProceso(char* archivo_test,int* size,t_planificador prioridad){

	//char* prueba = ruta_archivo_test;
	//string_append(*prueba, archivo_test);

	char*rutaAtestear = archivo_test;
	t_list* instruccion = obtenerListaInstruccion(archivo_test);

	crear_pcb(instruccion);

	op_code op = INICIAR_PROCESO;
	t_paquete* paquete =crear_paquete(op);
	agregar_a_paquete(paquete, rutaAtestear, sizeof(rutaAtestear));
	agregar_a_paquete(paquete, &size ,sizeof(size));

	enviar_paquete(paquete, conexion_memoria);

	//free(prueba);
	eliminar_paquete(paquete);
	free(rutaAtestear);

}

void crear_pcb(t_list* instrucciones,t_planificador prioridad){
	t_pcb* pcb = malloc(sizeof(pcb));
	pcb->pid= contador_pid;
	pcb->prioridad = prioridad;
	t_contexto_ejecucion* contexto = crearContexto();
	pcb->contexto =NULL;
	//pcb->tabla_archivo_abierto;
	pcb->estado=NEW;
	contador_pid++;

	agregarAColaNew(pcb);
}

t_contexto_ejecucion* crearContexto(){
	t_contexto_ejecucion* contexto = malloc(sizeof(t_contexto_ejecucion));
	contexto->pc =NULL;
	t_registro_cpu* registro = crearRegistro();
	contexto->registros_cpu = registro;
	return contexto;
}

t_registro_cpu* crearRegistro(){
	t_registro_cpu* reg = malloc(sizeof(t_registro_cpu));
    memset(reg->AX, 0, sizeof(reg->AX));
    memset(reg->BX, 0, sizeof(reg->BX));
    memset(reg->CX, 0, sizeof(reg->CX));
    memset(reg->DX, 0, sizeof(reg->DX));
	return reg;
}

t_list* obtenerListaInstruccion(char* ruta){
	t_list *instrucciones = list_create();
	FILE* pseucodigo;

	pseucodigo =fopen(ruta,"r");
	instrucciones =leer_pseudocodigo(pseucodigo);

	return instrucciones;
}

void agregarAColaNew(t_pcb* pcb){
	sem_wait(&mutex_cola_new);
	queue_push(cola_new,pcb);
	sem_post(&mutex_cola_new);
	log_info(logger,"El proceso [%d] fue agregado a la cola new",pcb->pid);
}

t_pcb* quitarDeColaNew(){
	sem_wait(&mutex_cola_new);
	t_pcb* pcb=queue_pop(cola_new);
	sem_post(&mutex_cola_new);
	log_info(logger,"El proceso [%d] fue quitado de la cola new",pcb->pid);
	return pcb;
}
void agregarAColaReady(t_pcb* pcb){
	sem_wait(&mutex_cola_ready);
	queue_push(cola_ready,pcb);
	pcb->estado=READY;
	sem_post(&mutex_cola_ready);
	log_info(logger,"El proceso [%d] fue agregado a la cola ready",pcb->pid);
}
t_pcb* quitarDeColaReady(){
	sem_wait(&mutex_cola_ready);
	t_pcb* pcb=queue_pop(cola_ready);
	sem_post(&mutex_cola_ready);
	log_info(logger,"El proceso [%d] fue quitado de la cola ready",pcb->pid);
	return pcb;
}
void planificadorLargoPlazo(){
	while(1){
		while(!queue_is_empty(cola_new)){
			sem_wait(&gradoMultiprogramacion);
			t_pcb* pcb =quitarDeColaNew();
			agregarAColaReady(pcb);
		}
	}
}
void planificadorCortoPlazo(){
	while(1){
			while(!queue_is_empty(cola_ready)){
				switch(planificador){
				case FIFO:
					//Transcionarlo a Running
					log_info(logger,"Planificador FIFO");
					deReadyAFifo();
					//Enviar su contexto de ejecucion al CPU a traves del puerto dispatch
					break;
				case ROUND_ROBIN:
					log_info(logger,"Planificador Round Robin");
					//deReadyARoundRobin();
					break;
				case PRIORIDADES:
					log_info(logger,"Planificador Prioridades");
					//deReadyAPrioridades();
					break;
				}
			}
		}
}
void deReadyAFifo(){
	t_pcb* pcb =quitarDeColaReady();
	pcb->estado=RUNNING;
	enviar_Pcb(pcb,conexion_cpu,EJECUTARINSTRUCIONES);
}

/*
void deReadyARoundRobin(){
	t_pcb* pcbEnEjecucion = NULL;

	enviar_Pcb(queue_pop(cola_ready),conexion_cpu,PLANIFICACION);

}

//alternativa agregar una cola de ejecucion

void deReadyAPrioridades(){

	t_pcb* pcb = procesoConMayorPrioridad();

	for(int i=0)


	if(pcb->prioridad > pcbEnEjecucion->prioridad){ //Conseguir contexto del pcb en ejecucion
		//Enviar interrupcion a CPU y desalojar proceso
		pcb->estado=RUNNING;
		if(pcbEnEjecucion->prioridad !=-1){
			desalojar();
		}

		enviar_Pcb(pcb,conexion_cpu,EJECUTARINSTRUCIONES);
	}
}


struct t_pcb* procesoConMayorPrioridad() {
    struct t_pcb* procesoElegido = NULL;
    int prioridadMaxima = -1; // Inicializar con un valor alto
    t_list * lista = list_create();
    for (int i = 0; i < cantProcesos; i++) {
        if (colaReady[i].prioridad < prioridadMaxima) {
        	procesoElegido = &colaReady[i];
        	prioridadMaxima = colaReady[i].prioridad;
        }
    }

    return procesoElegido;
}
*/

bool controladorMultiProgramacion(){
	return list_size(lista_pcb)<grado_multiprogramacion_ini;
}





t_contexto_ejecucion* obtenerContexto(char* archivo){
	t_contexto_ejecucion *estructura_retornar ;

	return estructura_retornar;
}

// ver como pasar int TODO
void finalizarProceso(char *pid){

	t_paquete * paquete = crear_paquete(FINALIZAR);
	agregar_a_paquete(paquete, pid, sizeof(pid));
	enviar_paquete(paquete, conexion_memoria);

	eliminar_paquete(paquete);
	free(paquete);




	//int posicion= buscarPosicionQueEstaElPid(pid);
	//t_pcb* auxiliar =list_remove(cola_new->elements,posicion);
	//liberarMemoriaPcb(auxiliar);
}

void liberarMemoriaPcb(t_pcb* pcbABorrar){
		free(pcbABorrar->contexto);
		free(pcbABorrar->tabla_archivo_abierto);
		free(pcbABorrar);
}

int buscarPosicionQueEstaElPid(int valor){
	int cantidad= list_size(lista_pcb);
	t_pcb* elemento ;
	for(int i=0;i<cantidad;i++){
		elemento = list_get(lista_pcb,cantidad);
		if(elemento->pid == valor){
			return cantidad;
		}
	}

	return -1;
}

void iniciarPlanificacion(){
	log_info(loggerConsola,"inicio el proceso de planificacion");

}
void detenerPlanificacion(){

}
void modificarGradoMultiprogramacion(){

}
void listarProcesoPorEstado(){

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
		planificador = FIFO;
	} else if (strcmp(algoritmo, "HRRN") == 0) {
		planificador = ROUND_ROBIN;
	}else if(strcmp(algoritmo, "PRIORIDADES")==0){
		planificador = PRIORIDADES;
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

