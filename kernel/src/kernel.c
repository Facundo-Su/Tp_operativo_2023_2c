#include "kernel.h"
#include<readline/readline.h>

int main(int argc, char **argv){


	char *rutaConfig = "kernel.config";
	config = cargar_config(rutaConfig);

    logger = log_create("./kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el Kernel!");

    obtener_configuracion();
    iniciar_recurso();
    lista_recursos = list_create();
	cola_ready = queue_create();
    int i =0;
    while(recursos_config[i]!=NULL){
    	t_recurso* recurso = malloc(sizeof(t_recurso));
    	recurso->nombre = recursos_config[i];
    	recurso->instancias = atoi(instancias_recursos_config[i]);
    	recurso->cola_bloqueados = queue_create();
    	list_add(lista_recursos,recurso);
    	i++;
    }
    pthread_create(&deadlock_hilo,NULL,(void*) deteccion_automatica,NULL);
    iniciar_consola();

    //envio de mensajes

    //error
    //paquete(conexion_memoria);

    return EXIT_SUCCESS;
}

void deteccion_automatica(){
	while(1){
		sem_wait(&sem_deadlock);
		deadlock();
	}
}


void procesar_conexion(void *conexion1){
	int *conexion = (int*)conexion1;
	int cliente_fd = *conexion;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		t_pcb* pcb_aux;
		t_list* paquete;
		t_list*paquete2;
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case RECIBIR_PCB:
			paquete = recibir_paquete(cliente_fd);
			pcb_aux = desempaquetar_pcb(paquete);
			log_info(logger,"recibi el pcb");
			log_pcb_info(pcb_aux);
			recv(cliente_fd,&cod_op,sizeof(op_code),0);
			switch(cod_op){
			case EJECUTAR_SLEEP:
				pthread_t hilo_sleep;
				t_datos_sleep *datos = malloc(sizeof(t_datos_sleep));
				paquete = recibir_paquete(cliente_fd);
				int *tiempo = list_get(paquete,0);
				list_remove(pcb_en_ejecucion,0);
				sem_post(&contador_ejecutando_cpu);
				log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb_aux->pid);
				log_info(logger,"PID: %i - Bloqueado por: SLEEP",pcb_aux->pid);
				pcb_aux->estado = WAITING;
				list_add(lista_sleep,pcb_aux);
				datos->pcb = pcb_aux;
				datos->tiempo = *tiempo;
				pthread_create(&hilo_sleep, NULL, ejecutar_sleep, datos);
				pthread_detach(hilo_sleep);
				break;
			case EJECUTAR_WAIT:
				paquete = recibir_paquete(cliente_fd);
				char *nombre_recurso =list_get(paquete,0);
				ejecutar_wait(nombre_recurso,pcb_aux);

				break;
			case EJECUTAR_SIGNAL:
				paquete = recibir_paquete(cliente_fd);
				char * nombre_recurso2 =list_get(paquete,0);
				ejecutar_signal(nombre_recurso2,pcb_aux);
				break;
			case EJECUTAR_F_TRUNCATE:
			    log_info(logger, "me llegó la instrucción ejecutar ftruncate del CPU");
			    // Agrega aquí la lógica para ejecutar la operación F_TRUCATE
			    paquete = recibir_paquete(cliente_fd);

			    char* nombre_archivo_truncate = list_get(paquete,0);
			    int* tamanio = list_get(paquete,1);
			    log_info(logger, "el archivo es %s",nombre_archivo_truncate);
			    log_info(logger, "el tamanio es  %",*tamanio);

				list_remove(pcb_en_ejecucion,0);
			    sem_post(&contador_ejecutando_cpu);
			    agregar_a_cola_ready(pcb_aux);
			    sem_post(&contador_cola_ready);
			    break;

			case EJECUTAR_F_OPEN:
			    log_info(logger, "me llegó la instrucción ejecutar fopen del CPU");
			    paquete = recibir_paquete(cliente_fd);
			    char* nombre_archivo = list_get(paquete,0);
			    char* modo_apertura = list_get(paquete,1);
			    log_info(logger, "el archivo es %s",nombre_archivo);
			    log_info(logger, "el modo del archivo es %s",modo_apertura);
				list_remove(pcb_en_ejecucion,0);
			    sem_post(&contador_ejecutando_cpu);
			    agregar_a_cola_ready(pcb_aux);
			    sem_post(&contador_cola_ready);
			    // Agrega aquí la lógica para ejecutar la operación F_OPEN
			    break;

			case EJECUTAR_F_CLOSE:
			    log_info(logger, "me llegó la instrucción ejecutar fclose del CPU");
			    // Agrega aquí la lógica para ejecutar la operación F_CLOSE
			    paquete = recibir_paquete(cliente_fd);
			    char* nombre_archivo_close = list_get(paquete,0);
			    log_info(logger, "el archivo a cerrar es %s",nombre_archivo_close);

				list_remove(pcb_en_ejecucion,0);
			    sem_post(&contador_ejecutando_cpu);
			    agregar_a_cola_ready(pcb_aux);
			    sem_post(&contador_cola_ready);
			    break;

			case EJECUTAR_F_SEEK:
			    log_info(logger, "me llegó la instrucción ejecutar fseek del CPU");
			    // Agrega aquí la lógica para ejecutar la operación F_SEEK
			    paquete = recibir_paquete(cliente_fd);

			    char* nombre_archivo_feseek = list_get(paquete,0);
			    int* posicion = list_get(paquete,1);
			    log_info(logger, "el archivo f_seek es %s",nombre_archivo_feseek);
			    log_info(logger, "el posicion del archivo es %i",*posicion);

				list_remove(pcb_en_ejecucion,0);
			    sem_post(&contador_ejecutando_cpu);
			    agregar_a_cola_ready(pcb_aux);
			    sem_post(&contador_cola_ready);
			    break;

			case EJECUTAR_F_READ:
			    log_info(logger, "me llegó la instrucción ejecutar fread del CPU");
			    // Agrega aquí la lógica para ejecutar la operación F_READ
			    paquete = recibir_paquete(cliente_fd);

			    char* nombre_archivo_f_read = list_get(paquete,0);
			    int* direccion_logica_read= list_get(paquete,1);
			    log_info(logger, "el archivo es %s",nombre_archivo_f_read);
			    log_info(logger, "el direccion logica es %i",*direccion_logica_read);
				list_remove(pcb_en_ejecucion,0);
			    sem_post(&contador_ejecutando_cpu);
			    agregar_a_cola_ready(pcb_aux);
			    sem_post(&contador_cola_ready);
			    break;

			case EJECUTAR_F_WRITE:
			    log_info(logger, "me llegó la instrucción ejecutar fwrite del CPU");
			    // Agrega aquí la lógica para ejecutar la operación F_WRITE
			    paquete = recibir_paquete(cliente_fd);

			    char* nombre_archivo_write = list_get(paquete,0);
			    char* direccion_logica_write= list_get(paquete,1);
			    log_info(logger, "el archivo es %s",nombre_archivo_write);
			    log_info(logger, "el direccion logica es %i",direccion_logica_write);

				list_remove(pcb_en_ejecucion,0);
			    sem_post(&contador_ejecutando_cpu);
			    agregar_a_cola_ready(pcb_aux);
			    sem_post(&contador_cola_ready);
			    break;
			case PAGE_FAULT:
				log_info(logger, "recibi un page fault del cpu ");
				list_add(list_bloqueado_page_fault,pcb_aux);
				paquete2 = recibir_paquete(cliente_fd);
				int *nro_pagina = list_get(paquete2,0);
				envio_page_fault_a_memoria(*nro_pagina,pcb_aux->pid,PAGE_FAULT);
				list_remove(pcb_en_ejecucion,0);
				log_info(logger,"“PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb_aux->pid);
				log_info(logger, "Page Fault PID: %i- Pagina: %i",pcb_aux->pid,nro_pagina);
			    sem_post(&contador_ejecutando_cpu);
			    sem_post(&contador_cola_ready);

				break;

			default:
				log_error(logger, "che no se que me mandaste");
				break;
			}
			break;
		case ENVIAR_DESALOJAR:
			paquete = recibir_paquete(cliente_fd);
			pcb_aux = desempaquetar_pcb(paquete);
			log_pcb_info(pcb_aux);
			log_info(logger,"proceso desalojados ============================");
			list_remove(pcb_en_ejecucion,0);
			agregar_a_cola_ready(pcb_aux);
			sem_post(&contador_cola_ready);
			sem_post(&contador_ejecutando_cpu);
			sem_post(&proceso_desalojo);
			break;

		case FINALIZAR:
			paquete = recibir_paquete(cliente_fd);
			pcb_aux = desempaquetar_pcb(paquete);
			terminar_proceso(pcb_aux);
			log_info(logger, "Finaliza el proceso %i - Motivo: SUCCES",pcb_aux->pid);
			break;

		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return;
}

