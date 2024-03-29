#include "kernel.h"
#include<readline/readline.h>

int main(int argc, char **argv){


	char *rutaConfig = "kernel.config";
	config = cargar_config(rutaConfig);

    logger = log_create("./kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el Kernel!");

    obtener_configuracion();
    iniciar_recurso();
<<<<<<< HEAD
    lista_recursos = list_create();
    	t_recurso * nuevo_recurso = malloc(sizeof(t_recurso));
    	nuevo_recurso->instancias = 3;
    	nuevo_recurso->nombre = strdup("RA");
    	nuevo_recurso->cola_bloqueados = queue_create();
    	//sem_init(&nuevo_recurso->sem_recurso,0,0);

    	log_info(logger,"El nombre del nuevo recurso es %s",nuevo_recurso->nombre);
    	//nuevo_recurso->cola_bloqueados = queue_create();

    	list_add(lista_recursos,nuevo_recurso);

    	t_recurso *recurso = list_get(lista_recursos,0);
    	log_info(logger,"El nombre del recurso es %s",recurso->nombre);
=======
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
    iniciar_consola();

    //envio de mensajes

    //error
    //paquete(conexion_memoria);
<<<<<<< HEAD
=======
    pthread_t servidor;


    terminar_programa(conexion_memoria, logger, config);
    terminar_programa(conexion_cpu, logger, config);
    terminar_programa(conexion_file_system, logger, config);

>>>>>>> parent of ca1d82c (borre todo para mergear rama)

    return EXIT_SUCCESS;
}


<<<<<<< HEAD

