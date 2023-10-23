#include "kernel.h"
#include<readline/readline.h>

int main(int argc, char **argv){


	char *rutaConfig = "kernel.config";
	config = cargar_config(rutaConfig);

    logger = log_create("./kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy el Kernel!");

    obtener_configuracion();
    iniciar_recurso();
    iniciar_consola();

    //envio de mensajes

    //error
    //paquete(conexion_memoria);

    return EXIT_SUCCESS;
}


void procesar_conexion(void *conexion1){
	int *conexion = (int*)conexion1;
	int cliente_fd = *conexion;

	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		t_pcb* pcb_aux;
		t_list* paquete;
		switch (cod_op) {
		case MENSAJE:
			log_info(logger,"hola");
			recibir_mensaje(cliente_fd);
			break;
		case EJECUTAR_SLEEP:
			paquete = recibir_paquete(cliente_fd);
			pcb_aux = desempaquetar_pcb(paquete);
			int tiempo = atoi(obtener_mensaje(cliente_fd));
			sleep(tiempo);
			break;
		case EJECUTAR_WAIT:
			paquete = recibir_paquete(cliente_fd);
			pcb_aux = desempaquetar_pcb(paquete);
			char * nombre_recurso = "RA";//obtener_mensaje(cliente_fd);
			t_recurso *recurso = list_get(lista_recursos,0);
			int instacias = recurso->instancias;
			log_info(logger,"ejecutando wait %i",instacias);
			ejecutar_wait(nombre_recurso,pcb_aux);
			t_recurso *recurso2 = list_get(lista_recursos,0);
			instacias= recurso2->instancias;
			log_info(logger,"ejecutando wait %i",instacias);
			break;
		case EJECUTAR_SIGNAL:
			paquete = recibir_paquete(cliente_fd);
			pcb_aux = desempaquetar_pcb(paquete);
			char * nombre_recurso2 = obtener_mensaje(cliente_fd);
			//ejecutar_signal(nombre_recurso2,pcb_aux);
			log_info(logger,"ejecutando wait %i",nombre_recurso2);
			break;
		case EJECUTAR_F_TRUNCATE:
			log_info(logger,"me llegaron la instruccion ejecutar f truncate del cpu");
			break;
		case FINALIZAR:
			t_list * paquete = recibir_paquete(cliente_fd);
			pcb_aux = desempaquetar_pcb(paquete);
			log_info(logger,"el pid del proceso finalizado es %i",pcb_aux->pid);
			//TODO VER SI NECESITA UNA LISTA PARA LAMACENAR LOS PROCESOS TERMINADO

			enviar_mensaje("hola se finalizo el proceso ", conexion);
			pcb_aux->estado = TERMINATED;
			log_pcb_info(pcb_aux);
			enviar_pcb(pcb_aux,conexion_memoria,FINALIZAR);
			sem_post(&contador_ejecutando_cpu);
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
	lista_recursos = list_create();
	t_recurso* nuevo_recurso = (t_recurso*) malloc(sizeof(t_recurso));
	nuevo_recurso->instancias =3;
	nuevo_recurso->nombre = "RA";
	log_info(logger,"llegue");
	list_add(lista_recursos,nuevo_recurso);
	free(nuevo_recurso);
	sem_init(&grado_multiprogramacion, 0, grado_multiprogramacion_ini);
	sem_init(&mutex_cola_new, 0, 1);
	sem_init(&contador_ejecutando_cpu,0,1);
	sem_init(&mutex_cola_ready,0,1);
	sem_init(&contador_agregando_new,0,0);
	sem_init(&contador_cola_ready,0,0);
}

void enviar_mensaje_kernel() {
	log_info(logger_consola,"ingrese q que modulos deseas mandar mensaje"
			"\n 1. modulo memoria"
			"\n 2. modulo cpu"
			"\n 3. modulo filesystem");
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
			enviar_mensaje("kernel a interrupt ", conexion_cpu_interrupt);
			 log_info(logger_consola,"mensaje enviado correctamente\n");
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
		case '4':
			conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt);
			//pthread_create(&conexion_cpu_interrupt_hilo, NULL, (void*) procesar_conexion, (void *)&conexion_cpu_interrupt_hilo);
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
	pcb->estado=NEW;
	contador_pid++;
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
    reg->ax = 1;
    reg->bx = 1;
    reg->cx = 1;
    reg->dx = 1;
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
	log_info(logger,"ando hasta aca");
	while(1){
		sem_wait(&contador_cola_ready);

		switch(planificador){
		case FIFO:
			sem_wait(&contador_ejecutando_cpu);
			log_info(logger,"Planificador FIFO");
			de_ready_a_fifo();
			break;
		case ROUND_ROBIN:
			sem_wait(&contador_ejecutando_cpu);
			log_info(logger,"Planificador Round Robin");
			//de_ready_a_round_robin();
			break;
		case PRIORIDADES:
			log_info(logger,"Planificador Prioridades");
			de_ready_a_prioridades();
			break;
		}
		}
}