void envio_page_fault_a_memoria(int nro_pagina,int pid,op_code operacion){
	t_paquete* paquete = crear_paquete(operacion);
	agregar_a_paquete(paquete, &(nro_pagina), sizeof(int));
	agregar_a_paquete(paquete, &(pid), sizeof(int));
	enviar_paquete(paquete, conexion_memoria);
	eliminar_paquete(paquete);

}

void iniciar_consola(){
	logger_consola = log_create("./kernelConsola.log", "consola", 1, LOG_LEVEL_INFO);
	char* variable;

	while(1){
		log_info(logger_consola,"ingrese la operacion que deseas realizar"
				"\n 1. iniciar Proceso"
				"\n 2. finalizar proceso"
				"\n 3. iniciar Planificacion"
				"\n 4. detener Planificacion"
				"\n 5. modificar grado multiprogramacion terminar programa"
				"\n 6. hacer que cpu mande mensaje a memoria"
				"\n 7. generar conexion"
				"\n 8. enviar mensaje");
		variable = readline(">");

		switch (*variable) {
			case '1':
				log_info(logger_consola, "ingrese la ruta");
				char* ruta = readline(">");
				log_info(logger_consola, "ingrese el tamanio");
				int size = atoi(readline(">"));
				log_info(logger_consola, "ingrese el prioridad");
				int prioridad = atoi(readline(">"));

				iniciar_proceso(ruta,size,prioridad,contador_pid);
				break;
			case '2':
				log_info(logger_consola, "ingrese pid");
				char* valor = readline(">");
				int valorNumero = atoi(valor);
				t_pcb * pcb = encontrar_pcb(valorNumero);
				terminar_proceso(pcb);
				break;
			case '3':
				iniciar_planificacion();
				break;
			case '4':
				detener_planificacion_corto_largo();
				break;
			case '5':
				modificar_grado_multiprogramacion();
				break;
			case '6':
				//listar_proceso_estado();
				break;
			case '7':
				generar_conexion();
				break;
			case '8':
				enviar_mensaje_kernel();
				break;
			case '9':
				crear_pcb(FIFO);
				log_info(logger,"%i",list_size(cola_new));
				enviar_pcb(queue_pop(cola_new),conexion_cpu,RECIBIR_PCB);
				break;
			default:
				log_info(logger_consola,"no corresponde a ninguno");
		}
		free(variable);


	}

}
void *ejecutar_sleep(void *arg) {
    t_datos_sleep *datos = (t_datos_sleep *)arg;
    usleep(datos->tiempo * 1000000);
    t_pcb * pcb = buscar_lista(datos->pcb->pid, lista_sleep);
    agregar_a_cola_ready(pcb);
    sem_post(&contador_cola_ready);
    free(datos);
    return NULL;
}
void iniciar_recurso(){
	lista_pcb=list_create();
	cola_new = queue_create();
	pcb_en_ejecucion = list_create();
    lista_recursos_pcb = list_create();
    list_bloqueado_page_fault = list_create();
    lista_sleep = list_create();
	//TODO cambiar por grado init
	sem_init(&grado_multiprogramacion, 0, 10);
	sem_init(&mutex_cola_new, 0, 1);
	sem_init(&contador_ejecutando_cpu,0,1);
	sem_init(&mutex_cola_ready,0,1);
	sem_init(&contador_agregando_new,0,0);
	sem_init(&contador_cola_ready,0,0);
	sem_init(&proceso_desalojo,0,0);
	sem_init(&sem_deadlock,0,0);
    pthread_mutex_init(&mutex_lista_ejecucion, 0);
    sem_init(&cont_detener_planificacion,0,0);
    detener = false;
}