void procesar_conexion(void *conexion1){
	int *conexion = (int*)conexion1;
	int cliente_fd = *conexion;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		t_pcb* pcb_aux;
		t_list* paquete;
=======
void procesar_conexion(void *conexion1){
	int *conexion = (int*)conexion1;
	int cliente_fd = *conexion;

	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		t_pcb* pcb_aux;
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
<<<<<<< HEAD
		case RECIBIR_PCB:
			paquete = recibir_paquete(cliente_fd);
			pcb_aux = desempaquetar_pcb(paquete);
			log_info(logger,"recibi el pcb");
			log_pcb_info(pcb_aux);
			recv(cliente_fd,&cod_op,sizeof(op_code),0);
			switch(cod_op){
			case EJECUTAR_SLEEP:

				paquete = recibir_paquete(cliente_fd);
				int *tiempo = list_get(paquete,0);
				log_info(logger,"recibi el sleep %d",*tiempo);
				list_remove(pcb_en_ejecucion,0);
				sem_post(&contador_ejecutando_cpu);
				usleep(*tiempo*1000);
				agregar_a_cola_ready(pcb_aux);
				sem_post(&contador_cola_ready);
				break;
			case EJECUTAR_WAIT:
//				paquete = recibir_paquete(cliente_fd);
//				pcb_aux = desempaquetar_pcb(paquete);
				char * nombre_recurso = "RB";//obtener_mensaje(cliente_fd);
	//			if(recurso != NULL){
	//				log_info(logger,"esta vaciooooooooooooooooooo");
	//			}
				int instacias;
				ejecutar_wait(nombre_recurso,pcb_aux);
				break;
			case EJECUTAR_SIGNAL:
//				paquete = recibir_paquete(cliente_fd);
//				pcb_aux = desempaquetar_pcb(paquete);
				char * nombre_recurso2 = "RA";// obtener_mensaje(cliente_fd);
				t_recurso *recurso3 = list_get(lista_recursos,0);
				instacias= recurso3->instancias;
				log_info(logger,"ejecutando signal %i",instacias);
				ejecutar_signal(nombre_recurso2,pcb_aux);
				t_recurso *recurso4 = list_get(lista_recursos,0);
				instacias= recurso4->instancias;
				log_info(logger,"ejecutando signal %i",instacias);
				break;
			case EJECUTAR_F_TRUNCATE:
			    log_info(logger, "me llegó la instrucción ejecutar ftruncate del CPU");
			    // Agrega aquí la lógica para ejecutar la operación F_TRUCATE
			    paquete = recibir_paquete(cliente_fd);
				list_remove(pcb_en_ejecucion,0);
			    sem_post(&contador_ejecutando_cpu);
			    agregar_a_cola_ready(pcb_aux);
			    sem_post(&contador_cola_ready);
			    break;

			case EJECUTAR_F_OPEN:
			    log_info(logger, "me llegó la instrucción ejecutar fopen del CPU");
			    paquete = recibir_paquete(cliente_fd);
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
				list_remove(pcb_en_ejecucion,0);
			    sem_post(&contador_ejecutando_cpu);
			    agregar_a_cola_ready(pcb_aux);
			    sem_post(&contador_cola_ready);
			    break;

			case EJECUTAR_F_SEEK:
			    log_info(logger, "me llegó la instrucción ejecutar fseek del CPU");
			    // Agrega aquí la lógica para ejecutar la operación F_SEEK
			    paquete = recibir_paquete(cliente_fd);
				list_remove(pcb_en_ejecucion,0);
			    sem_post(&contador_ejecutando_cpu);
			    agregar_a_cola_ready(pcb_aux);
			    sem_post(&contador_cola_ready);
			    break;

			case EJECUTAR_F_READ:
			    log_info(logger, "me llegó la instrucción ejecutar fread del CPU");
			    // Agrega aquí la lógica para ejecutar la operación F_READ
			    paquete = recibir_paquete(cliente_fd);
				list_remove(pcb_en_ejecucion,0);
			    sem_post(&contador_ejecutando_cpu);
			    agregar_a_cola_ready(pcb_aux);
			    sem_post(&contador_cola_ready);
			    break;

			case EJECUTAR_F_WRITE:
			    log_info(logger, "me llegó la instrucción ejecutar fwrite del CPU");
			    // Agrega aquí la lógica para ejecutar la operación F_WRITE
			    paquete = recibir_paquete(cliente_fd);
				list_remove(pcb_en_ejecucion,0);
			    sem_post(&contador_ejecutando_cpu);
			    agregar_a_cola_ready(pcb_aux);
			    sem_post(&contador_cola_ready);
			    break;

			default:
				log_error(logger, "che %s no se que me mandaste", cliente_fd);
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
			log_info(logger,"el pid del proceso finalizado es %i",pcb_aux->pid);
			//TODO VER SI NECESITA UNA LISTA PARA LAMACENAR LOS PROCESOS TERMINADO
			enviar_mensaje("hola se finalizo el proceso ", conexion);
			pcb_aux->estado = TERMINATED;
			log_pcb_info(pcb_aux);
			liberar_recursos(pcb_aux->pid);
			enviar_pcb(pcb_aux,conexion_memoria,FINALIZAR);
			if(!list_is_empty(pcb_en_ejecucion)){
				list_remove(pcb_en_ejecucion,0);
			}
			sem_post(&grado_multiprogramacion);
			sem_post(&contador_ejecutando_cpu);
			sem_post(&contador_cola_ready);
			break;
=======
		case EJECUTAR_SLEEP:
			pcb_aux = recibir_pcb(cliente_fd);
			int tiempo = atoi(obtener_mensaje(cliente_fd));
			sleep(tiempo);
			break;
		case EJECUTAR_WAIT:
			pcb_aux = recibir_pcb(cliente_fd);
			char * nombre_recurso = obtener_mensaje(cliente_fd);
			ejecutar_wait(nombre_recurso,pcb_aux);
			break;
		case EJECUTAR_SIGNAL:
			pcb_aux = recibir_pcb(cliente_fd);
			char * nombre_recurso2 = obtener_mensaje(cliente_fd);
			ejecutar_signal(nombre_recurso2,pcb_aux);
			break;
		case EJECUTAR_F_TRUNCATE:
			log_info(logger,"me llegaron la instruccion ejecutar f truncate del cpu");
			break;
		case FINALIZAR:
			pcb_aux = recibir_pcb(cliente_fd);
			enviar_pcb(pcb_aux,conexion_memoria,FINALIZAR);
			sem_post(&contador_ejecutando_cpu);
			break;

>>>>>>> parent of ca1d82c (borre todo para mergear rama)
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

<<<<<<< HEAD
=======

>>>>>>> parent of ca1d82c (borre todo para mergear rama)
void iniciar_consola(){
	logger_consola = log_create("./kernelConsola.log", "consola", 1, LOG_LEVEL_INFO);
	char* variable;

	while(1){
		log_info(logger_consola,"ingrese la operacion que deseas realizar"
				"\n 1. iniciar Proceso"
				"\n 2. finalizar proceso"
				"\n 3. iniciar Planificacion"
				"\n 4. detener Planificacion"
<<<<<<< HEAD
				"\n 5. modificar grado multiprogramacion terminar programa"
=======
				"\n 5. modificar grado multiprogramacion"
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
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
				finalizar_proceso(valorNumero);
				break;
			case '3':
				iniciar_planificacion();
				break;
			case '4':
				detener_planificacion();
				break;
			case '5':
				modificar_grado_multiprogramacion();
				break;
			case '6':
				listar_proceso_estado();
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
				exit(2);
		}
		free(variable);


	}

}

void iniciar_recurso(){
	lista_pcb=list_create();
	cola_new = queue_create();
	cola_ready = queue_create();
<<<<<<< HEAD
	pcb_en_ejecucion = list_create();
    lista_recursos_pcb = list_create();
    lista_bloqueados = list_create();
	log_info(logger,"llegue");
	//TODO cambiar por grado init
	sem_init(&grado_multiprogramacion, 0, 10);
	sem_init(&mutex_cola_new, 0, 1);
	sem_init(&contador_ejecutando_cpu,0,1);
	sem_init(&mutex_cola_ready,0,1);
	sem_init(&contador_agregando_new,0,0);
	sem_init(&contador_cola_ready,0,0);
	sem_init(&proceso_desalojo,0,0);
    pthread_mutex_init(&mutex_lista_ejecucion, 0);
    pthread_t deadlock;
    pthread_create(&deadlock,NULL,(void*) detect_deadlock,NULL);
=======
	lista_recursos = list_create();
	sem_init(&grado_multiprogramacion, 0, grado_multiprogramacion_ini);
	sem_init(&mutex_cola_new, 0, 1);
	sem_init(&contador_ejecutando_cpu,0,1);
	sem_init(&mutex_cola_ready,0,1);


>>>>>>> parent of ca1d82c (borre todo para mergear rama)
}

void enviar_mensaje_kernel() {
	log_info(logger_consola,"ingrese q que modulos deseas mandar mensaje"
			"\n 1. modulo memoria"
			"\n 2. modulo cpu"
<<<<<<< HEAD
			"\n 3. modulo filesystem"
			"\n 4. modulo cpu_interrupt");
=======
			"\n 3. modulo filesystem");
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
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
<<<<<<< HEAD
		case '4':
	        enviar_mensaje_instrucciones("kernel a interrupt", conexion_cpu_interrupt,ENVIAR_DESALOJAR);
	       // enviar_interrupciones(conexion_cpu_interrupt,ENVIAR_DESALOJAR);
	        log_info(logger_consola,"mensaje enviado correctamente\n");
	        break;
=======
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
		default:
			log_info(logger_consola,"no corresponde a ninguno\n");
			break;
	}
}

void generar_conexion() {
	pthread_t conexion_memoria_hilo;
	pthread_t conexion_file_system_hilo;
	pthread_t conexion_cpu_hilo;
<<<<<<< HEAD
	pthread_t conexion_cpu_interrupt_hilo;
=======
>>>>>>> parent of ca1d82c (borre todo para mergear rama)

	log_info(logger_consola,"ingrese q que modulos deseas conectar"
			"\n 1. modulo memoria"
			"\n 2. modulo filesystem"
<<<<<<< HEAD
			"\n 3. modulo cpu"
			"\n 4. modulo cpu interrupt");
=======
			"\n 3. modulo cpu");
>>>>>>> parent of ca1d82c (borre todo para mergear rama)

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
<<<<<<< HEAD
			conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);
			pthread_create(&conexion_cpu_interrupt_hilo, NULL, (void*) procesar_conexion, (void *)&conexion_cpu_interrupt_hilo);
	        log_info(logger_consola,"conexion generado correctamente\n");