//TODO AGREGAR DISPATCH agregar semaforo

void de_ready_a_fifo(){
	t_pcb* pcb =quitar_de_cola_ready();
    enviar_por_dispatch(pcb);
}

//TODO Revisar el if que crashea

void de_ready_a_prioridades(){
	list_sort(cola_ready->elements,comparador_prioridades);
	t_pcb* pcb = quitar_de_cola_ready();
	pcb->estado=RUNNING;
	enviar_pcb(pcb,conexion_cpu,RECIBIR_PCB);
}
/*
void de_ready_a_prioridades(){
    list_sort(cola_ready->elements,comparador_prioridades);
    t_pcb* pcb_a_comparar_prioridad = queue_peek(cola_ready);
    if( pcb_en_ejecucion->prioridad < pcb_a_comparar_prioridad){
        //DESALOJAR
        de_ready_a_fifo();
    } else {
        de_ready_a_fifo();
    }
}
*/
/*
void de_ready_a_round_robin(){
    if(hay_proceso_en_ejecucion && tiempo_transcurrido > quantum){
        //DESALOJAR
        de_ready_a_fifo();
    } else {
        de_ready_a_fifo();
    }
}*/

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
    enviar_pcb(pcb,conexion_cpu,RECIBIR_PCB);
    pcb_en_ejecucion = pcb;
    log_info(logger,"El proceso [%d] fue pasado al estado de running y enviado al CPU",pcb->pid);
}



t_contexto_ejecucion* obtener_contexto(char* archivo){
	t_contexto_ejecucion *estructura_retornar ;

	return estructura_retornar;
}

// ver como pasar int TODO
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

	pthread_t * hilo_corto_plazo;
	pthread_t * hilo_largo_plazo;
	log_info(logger_consola,"inicio el proceso de planificacion");
	pthread_create(&hilo_largo_plazo,NULL,(void*) planificador_largo_plazo,NULL);
	pthread_create(&hilo_corto_plazo,NULL,(void*) planificador_corto_plazo,NULL);
	pthread_detach(*hilo_largo_plazo);
	pthread_detach(*hilo_corto_plazo);
	log_info(logger_consola, "llego hasta aca asddddddddddddddd");

}


void detener_planificacion(){

}
void modificar_grado_multiprogramacion(){
    terminar_programa(conexion_memoria, logger, config);
    terminar_programa(conexion_cpu, logger, config);
    terminar_programa(conexion_file_system, logger, config);

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
    recursos = config_get_array_value(config, "RECURSOS");
    char **instancias = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    instancias_recursos = string_to_int_array(instancias);
    string_array_destroy(instancias);
}

void asignar_algoritmo(char *algoritmo){
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

void ejecutar_wait(char* recurso_a_encontrar, t_pcb * pcb){

    bool encontrar_recurso(void * recurso){
          t_recurso* un_recurso = (t_recurso*)recurso;
          return strcmp(un_recurso->nombre, recurso_a_encontrar) == 0;
    }

    t_recurso *recurso_encontrado = list_find(lista_recursos, encontrar_recurso);
        if(recurso_encontrado != NULL){
            if(recurso_encontrado->instancias >0 ){
            	int posicion = buscar_posicion_lista_recurso(lista_recursos, recurso_encontrado);
                recurso_encontrado->instancias -=1;
                list_replace(lista_recursos,posicion,recurso_encontrado);
                pcb = agregar_recurso_pcb(pcb, recurso_a_encontrar);
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

//TODO DESCOMENTAR


int buscar_posicion_lista_recurso_pcb(t_list *lista, t_recurso_pcb *recurso) {
    int cantidad = list_size(lista);
    t_recurso_pcb *elemento;

    for (int i = 0; i < cantidad; i++) {
        elemento = list_get(lista, i);
        if (strcmp(elemento->nombre, recurso->nombre) == 0) {
            return i;
        }
    }
    return -1;
}
int buscar_posicion_lista_recurso(t_list *lista, t_recurso *recurso) {
    int cantidad = list_size(lista);
    t_recurso *elemento;

    for (int i = 0; i < cantidad; i++) {
        elemento = list_get(lista, i);
        if (strcmp(elemento->nombre, recurso->nombre) == 0) {
            return i;
        }
    }
    return -1;
}