void enviar_mensaje_kernel() {
	log_info(logger_consola,"ingrese q que modulos deseas mandar mensaje"
			"\n 1. modulo memoria"
			"\n 2. modulo cpu"
			"\n 3. modulo filesystem"
			"\n 4. modulo cpu_interrupt");
    char *valor = readline(">");
	switch (*valor) {
		case '1':
	        enviar_mensaje("kernel a memoria", conexion_memoria);
	        log_info(logger_consola,"mensaje enviado correctamente\n");
			break;
		case '2':
	        enviar_mensaje("kernel a cpu", conexion_cpu);
	        log_info(logger_consola,"mensaje enviado correctamente\n");
			break;
		case '3':
	        enviar_mensaje("kernel a filesystem", conexion_file_system);
	        log_info(logger_consola,"mensaje enviado correctamente\n");
			break;
		case '4':
	        enviar_mensaje_instrucciones("kernel a interrupt", conexion_cpu_interrupt,ENVIAR_DESALOJAR);
	       // enviar_interrupciones(conexion_cpu_interrupt,ENVIAR_DESALOJAR);
	        log_info(logger_consola,"mensaje enviado correctamente\n");
	        break;
		default:
			log_info(logger_consola,"no corresponde a ninguno\n");
			break;
	}
}

void generar_conexion() {
	pthread_t conexion_memoria_hilo;
	pthread_t conexion_file_system_hilo;
	pthread_t conexion_cpu_hilo;
	pthread_t conexion_cpu_interrupt_hilo;

	log_info(logger_consola,"ingrese q que modulos deseas conectar"
			"\n 1. modulo memoria"
			"\n 2. modulo filesystem"
			"\n 3. modulo cpu"
			"\n 4. modulo cpu interrupt");

    char *valor = readline(">");
	switch (*valor) {
		case '1':

			conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
			pthread_create(&conexion_cpu_hilo,NULL,(void*) procesar_conexion,(void *)&conexion_memoria);
	        log_info(logger_consola,"conexion generado correctamente\n");
			break;
		case '2':
			conexion_file_system = crear_conexion(ip_filesystem, puerto_filesystem);
	        log_info(logger_consola,"conexion generado correctamente\n");
			break;
		case '3':
			conexion_cpu = crear_conexion(ip_cpu, puerto_cpu_dispatch);
	        log_info(logger_consola,"conexion generado correctamente\n");
			pthread_create(&conexion_cpu_hilo,NULL,(void*) procesar_conexion,(void *)&conexion_cpu);
			conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);
			pthread_create(&conexion_cpu_interrupt_hilo, NULL, (void*) procesar_conexion, (void *)&conexion_cpu_interrupt_hilo);
	        log_info(logger_consola,"conexion generado correctamente\n");
			break;
		default:
			log_info(logger_consola,"no corresponde a ninguno\n");
			break;
	}

}