=======
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
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
<<<<<<< HEAD
	agregar_a_paquete(paquete, ruta_a_testear, strlen(ruta_a_testear) + 1);
=======
	agregar_a_paquete(paquete, ruta_a_testear, sizeof(ruta_a_testear));
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
	agregar_a_paquete(paquete, &size ,sizeof(int));
	agregar_a_paquete(paquete, &prioridad, sizeof(int));
	agregar_a_paquete(paquete, &pid, sizeof(int));

	enviar_paquete(paquete, conexion_memoria);
	crear_pcb(prioridad);
<<<<<<< HEAD
	sem_post(&contador_agregando_new);

=======
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
	//free(prueba);
	eliminar_paquete(paquete);
	free(ruta_a_testear);

}

<<<<<<< HEAD
void crear_pcb(int prioridad){
	t_pcb* pcb = malloc(sizeof(t_pcb));
=======
void crear_pcb(t_planificador prioridad){
	t_pcb* pcb = malloc(sizeof(pcb));
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
	pcb->pid= contador_pid;
	pcb->prioridad = prioridad;
	t_contexto_ejecucion* contexto = crear_contexto();
	pcb->contexto =contexto;
	//pcb->tabla_archivo_abierto;
<<<<<<< HEAD
	contador_pid++;
	log_pcb_info(pcb);
	agregar_a_cola_new(pcb);

=======
	pcb->estado=NEW;
	contador_pid++;

	agregar_a_cola_new(pcb);
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
}

t_contexto_ejecucion* crear_contexto(){
	t_contexto_ejecucion* contexto = malloc(sizeof(t_contexto_ejecucion));
<<<<<<< HEAD
	contexto->pc =0;
=======
	contexto->pc =NULL;
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
	t_registro_cpu* registro = crear_registro();
	contexto->registros_cpu = registro;
	return contexto;
}

t_registro_cpu* crear_registro(){
<<<<<<< HEAD
    t_registro_cpu* reg = malloc(sizeof(t_registro_cpu));
    reg->ax = 1;
    reg->bx = 1;
    reg->cx = 1;
    reg->dx = 1;
    return reg;
=======
	t_registro_cpu* reg = malloc(sizeof(t_registro_cpu));
    memset(reg->AX, 0, sizeof(reg->AX));
    memset(reg->BX, 0, sizeof(reg->BX));
    memset(reg->CX, 0, sizeof(reg->CX));
    memset(reg->DX, 0, sizeof(reg->DX));
	return reg;
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
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
	pcb->estado=READY;
	sem_post(&mutex_cola_ready);
	log_info(logger,"El proceso [%d] fue agregado a la cola ready",pcb->pid);
}
t_pcb* quitar_de_cola_ready(){
	sem_wait(&mutex_cola_ready);
	t_pcb* pcb=queue_pop(cola_ready);
	sem_post(&mutex_cola_ready);
	log_info(logger,"El proceso [%d] fue quitado de la cola ready",pcb->pid);
	return pcb;
}

void planificador_largo_plazo(){
	while(1){
<<<<<<< HEAD
		sem_wait(&contador_agregando_new);
		sem_wait(&grado_multiprogramacion);
		t_pcb* pcb =quitar_de_cola_new();
		log_info(logger, "el pid del proceso es %i",pcb->pid);
		agregar_a_cola_ready(pcb);
		sem_post(&contador_cola_ready);
=======
	while(!queue_is_empty(cola_new)){
			sem_wait(&grado_multiprogramacion);
			t_pcb* pcb =quitar_de_cola_new();
			log_info(logger, "el pid del proceso es %i",pcb->pid);
			agregar_a_cola_ready(pcb);
		}
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
	}
}

//TODO MOTIVO DE QUE DESPUES DE INICIAR PLANIFICACION NO ME DEJA INGRESAR OTRA OPERACION
void planificador_corto_plazo(){
<<<<<<< HEAD
	while(1){
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
=======
//  Desmarcar para probar los planificadores
	planificador = FIFO;

	t_pcb pcb1 = {
		1,
		2,
		1,
		NULL,
		NULL,
		NEW,
		NULL,
		NULL
	};
	t_pcb pcb2 = {
		2,
		1,
		1,
		NULL,
		NULL,
		NEW,
		NULL,
		NULL
	};
	t_pcb pcb3 = {
		3,
	    5,
	    1,
		NULL,
		NULL,
		NEW,
		NULL,
	    NULL
	};
    t_pcb *ppcb1;
	ppcb1 = &pcb1;
	t_pcb *ppcb2;
	ppcb2 = &pcb2;
	t_pcb *ppcb3;
	ppcb3 = &pcb3;
	agregar_a_cola_ready(ppcb1);
	agregar_a_cola_ready(ppcb2);
	agregar_a_cola_ready(ppcb3);

	log_info(logger,"ando hasta aca");
	while(1){
			while(!queue_is_empty(cola_ready)){
				sem_wait(&contador_ejecutando_cpu);
				switch(planificador){
				case FIFO:
					log_info(logger,"Planificador FIFO");
					de_ready_a_fifo();
					break;
				case ROUND_ROBIN:
					log_info(logger,"Planificador Round Robin");

					de_ready_a_round_robin();
					break;
				case PRIORIDADES:
					log_info(logger,"Planificador Prioridades");
					de_ready_a_prioridades();
					break;
				}
			}
		}
}

//TODO AGREGAR DISPATCH agregar semaforo

void de_ready_a_fifo(){
	t_pcb* pcb =quitar_de_cola_ready();
	enviar_por_dispatch(pcb);
}

void de_ready_a_round_robin(){
	if(hay_proceso_en_ejecucion && tiempo_transcurrido > quantum){
			//DESALOJAR
			de_ready_a_fifo();
		} else {
			de_ready_a_fifo();
		}
}

void de_ready_a_prioridades(){
	list_sort(cola_ready->elements,comparador_prioridades);
	t_pcb* pcb = queue_peek(cola_ready);
	if(hay_proceso_en_ejecucion && pcb->prioridad > prioridad_de_proceso_en_ejecucion){
			//DESALOJAR
			de_ready_a_fifo();
		} else {
			de_ready_a_fifo();
		}
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
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
<<<<<<< HEAD
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


=======
	pcb->estado=RUNNING;
	enviar_pcb(pcb,conexion_cpu,RECIBIR_PCB);
	hay_proceso_en_ejecucion = true;
	prioridad_de_proceso_en_ejecucion = pcb->prioridad;
	log_info(logger,"El proceso [%d] fue pasado al estado de running y enviado al CPU",pcb->pid);
}

t_contexto_ejecucion* obtener_contexto(char* archivo){
	t_contexto_ejecucion *estructura_retornar ;
	return estructura_retornar;
}

// ver como pasar int TODO
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
void finalizar_proceso(int pid){

	t_paquete * paquete = crear_paquete(FINALIZAR);
	agregar_a_paquete(paquete, &pid, sizeof(int));
	enviar_paquete(paquete, conexion_memoria);

	eliminar_paquete(paquete);
	free(paquete);




	//int posicion= buscarPosicionQueEstaElPid(pid);
	//t_pcb* auxiliar =list_remove(cola_new->elements,posicion);
	//liberarMemoriaPcb(auxiliar);
}

void liberarMemoriaPcb(t_pcb* pcbABorrar){
		free(pcbABorrar->contexto);
<<<<<<< HEAD
		//free(pcbABorrar->tabla_archivo_abierto);
=======
		free(pcbABorrar->tabla_archivo_abierto);
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
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
<<<<<<< HEAD

=======
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
	pthread_t * hilo_corto_plazo;
	pthread_t * hilo_largo_plazo;
	log_info(logger_consola,"inicio el proceso de planificacion");
	pthread_create(&hilo_largo_plazo,NULL,(void*) planificador_largo_plazo,NULL);
	pthread_create(&hilo_corto_plazo,NULL,(void*) planificador_corto_plazo,NULL);
	pthread_detach(*hilo_largo_plazo);
	pthread_detach(*hilo_corto_plazo);
<<<<<<< HEAD
=======
	planificador_largo_plazo();
	planificador_corto_plazo();

>>>>>>> parent of ca1d82c (borre todo para mergear rama)
	log_info(logger_consola, "llego hasta aca asddddddddddddddd");

}


void detener_planificacion(){

}
void modificar_grado_multiprogramacion(){
<<<<<<< HEAD
    terminar_programa(conexion_memoria, logger, config);
    terminar_programa(conexion_cpu, logger, config);
    terminar_programa(conexion_file_system, logger, config);
=======
>>>>>>> parent of ca1d82c (borre todo para mergear rama)

}
void listar_proceso_estado(){
	t_paquete * aux = crear_paquete(CPU_ENVIA_A_MEMORIA);
	enviar_paquete(aux, conexion_cpu);
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
<<<<<<< HEAD
    //recursos = config_get_array_value(config, "RECURSOS");
=======
    recursos = config_get_array_value(config, "RECURSOS");
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
    char **instancias = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    instancias_recursos = string_to_int_array(instancias);
    string_array_destroy(instancias);
}

void asignar_algoritmo(char *algoritmo){
	if (strcmp(algoritmo, "FIFO") == 0) {
		planificador = FIFO;
<<<<<<< HEAD
	} else if (strcmp(algoritmo, "ROUND_ROBIN") == 0) {
=======
	} else if (strcmp(algoritmo, "HRRN") == 0) {
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
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
<<<<<<< HEAD
void ejecutar_wait(char*nombre,t_pcb*pcb){
	if (list_is_empty(lista_recursos)) {
		log_info(logger,"LISTA VACIAA");
	    return;
	}
	t_list_iterator* iterador = list_iterator_create(lista_recursos);
	int j=0;
	int encontro =0;
	while(list_iterator_has_next(iterador)){
		t_recurso* recurso = (t_recurso*)list_iterator_next(iterador);
		log_info(logger,"el nombre del recurso es %s" ,nombre);
		log_info(logger,"el nombre del recurso es %s" ,recurso->nombre);
		if(strcmp(nombre,recurso->nombre) == 0){
			encontro = 1;
			log_info(logger,"ENCONTRO EL RECURSO");
			if(recurso->instancias >0){
				recurso->instancias--;
				log_info(logger,"EJECUTANDO WAIT %d" ,recurso->instancias);
				list_replace(lista_recursos,j,recurso);
				agregar_recurso_pcb(pcb->pid,nombre);
			}else{
				pcb->estado = WAITING;
				queue_push(recurso->cola_bloqueados  ,pcb);
				list_add(lista_bloqueados,pcb);
				log_info(logger,"Se agrego a la cola de bloqueados ");

			}
		}
		j++;
	}
	if(encontro ==0){
		enviar_pcb(pcb,conexion_memoria,FINALIZAR);
	}
	list_iterator_destroy(iterador);
}

void ejecutar_signal(char*nombre,t_pcb*pcb){
	if (list_is_empty(lista_recursos)) {
		log_info(logger,"LISTA VACIAA");
	    return;
	}
	t_list_iterator* iterador = list_iterator_create(lista_recursos);
	int j=0;
	int encontro =0;
	while(list_iterator_has_next(iterador)){
		t_recurso* recurso = (t_recurso*)list_iterator_next(iterador);
		log_info(logger,"el nombre del recurso es %s" ,nombre);
		log_info(logger,"el nombre del recurso es %s" ,recurso->nombre);
		if(strcmp(nombre,recurso->nombre) == 0){
			encontro = 1;
			t_recurso_pcb * recurso_pcb = buscar_recurso_pcb(nombre, pcb->pid);
			log_info(logger,"ENCONTRO EL RECURSO");
			if(recurso_pcb != NULL){
				recurso->instancias++;
				log_info(logger,"EJECUTANDO SIGNAL %s" ,recurso->nombre);
				list_replace(lista_recursos,j,recurso);
				quitar_recurso_pcb(pcb->pid,nombre);
				if(!queue_is_empty(recurso->cola_bloqueados)){
					t_pcb* pcb_bloqueado = queue_pop(recurso->cola_bloqueados);
					agregar_a_cola_ready(pcb_bloqueado);
					list_remove_element(lista_bloqueados,pcb_bloqueado);
				}
			}else{
				enviar_pcb(pcb,conexion_memoria,FINALIZAR);
			}
		}
		j++;
	}
	if(encontro ==0){
		enviar_pcb(pcb,conexion_memoria,FINALIZAR);
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
			log_info(logger,"ENCONTRO EL RECURSO AG");
			recurso->instancias++;
			list_replace(lista_recursos_pcb, j, recurso);
		}
		j++;
	}
	if(encontro ==0){
		t_recurso_pcb* recurso = crear_recurso_pcb(nombre, pid);
		list_add(lista_recursos_pcb, recurso);
		log_info(logger,"SE AGREGO RECURSO_PCB A LA LISTA");
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
			log_info(logger,"ENCONTRO EL RECURSO PARA QUITAR");
				recurso->instancias--;
				list_replace(lista_recursos_pcb, j, recurso);
			}else{
				list_remove(lista_recursos_pcb,j);
				free(recurso);
				log_info(logger,"ELIMINE EL RECURSO");
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
						list_remove_element(lista_bloqueados,pcb);
						agregar_a_cola_ready(pcb);
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
bool can_allocate(int pid, int work[]) {
    for (int i = 0; i < list_size(lista_recursos_pcb); i++) {
        t_recurso_pcb* recurso_pcb = list_get(lista_recursos_pcb, i);
        if (recurso_pcb->pid == pid) {
            int index = -1;
            for (int j = 0; j < list_size(lista_recursos); j++) {
                t_recurso* recurso = list_get(lista_recursos, j);
                if (strcmp(recurso->nombre, recurso_pcb->nombre) == 0) {
                    index = j;
                    break;
                }
            }
            if (index == -1 || work[index] < recurso_pcb->instancias) {
                return false;
            }
        }
    }
    return true;
}

void detect_deadlock() {
    int n_resources = list_size(lista_recursos);
    int work[n_resources];

    for (int i = 0; i < n_resources; i++) {
        t_recurso* recurso = list_get(lista_recursos, i);
        work[i] = recurso->instancias;
    }

    bool progress = true;
    while (progress) {
        progress = false;
        for (int i = 0; i < list_size(lista_bloqueados); i++) {
            int pid = list_get(lista_bloqueados, i);
            if (can_allocate(pid, work)) {
                for (int j = 0; j < list_size(lista_recursos_pcb); j++) {
                    t_recurso_pcb* recurso_pcb = list_get(lista_recursos_pcb, j);
                    if (recurso_pcb->pid == pid) {
                        int index = -1;
                        for (int k = 0; k < n_resources; k++) {
                            t_recurso* recurso = list_get(lista_recursos, k);
                            if (strcmp(recurso->nombre, recurso_pcb->nombre) == 0) {
                                index = k;
                                break;
                            }
                        }
                        if (index != -1) {
                            work[index] += recurso_pcb->instancias;
                        }
                    }
                }
                list_remove(lista_bloqueados, i);
                progress = true;
            }
        }
    }

    printf("Procesos en deadlock: ");
    if (list_is_empty(lista_bloqueados)) {
        printf("Ninguno");
    } else {
        for (int i = 0; i < list_size(lista_bloqueados); i++) {
            printf("%d ", list_get(lista_bloqueados, i));
        }
    }
    printf("\n");
=======

void ejecutar_wait(char* recurso_a_encontrar, t_pcb * pcb){

    bool encontrar_recurso(void * recurso){
          t_recurso* un_recurso = (t_recurso*)recurso;
          return strcmp(un_recurso->nombre, recurso_a_encontrar) == 0;
    }

    t_recurso *recurso_encontrado = list_find(lista_recursos, encontrar_recurso);
        if(recurso_encontrado != NULL){
            if(recurso_encontrado->instancias >0 ){
            	pcb = agregar_recurso_pcb(pcb, recurso_a_encontrar);
            	int posicion = buscar_posicion_lista_recurso(lista_recursos, recurso_encontrado);
                recurso_encontrado->instancias -=1;
                list_replace(lista_recursos,posicion,recurso_encontrado);
            }else{
                queue_push(recurso_encontrado->cola_bloqueados,pcb);
            }
        }else{
            enviar_pcb(pcb,conexion_memoria,FINALIZAR);
        }
}

t_pcb*agregar_recurso_pcb(t_pcb*pcb, char*nombre){
	bool encontrar_recurso(void * recurso){
	          t_recurso_pcb* un_recurso = (t_recurso_pcb*)recurso;
	          return strcmp(un_recurso->nombre, nombre) == 0;
	    }
	    t_recurso_pcb *recurso_encontrado = list_find(pcb->recursos, encontrar_recurso);
	    if(recurso_encontrado != NULL){
	    	int posicion = buscar_posicion_lista_recurso_pcb(pcb->recursos,recurso_encontrado);
	    	recurso_encontrado->instancias++;
	    	list_replace(pcb->recursos,posicion,recurso_encontrado);
	    }else{
	    	t_recurso_pcb*recurso_nuevo;
	    	recurso_nuevo=crear_recurso(nombre);
	    	list_add(pcb->recursos, recurso_nuevo);
	    }
	    return pcb;
}
t_pcb*quitar_recurso_pcb(t_pcb*pcb, char*nombre){
	bool encontrar_recurso(void * recurso){
	          t_recurso_pcb* un_recurso = (t_recurso_pcb*)recurso;
	          return strcmp(un_recurso->nombre, nombre) == 0;
	    }
	    t_recurso_pcb *recurso_encontrado = list_find(pcb->recursos, encontrar_recurso);
	    	if(recurso_encontrado != NULL){
				int posicion = buscar_posicion_lista_recurso_pcb(pcb->recursos,recurso_encontrado);
				recurso_encontrado->instancias--;
				list_replace(pcb->recursos,posicion,recurso_encontrado);
	    	}
	    return pcb;
}

t_recurso_pcb*crear_recurso(char*nombre){
	t_recurso_pcb*recurso_nuevo=malloc(sizeof(t_recurso_pcb));
	recurso_nuevo->nombre=nombre;
	recurso_nuevo->instancias=1;
	return recurso_nuevo;
}
void ejecutar_signal(char* recurso_a_encontrar, t_pcb * pcb){
    bool encontrar_recurso(void * recurso){
              t_recurso* un_recurso = (t_recurso*)recurso;
              return strcmp(un_recurso->nombre, recurso_a_encontrar) == 0;
    }
    bool encontrar_recurso_pcb(void * recurso){
                  t_recurso_pcb* un_recurso = (t_recurso_pcb*)recurso;
                  return strcmp(un_recurso->nombre, recurso_a_encontrar) == 0;
        }
    t_recurso *recurso_encontrado = list_find(lista_recursos, encontrar_recurso);
    t_recurso_pcb *recurso_pcb = list_find(pcb->recursos, encontrar_recurso_pcb);
    if(recurso_encontrado != NULL){
    	if(recurso_pcb->instancias > 0){
    	 int posicion = buscar_posicion_lista_recurso(lista_recursos, recurso_encontrado);
    	 recurso_encontrado->instancias ++;
    	 list_replace(lista_recursos,posicion,recurso_encontrado);
    	 pcb = quitar_recurso_pcb(pcb,recurso_a_encontrar);
    	}else{
    	enviar_pcb(pcb,conexion_memoria,FINALIZAR);
    	}
    }else{
        enviar_pcb(pcb,conexion_memoria,FINALIZAR);
    }
}


int buscar_posicion_lista_recurso_pcb(t_list*lista, t_recurso_pcb *recurso){
	int cantidad= list_size(lista);
	t_recurso_pcb* elemento ;
	for(int i=0;i<cantidad;i++){
		elemento = list_get(lista,cantidad);
		if(elemento->nombre == recurso->nombre){
			return cantidad;
		}
	}
	return -1;
}
int buscar_posicion_lista_recurso(t_list*lista, t_recurso *recurso){
	int cantidad= list_size(lista);
	t_recurso* elemento ;
	for(int i=0;i<cantidad;i++){
		elemento = list_get(lista,cantidad);
		if(elemento->nombre == recurso->nombre){
			return cantidad;
		}
	}
	return -1;
>>>>>>> parent of ca1d82c (borre todo para mergear rama)
}