//hilo que espere consola,
void iniciar_proceso(char* archivo_test,int size,int prioridad,int pid){

	//char* prueba = ruta_archivo_test;
	//string_append(*prueba, archivo_test);

	char*ruta_a_testear = archivo_test;
	op_code op = INICIAR_PROCESO;
	t_paquete* paquete =crear_paquete(op);
	agregar_a_paquete(paquete, ruta_a_testear, strlen(ruta_a_testear) + 1);
	agregar_a_paquete(paquete, &size ,sizeof(int));
	agregar_a_paquete(paquete, &prioridad, sizeof(int));
	agregar_a_paquete(paquete, &pid, sizeof(int));

	enviar_paquete(paquete, conexion_memoria);
	crear_pcb(prioridad);
	sem_post(&contador_agregando_new);
	//free(prueba);
	eliminar_paquete(paquete);
	free(ruta_a_testear);

}

void crear_pcb(int prioridad){
	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->pid= contador_pid;
	pcb->prioridad = prioridad;
	t_contexto_ejecucion* contexto = crear_contexto();
	pcb->contexto =contexto;
	//pcb->tabla_archivo_abierto;
	contador_pid++;
	log_pcb_info(pcb);
	log_info(logger_consola,"Se crea el proceso %i en NEW",pcb->pid);
	agregar_a_cola_new(pcb);

}

t_contexto_ejecucion* crear_contexto(){
	t_contexto_ejecucion* contexto = malloc(sizeof(t_contexto_ejecucion));
	contexto->pc =0;
	t_registro_cpu* registro = crear_registro();
	contexto->registros_cpu = registro;
	return contexto;
}

t_registro_cpu* crear_registro(){
    t_registro_cpu* reg = malloc(sizeof(t_registro_cpu));
    reg->ax = 0;
    reg->bx = 0;
    reg->cx = 0;
    reg->dx = 0;
    return reg;
}


void agregar_a_cola_new(t_pcb* pcb){
	sem_wait(&mutex_cola_new);
	queue_push(cola_new,pcb);
	sem_post(&mutex_cola_new);
	log_info(logger,"El proceso [%d] fue agregado a la cola new",pcb->pid);
}

t_pcb* quitar_de_cola_new(){
	sem_wait(&mutex_cola_new);
	t_pcb* pcb=queue_pop(cola_new);
	sem_post(&mutex_cola_new);
	log_info(logger,"El proceso [%d] fue quitado de la cola new",pcb->pid);
	return pcb;
}
void agregar_a_cola_ready(t_pcb* pcb){
	sem_wait(&mutex_cola_ready);
	queue_push(cola_ready,pcb);
	char* estado_anterior = estado_a_string(pcb->estado);
	log_info(logger, "PID: %i - Estado Anterior: %s - Estado Actual: READY",pcb->pid,estado_anterior);
	pcb->estado=READY;
	sem_post(&mutex_cola_ready);
	//mostrar_pid_cola_ready();
}
void mostrar_pid_cola_ready(){
	int i = queue_size(cola_ready);
	char *pids;
	for(int c=0;c<i;c++){
		char * pid;
		 sprintf(pid,"%d ", list_get(cola_ready->elements,c));
		 strcat(pids, pid);
	}
	log_info(logger,"Cola Ready %s: [%s]",planificador,pids);
}
void agregar_a_inicio_cola_ready(t_pcb* pcb){
	sem_wait(&mutex_cola_ready);
	list_add_in_index(cola_ready,0,pcb);

	pcb->estado=READY;
	sem_post(&mutex_cola_ready);
	log_info(logger,"El proceso [%d] fue agregado a la cola ready",pcb->pid);
}
t_pcb* quitar_de_cola_ready(){
	sem_wait(&mutex_cola_ready);
	t_pcb* pcb=queue_pop(cola_ready);
	sem_post(&mutex_cola_ready);
	log_info(logger,"PID: %i - Estado Anterior: READY - Estado Actual: RUNNING",pcb->pid);
	return pcb;
}

void planificador_largo_plazo(){
	while(1){
		if(detener){
			//sem_wait(&cont_detener_planificacion);
			break;
		}
		sem_wait(&contador_agregando_new);
		sem_wait(&grado_multiprogramacion);
		t_pcb* pcb =quitar_de_cola_new();
		log_info(logger, "el pid del proceso es %i",pcb->pid);
		agregar_a_cola_ready(pcb);
		sem_post(&contador_cola_ready);
	}
}

//TODO MOTIVO DE QUE DESPUES DE INICIAR PLANIFICACION NO ME DEJA INGRESAR OTRA OPERACION
void planificador_corto_plazo(){
	while(1){
		if(detener){
			break;
		}

		log_info(logger,"EJECUTANDO UN NUEVO PROCESO");
		sem_wait(&contador_cola_ready);
		//sem_wait(&contador_ejecutando_cpu);
		switch(planificador){
		case FIFO:
			if(!queue_is_empty(cola_ready)){
				sem_wait(&contador_ejecutando_cpu);
				log_info(logger,"Planificador FIFO");
				de_ready_a_fifo();
			}
			break;
		case ROUND_ROBIN:
			if(!queue_is_empty(cola_ready)){
			sem_wait(&contador_ejecutando_cpu);
			log_info(logger,"Planificador Round Robin");
			de_ready_a_round_robin();
			}
			break;
		case PRIORIDADES:
			log_info(logger,"Planificador Prioridades");
			de_ready_a_prioridades();
			break;
		}
	}
}

void de_ready_a_fifo(){
	t_pcb* pcb =quitar_de_cola_ready();
    enviar_por_dispatch(pcb);
}

//TODO c
/*
void de_ready_a_prioridades(){

    list_sort(cola_ready->elements,comparador_prioridades);
    t_pcb* pcb_a_comparar_prioridad = queue_peek(cola_ready);

    if(list_is_empty(pcb_en_ejecucion)){
        //list_sort(cola_ready->elements,comparador_prioridades);
		sem_wait(&contador_ejecutando_cpu);
		de_ready_a_fifo();
    }else{
    		t_pcb* pcb_aux = list_get(pcb_en_ejecucion,0);
    		//t_pcb* pcb_axu_comparador = queue_pop(cola_ready);
    		log_info(logger,"el valor que esta ejecutando es %i",pcb_aux->prioridad);
    		//log_info(logger,"el valor que esta comparando es %i",pcb_axu_comparador->prioridad);
    		if(pcb_aux->prioridad<pcb_a_comparar_prioridad->prioridad){
    			log_info(logger,"HAYY DESALOJOOOOOOO");
    	        enviar_mensaje_instrucciones("kernel a interrupt", conexion_cpu_interrupt,ENVIAR_DESALOJAR);
    			sem_wait(&contador_ejecutando_cpu);
    			de_ready_a_fifo();
    		}
    }
}
*/

void de_ready_a_round_robin(){

	de_ready_a_fifo();
	usleep(quantum *1000);
	enviar_mensaje_instrucciones("interrumpido por quantum",conexion_cpu_interrupt,ENVIAR_DESALOJAR);
}


void de_ready_a_prioridades(){

	if(!queue_is_empty(cola_ready)){

    list_sort(cola_ready->elements,comparador_prioridades);
    t_pcb* pcb_a_comparar_prioridad = queue_peek(cola_ready);

    if(list_is_empty(pcb_en_ejecucion)){
    	sem_wait(&contador_ejecutando_cpu);
        list_sort(cola_ready->elements,comparador_prioridades);
		de_ready_a_fifo();
    }else{
    		t_pcb* pcb_aux = list_get(pcb_en_ejecucion,0);
    		//t_pcb* pcb_axu_comparador = queue_pop(cola_ready);
    		log_info(logger,"el valor que esta ejecutando es %i",pcb_aux->prioridad);
    		//log_info(logger,"el valor que esta comparando es %i",pcb_axu_comparador->prioridad);
    		if(pcb_aux->prioridad<pcb_a_comparar_prioridad->prioridad){
    			log_info(logger,"hubo desalojo");
    	        enviar_mensaje_instrucciones("kernel a interrupt", conexion_cpu_interrupt,ENVIAR_DESALOJAR);
    	        sem_wait(&proceso_desalojo);
    	        sem_post(&contador_cola_ready);
    		}
		}
    }
}

bool comparador_prioridades(void* caso1,void* caso2){
	t_pcb* pcb1 = ((t_pcb*) caso1);
	t_pcb* pcb2 = ((t_pcb*) caso2);
	if(pcb1->prioridad > pcb2->prioridad){
		return true;
	} else return false;
}

bool controlador_multi_programacion(){
	return list_size(lista_pcb)<grado_multiprogramacion_ini;
}

void enviar_por_dispatch(t_pcb* pcb) {
    pcb->estado=RUNNING;

    pthread_mutex_lock(&mutex_lista_ejecucion);
    list_add(pcb_en_ejecucion, pcb);
    pthread_mutex_unlock(&mutex_lista_ejecucion);

    enviar_pcb(pcb,conexion_cpu,RECIBIR_PCB);

    log_info(logger,"El proceso [%d] fue pasado al estado de running y enviado al CPU",pcb->pid);
}



t_contexto_ejecucion* obtener_contexto(char* archivo){
	t_contexto_ejecucion *estructura_retornar ;

	return estructura_retornar;
}


void finalizar_proceso(int pid){

	t_paquete * paquete = crear_paquete(FINALIZAR);
	agregar_a_paquete(paquete, &pid, sizeof(int));
	//log_info(logger,"PID: <%i> - Estado Anterior: <%s> - Estado Actual: <TERMINATED>",pcb->pid,pcb->estado);
	enviar_paquete(paquete, conexion_memoria);
	eliminar_paquete(paquete);
	free(paquete);




	//int posicion= buscarPosicionQueEstaElPid(pid);
	//t_pcb* auxiliar =list_remove(cola_new->elements,posicion);
	//liberarMemoriaPcb(auxiliar);
}

void liberarMemoriaPcb(t_pcb* pcbABorrar){
		free(pcbABorrar->contexto);
		//free(pcbABorrar->tabla_archivo_abierto);
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


void iniciar_planificacion(){
	detener = false;
	pthread_t * hilo_corto_plazo;
	pthread_t * hilo_largo_plazo;
	log_info(logger_consola,"inicio el proceso de planificacion");
	pthread_create(&hilo_largo_plazo,NULL,(void*) planificador_largo_plazo,NULL);
	pthread_create(&hilo_corto_plazo,NULL,(void*) planificador_corto_plazo,NULL);
	pthread_detach(*hilo_largo_plazo);
	pthread_detach(*hilo_corto_plazo);
	log_info(logger_consola, "llego hasta aca asddddddddddddddd");

}


void detener_planificacion_corto_largo(){
	detener= true;
}
void modificar_grado_multiprogramacion(){
    terminar_programa(conexion_memoria, logger, config);
    terminar_programa(conexion_cpu, logger, config);
    terminar_programa(conexion_file_system, logger, config);

}


void obtener_configuracion(){

    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    ip_filesystem = config_get_string_value(config, "IP_FILESYSTEM");
    ip_cpu = config_get_string_value(config, "IP_CPU");
    char *algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    asignar_algoritmo(algoritmo);

    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    puerto_filesystem = config_get_string_value(config, "PUERTO_FILESYSTEM");
    puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    quantum = config_get_int_value(config, "QUANTUM");
    grado_multiprogramacion_ini = config_get_int_value(config, "GRADO_MULTIPROGRAMACION_INI");
    recursos_config = config_get_array_value(config, "RECURSOS");
    instancias_recursos_config = config_get_array_value(config, "INSTANCIAS_RECURSOS");

}

void asignar_algoritmo(char *algoritmo){
	if (strcmp(algoritmo, "FIFO") == 0) {
		planificador = FIFO;
	} else if (strcmp(algoritmo, "ROUND_ROBIN") == 0) {
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
void ejecutar_wait(char*nombre,t_pcb*pcb){
	if (list_is_empty(lista_recursos)) {
	    return;
	}
	t_list_iterator* iterador = list_iterator_create(lista_recursos);
	int j=0;
	int encontro =0;
	while(list_iterator_has_next(iterador)){
		t_recurso* recurso = (t_recurso*)list_iterator_next(iterador);
		if(strcmp(nombre,recurso->nombre) == 0){
			encontro = 1;
			if(recurso->instancias >0){
				recurso->instancias--;
				list_replace(lista_recursos,j,recurso);
				agregar_recurso_pcb(pcb->pid,nombre);
				log_info(logger,"PID: %i - Wait: %s - Instancias: %i",pcb->pid,recurso->nombre,recurso->instancias);
				enviar_pcb(pcb,conexion_cpu,RECIBIR_PCB);
				break;
			}else{
				pcb->estado = WAITING;
				log_info(logger,"PID: %i - Estado Anterior: RUNNING - Estado Actual: WAITING",pcb->pid);
				log_info(logger ,"PID: %i - Bloqueado por: %s",pcb->pid,nombre);
				queue_push(recurso->cola_bloqueados  ,pcb);
				if(!list_is_empty(pcb_en_ejecucion)){
					list_remove(pcb_en_ejecucion,0);
				}
				sem_post(&contador_ejecutando_cpu);
				sem_post(&contador_cola_ready);
				sem_post(&sem_deadlock);
				//list_add(lista_bloqueados,pcb);

				break;
			}
		}
		j++;
	}
	if(encontro ==0){
		terminar_proceso(pcb);
		log_info(logger, "Finaliza el proceso %i - Motivo: INVALID_RESOURCE",pcb->pid);
	}
	list_iterator_destroy(iterador);
}

void ejecutar_signal(char*nombre,t_pcb*pcb){
	if (list_is_empty(lista_recursos)) {
	    return;
	}
	t_list_iterator* iterador = list_iterator_create(lista_recursos);
	int j=0;
	int encontro =0;
	while(list_iterator_has_next(iterador)){
		t_recurso* recurso = (t_recurso*)list_iterator_next(iterador);
		if(strcmp(nombre,recurso->nombre) == 0){
			encontro = 1;
			t_recurso_pcb * recurso_pcb = buscar_recurso_pcb(nombre, pcb->pid);
			if(recurso_pcb != NULL){
				recurso->instancias++;
				list_replace(lista_recursos,j,recurso);
				quitar_recurso_pcb(pcb->pid,nombre);
				log_info(logger,"PID: %i - Signal: %s - Instancias: %i",pcb->pid,recurso->nombre,recurso->instancias);
				enviar_pcb(pcb,conexion_cpu,RECIBIR_PCB);
				if(!queue_is_empty(recurso->cola_bloqueados)){
					t_pcb* pcb_bloqueado = queue_pop(recurso->cola_bloqueados);
					agregar_a_cola_ready(pcb_bloqueado);
					sem_post(&contador_cola_ready);
				}
			}else{
				terminar_proceso(pcb);
				log_info(logger, "Finaliza el proceso %i - Motivo: INVALID_RESOURCE",pcb->pid);
			}
		}
		j++;
	}
	if(encontro ==0){
		terminar_proceso(pcb);
		log_info(logger, "Finaliza el proceso %i - Motivo: INVALID_RESOURCE",pcb->pid);
	}
	list_iterator_destroy(iterador);
}

t_recurso_pcb*buscar_recurso_pcb(char*nombre,int pid){
	t_list_iterator* iterador = list_iterator_create(lista_recursos_pcb);
	int encontro =0;
	while(list_iterator_has_next(iterador)){
		t_recurso_pcb* recurso = (t_recurso_pcb*)list_iterator_next(iterador);
		if((strcmp(nombre,recurso->nombre) == 0)&&(pid == recurso->pid)){
			encontro=1;
			list_iterator_destroy(iterador);
			return recurso;
		}
	}
	if(encontro == 0){
		list_iterator_destroy(iterador);
		return NULL;
	}
}
void agregar_recurso_pcb(int pid, char*nombre){
	t_list_iterator* iterador = list_iterator_create(lista_recursos_pcb);
	int j=0;
	int encontro =0;
	while(list_iterator_has_next(iterador)){
		t_recurso_pcb* recurso = (t_recurso_pcb*)list_iterator_next(iterador);
		if((strcmp(nombre,recurso->nombre) == 0) && (pid == recurso->pid)){
			encontro = 1;
			recurso->instancias++;
			list_replace(lista_recursos_pcb, j, recurso);
		}
		j++;
	}
	if(encontro ==0){
		t_recurso_pcb* recurso = crear_recurso_pcb(nombre, pid);
		list_add(lista_recursos_pcb, recurso);
	}
	list_iterator_destroy(iterador);
}

t_recurso_pcb*crear_recurso_pcb(char*nombre,int pid){
	t_recurso_pcb*recurso_nuevo=malloc(sizeof(t_recurso_pcb));
	recurso_nuevo->nombre = nombre;
	recurso_nuevo->instancias=1;
	recurso_nuevo->pid = pid;
	log_info(logger,"Se creo el recurso %s", recurso_nuevo->nombre);
	return recurso_nuevo;
}
void quitar_recurso_pcb(int pid, char*nombre){
	t_list_iterator* iterador = list_iterator_create(lista_recursos_pcb);
	int j=0;
	while(list_iterator_has_next(iterador)){
		t_recurso_pcb* recurso = (t_recurso_pcb*)list_iterator_next(iterador);
		if((strcmp(nombre,recurso->nombre) == 0) && pid == recurso->pid){
			if(recurso->instancias > 1){
				recurso->instancias--;
				list_replace(lista_recursos_pcb, j, recurso);
			}else{
				list_remove(lista_recursos_pcb,j);
				free(recurso);
			}
		}
		j++;
	}
	list_iterator_destroy(iterador);
}

void liberar_recursos(int pid){

	t_list_iterator* iterador = list_iterator_create(lista_recursos);

		int j=0;
		while(list_iterator_has_next(iterador)){
			t_recurso* recurso = (t_recurso*)list_iterator_next(iterador);
			t_recurso_pcb * recurso_pcb = buscar_recurso_pcb(recurso->nombre,pid);

			if(recurso_pcb != NULL){
				int instancias = recurso_pcb->instancias;
				while(instancias!=0){
					if(!queue_is_empty(recurso->cola_bloqueados)){
						t_pcb* pcb = queue_pop(recurso->cola_bloqueados);
						agregar_a_cola_ready(pcb);
						sem_post(&contador_cola_ready);
					}
					recurso->instancias++;
					list_replace(lista_recursos,j,recurso);
					quitar_recurso_pcb(pid,recurso->nombre);
					log_info(logger,"Libere el recurso %s",recurso->nombre);
					instancias --;
				}
			}
			j++;
		}
		list_iterator_destroy(iterador);
}

void deadlock(){
	if(list_is_empty(pcb_en_ejecucion) && list_is_empty(cola_ready) && !list_is_empty(lista_recursos_pcb)){
		t_list_iterator* iterador = list_iterator_create(lista_recursos);
		int j=0;
		log_info(logger,"ANÁLISIS DE DETECCIÓN DE DEADLOCK");
		while(list_iterator_has_next(iterador)){
			t_recurso* recurso = (t_recurso*)list_iterator_next(iterador);
			if(!queue_is_empty(recurso->cola_bloqueados)){
				int t = queue_size(recurso->cola_bloqueados);
				for(int i =0; i < t; i++){
					t_pcb* pcb = list_get(recurso->cola_bloqueados->elements, i);
					log_info(logger,"Deadlock detectado: %i - Recursos en posesión: ",pcb->pid);
					mostrar_recursos_pcb(pcb->pid);
					log_info(logger, "- Recurso requerido:%s",recurso->nombre);
				}
			}
		}
		list_iterator_destroy(iterador);
	}
}

void mostrar_recursos_pcb(int pid){
	t_list_iterator* iterador = list_iterator_create(lista_recursos_pcb);
		while(list_iterator_has_next(iterador)){
			t_recurso_pcb* recurso = (t_recurso_pcb*)list_iterator_next(iterador);
			if(recurso->pid == pid){
				log_info(logger,"%s ,", recurso->nombre);
			}
		}
		list_iterator_destroy(iterador);
}
void terminar_proceso(t_pcb * pcb){
	char* estado_anterior = estado_a_string(pcb->estado);
	pcb->estado = TERMINATED;
	log_info(logger,"PID: %i - Estado Anterior: %s - Estado Actual: TERMINATED",pcb->pid,estado_anterior);
	liberar_recursos(pcb->pid);
	enviar_pcb(pcb,conexion_memoria,FINALIZAR);
	if(!list_is_empty(pcb_en_ejecucion)){
		list_remove(pcb_en_ejecucion,0);
	}
	sem_post(&grado_multiprogramacion);
	sem_post(&contador_ejecutando_cpu);
	sem_post(&contador_cola_ready);
}

t_pcb * encontrar_pcb(int pid){
	t_pcb *bloqueado = buscar_pcb_bloqueados(pid);
	if(bloqueado!=NULL){
		return bloqueado;
	}
	t_pcb * ready = buscar_pcb_colas(pid, cola_ready);
	if(ready!=NULL){
		return ready;
	}
	t_pcb * running = buscar_pcb_colas(pid, cola_ejecucion);
	if(running!=NULL){
		return running;
	}
	t_pcb * sleep = buscar_lista(pid, lista_sleep);
	if(sleep != NULL){
		return sleep;
	}
	t_pcb * page_fault = buscar_lista(pid,list_bloqueado_page_fault);
	if(page_fault != NULL){
		return page_fault;
	}
	return NULL;
}
t_pcb * buscar_pcb_colas(int pid, t_queue * cola){
	int d = queue_size(cola);
	if(d>0){
		for(int c = 0; c<d;c++){
			t_pcb * pcb = list_get(cola->elements,c);
			if(pid == pcb->pid){
				list_remove(cola->elements,c);
				return pcb;
			}
		}
	}
	return NULL;
}

t_pcb*buscar_pcb_bloqueados(int pid){
	t_list_iterator* iterador = list_iterator_create(lista_recursos);
	int d =0;
	while(list_iterator_has_next(iterador)){
		t_recurso* recurso = (t_recurso*)list_iterator_next(iterador);
		int i = queue_size(recurso->cola_bloqueados);
		if(i>0){
			for(int c = 0; c<i;c++){
				t_pcb * pcb = list_get(recurso->cola_bloqueados->elements,c);
				if(pid == pcb->pid){
					list_iterator_destroy(iterador);
					list_remove(recurso->cola_bloqueados->elements,c);
					list_replace(lista_recursos,d,recurso);
					return pcb;
				}
			}
		}
		d++;
	}
	list_iterator_destroy(iterador);
	return NULL;
}
t_pcb * buscar_lista(int pid,t_list *lista){
	t_list_iterator* iterador = list_iterator_create(lista);
	int d =0;
	while(list_iterator_has_next(iterador)){
		t_pcb* pcb = (t_pcb*)list_iterator_next(iterador);
		if(pcb->pid == pid){
			list_iterator_destroy(iterador);
			list_remove(lista,d);
			return pcb;
		}
		d++;
	}
	list_iterator_destroy(iterador);
	return NULL;
}


char* estado_a_string(t_estado estado) {
    switch (estado) {
        case NEW:
            return "NEW";
        case READY:
            return "READY";
        case RUNNING:
            return "RUNNING";
        case WAITING:
            return "WAITING";
        case TERMINATED:
            return "TERMINATED";
        default:
            return "Estado desconocido";
    }
}
