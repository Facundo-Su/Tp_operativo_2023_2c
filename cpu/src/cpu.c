
#include "cpu.h"

int main(int argc, char* argv[]) {

	char *rutaConfig = "./cpu.config";

	config = cargar_config(rutaConfig);

    logger = log_create("./cpu.log", "CPU", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy la cpu!");

    //iniciar configuraciones
	 obtener_configuracion();
	 iniciar_recurso();
	//iniciar_consola();
	log_info(logger_consola_cpu, "se inicio el servidor\n");
	pthread_t servidor_interrupt;
	pthread_t servidor_dispatch;
	pthread_create(&servidor_interrupt,NULL,(void*)iniciar_servidor_interrupt,(void *) puerto_escucha_interrupt);
	pthread_create(&servidor_dispatch,NULL,(void*)iniciar_servidor_cpu,(void*) puerto_escucha);
	//iniciar_servidor_cpu(puerto_escucha);

	pthread_join(servidor_dispatch,NULL);
	pthread_join(servidor_interrupt, NULL);
	terminar_programa(conexion_memoria, logger, config);
    return 0;
}

void iniciar_recurso(){
	hay_desalojo=false;
	recibi_archivo=false;
	hayInterrupcion = false;
	sem_init(&contador_instruccion, 0,0);
	instruccion_a_realizar= malloc(sizeof(t_instruccion));
	logger_consola_cpu = log_create("./cpuConsola.log", "consola", 1, LOG_LEVEL_INFO);
	//sem_init(&instruccion_ejecutando, 0,1);
}

void iniciar_servidor_interrupt(char * puerto){
	int cpu_interrupt_fd = iniciar_servidor(puerto);
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(cpu_interrupt_fd);
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
			case ENVIAR_DESALOJAR:
				hay_desalojo = true;
				hayInterrupcion= true;
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

void procesar_conexion(void *conexion1){
	int *conexion = (int*)conexion1;
	int cliente_fd = *conexion;
	t_list* lista = list_create();

	while (1) {
		int cod_op = recibir_operacion(cliente_fd);

		t_pcb* pcb_aux;
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			enviar_mensaje("hola che",cliente_fd);
			break;
		case INSTRUCCIONES_A_MEMORIA:
			char* auxiliar =recibir_instruccion(cliente_fd);
			log_info(logger_consola_cpu,"me llego la siguiente instruccion %s",auxiliar);
			transformar_en_instrucciones(auxiliar);
			hayInterrupcion = true;
			recibi_archivo=true;
			sem_post(&contador_instruccion);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case ENVIARREGISTROCPU:
			t_list* valores_cpu = list_create();
			valores_cpu= recibir_paquete(cliente_fd);
			log_info(logger, "ME LLEGARON");
			break;
			//TODO
			//preguntar porque si lo meto dentro de una funcion no me reconoce
		case RECIBIR_PCB:
			log_info(logger, "Estoy por recibir un PCB");
			t_list * paquete = recibir_paquete(cliente_fd);
			pcb = desempaquetar_pcb(paquete);
			//recibir_pcb(cliente_fd);
			hayInterrupcion = false;
			hay_desalojo= false;
			log_pcb_info(pcb);
			ejecutar_ciclo_de_instruccion(cliente_fd);

			break;
		case CPU_ENVIA_A_MEMORIA:
			enviar_mensaje("hola capo", conexion_memoria);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			close(cliente_fd);
			return;
		default:
			log_info(logger,"hola pepe");
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return;
}

void transformar_en_instrucciones(char* auxiliar){
	instruccion_a_realizar->parametros= list_create();
	int cantidad_parametros;
	char** instruccion_parseada = parsear_instruccion(auxiliar);

	        if (strcmp(instruccion_parseada[0], "SET") == 0) {
	        	instruccion_a_realizar->nombre = SET;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "SUB") == 0) {
	        	instruccion_a_realizar->nombre = SUB;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "SUM") == 0) {
	        	instruccion_a_realizar->nombre = SUM;
	            cantidad_parametros = 2;
	        }

	        if (strcmp(instruccion_parseada[0], "JNZ") == 0) {
	        	instruccion_a_realizar->nombre = JNZ;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "SLEEP") == 0) {
	        	instruccion_a_realizar->nombre = SLEEP;
	            cantidad_parametros = 1;
	        }
	        if (strcmp(instruccion_parseada[0], "WAIT") == 0) {
	        	instruccion_a_realizar->nombre = WAIT;
	            cantidad_parametros = 1;
	        }
	        if (strcmp(instruccion_parseada[0], "SIGNAL") == 0) {
	        	instruccion_a_realizar->nombre = SIGNAL;
	            cantidad_parametros = 1;
	        }
	        if (strcmp(instruccion_parseada[0], "MOV_IN") == 0) {
	        	instruccion_a_realizar->nombre = MOV_IN;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "MOV_OUT") == 0) {
	        	instruccion_a_realizar->nombre = MOV_OUT;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "F_OPEN") == 0) {
	        	instruccion_a_realizar->nombre = F_OPEN;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "F_CLOSE") == 0) {
	        	instruccion_a_realizar->nombre = F_CLOSE;
	            cantidad_parametros = 1;
	        }
	        if (strcmp(instruccion_parseada[0], "F_SEEK") == 0) {
	        	instruccion_a_realizar->nombre = F_SEEK;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "F_READ") == 0) {
	        	instruccion_a_realizar->nombre = F_READ;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "F_WRITE") == 0) {
	        	instruccion_a_realizar->nombre = F_WRITE;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "F_TRUNCATE") == 0) {
	        	instruccion_a_realizar->nombre = F_TRUNCATE;
	            cantidad_parametros = 2;
	        }
	        if (strcmp(instruccion_parseada[0], "EXIT") == 0) {
	        	instruccion_a_realizar->nombre = EXIT;
	            cantidad_parametros = 0;
	        }

	    	t_list* parametros = list_create();

	        for(int i=1;i<cantidad_parametros+1;i++){
	            list_add(parametros,instruccion_parseada[i]);
	        }

	        list_add_all(instruccion_a_realizar->parametros,parametros);
}


char** parsear_instruccion(char* instruccion){

    // Parseo la instruccion
    char** instruccion_parseada = string_split(instruccion, " ");

    // Retorno la instruccion parseada
    return instruccion_parseada;
}


//TODO
/*
void iniciar_consola(){

	char* valor;

	while(1){
		log_info(logger_consola_cpu,"ingrese la operacion que deseas realizar"
				"\n 1. generar conexion"
				"\n 2. enviar mensaje"
				"\n 3. iniciar Como Servidor"
				"\n 4. leer archivo");
		valor = readline(">");
		switch (*valor) {
			case '1':

				generar_conexion_memoria();
				break;
			case '2':
				log_info(logger_consola_cpu, "enviar mensaje a memoria\n");
				enviar_mensaje("cpu a memoria", conexion_memoria);
				break;
			case '3':
				log_info(logger_consola_cpu, "se inicio el servidor\n");
				iniciar_servidor_cpu(puerto_escucha);
				break;
			default:
				log_info(logger_consola_cpu,"no corresponde a ninguno");
				exit(2);
		}
		free(valor);
	}

}
*/
void obtener_configuracion(){
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
	puerto_escucha_interrupt = config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");
}
void iniciar_servidor_cpu(char *puerto){

	int cpu_fd = iniciar_servidor(puerto);
	log_info(logger, "Servidor listo para recibir al cliente");
	generar_conexion_memoria();

	log_info(logger, "genere conexion con memoria");

	while(1){
	    int cliente_fd = esperar_cliente(cpu_fd);
		pthread_t atendiendo_cpu;
		pthread_create(&atendiendo_cpu,NULL,(void*)procesar_conexion,(void *) &cliente_fd);
		pthread_detach(atendiendo_cpu);
	}
}

void generar_conexion_memoria(){
	log_info(logger_consola_cpu, "generar conexion con memoria\n");
	pthread_t conexion_memoria_hilo_cpu;
	conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	pthread_create(&conexion_memoria_hilo_cpu,NULL,(void*) procesar_conexion,(void *)&conexion_memoria);
}
/*
void atendiendo_pedido(int cliente_fd){
	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			enviar_mensaje("te respondi el mensaje ", cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case ENVIARREGISTROCPU:
			t_list* valores_cpu = list_create();
			valores_cpu= recibir_paquete(cliente_fd);
			log_info(logger, "ME LLEGARON");
			break;

		case RECIBIR_PCB:
			t_pcb* pcb = recibir_pcb(cliente_fd);
			log_info(logger, "recibi el pid %i",pcb->pid);
			ejecutar_ciclo_de_instruccion(pcb);
			break;
		case CPU_ENVIA_A_MEMORIA:
			enviar_mensaje("hola capo", conexion_memoria);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
            close(cliente_fd);
            return;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return;
}*/

void ejecutar_ciclo_de_instruccion(int cliente_fd){
	instruccion_ejecutando= true;
//pide a memoria
	if(hay_desalojo){
		enviar_pcb(pcb,cliente_fd,ENVIAR_DESALOJAR);
		log_info(logger, "LLEGO A DESALOJAR");
		return;
	}
	while(!hayInterrupcion){
		fetch(cliente_fd);
	}


}

void fetch(int cliente_fd){

	int pc = pcb->contexto->pc;
	int pid = pcb->pid;
	log_info(logger, "estoy en fetch con pid %i ",pid);

	solicitar_instruccion_ejecutar_segun_pc(pc, pid);
	sem_wait(&contador_instruccion);
	pcb->contexto->pc+=1;
	decode(instruccion_a_realizar,cliente_fd);

}

void solicitar_instruccion_ejecutar_segun_pc(int pc,int pid){
	t_paquete* paquete = crear_paquete(INSTRUCCIONES_A_MEMORIA);
	agregar_a_paquete(paquete, &pc, sizeof(int));
	agregar_a_paquete(paquete, &pid, sizeof(int));
	enviar_paquete(paquete, conexion_memoria);

}


void decode(t_instruccion* instrucciones,int cliente_fd){
	t_estrucutra_cpu registro_aux;
	t_estrucutra_cpu registro_aux2;
	char * recurso ="";
	char* parametro="";
	char* parametro2="";
	uint32_t valor_uint1;
	uint32_t valor_uint2;
	switch(instrucciones->nombre){
	case SET:
		hayInterrupcion= false;

		parametro2= list_get(instrucciones->parametros,1);
		parametro= list_get(instrucciones->parametros,0);

		valor_uint1 = strtoul(parametro2, NULL, 10);
		registro_aux = devolver_registro(parametro);
		setear(registro_aux,valor_uint1);
		//ADormir(x segundo);
		break;
	case SUB:
		hayInterrupcion= false;
		parametro= list_get(instrucciones->parametros,0);
		parametro2= list_get(instrucciones->parametros,1);
		registro_aux = devolver_registro(parametro);
		registro_aux2 = devolver_registro(parametro2);
		restar(pcb, registro_aux, registro_aux2);
		break;
	case SUM:
		hayInterrupcion= false;
		parametro= list_get(instrucciones->parametros,0);
		parametro2= list_get(instrucciones->parametros,1);

		registro_aux = devolver_registro(parametro);
		registro_aux2 = devolver_registro(parametro2);
		sumar(registro_aux, registro_aux2);
		break;
	case JNZ:
		parametro = list_get(instrucciones->parametros,0);
		parametro2 =list_get(instrucciones->parametros,1);
		registro_aux = devolver_registro(parametro);
		char* valorObtenido = obtener_valor(registro_aux);
		if(strcmp(valorObtenido,"0") ==0){
			int valorEntero = atoi(parametro2);
			pcb->contexto->pc =valorEntero;
		}
		break;
	case SLEEP:
		char * tiempo = list_get(instrucciones->parametros,0);
		enviar_pcb(pcb,cliente_fd,EJECUTAR_SLEEP);
		enviar_mensaje(tiempo,cliente_fd);
		break;
   case WAIT:
	    hayInterrupcion= false;
		recurso= list_get(instrucciones->parametros,0);
		enviar_pcb(pcb,cliente_fd,EJECUTAR_WAIT);
		break;
	case SIGNAL:
		recurso = list_get(instrucciones->parametros,0);
		enviar_pcb(pcb,cliente_fd,EJECUTAR_SIGNAL);
		enviar_mensaje(recurso,cliente_fd);
		break;
	case MOV_IN:
		log_info(logger_consola_cpu,"entendi el mensaje MOV_IN");
		break;
	case MOV_OUT:
		log_info(logger_consola_cpu,"entendi el mensaje MOV_OUT");
		break;
	case F_OPEN:
		hayInterrupcion=false;
		log_info(logger_consola_cpu,"entendi el mensaje F_OPEN");
		break;
	case F_CLOSE:
		log_info(logger_consola_cpu,"entendi el mensaje F_CLOSE");
		break;
	case F_SEEK:
		log_info(logger_consola_cpu,"entendi el mensaje F_SEEK");
		break;
	case F_READ:
		hayInterrupcion=false;
		log_info(logger_consola_cpu,"entendi el mensaje F_READ");
		break;
	case F_WRITE:
		log_info(logger_consola_cpu,"entendi el mensaje F_WRITE");
		break;
	case F_TRUNCATE:
		//hayInterrupcion = true;
		log_info(logger_consola_cpu,"entendi el mensaje F_TRUNCATE");
		break;
	case EXIT:
		//TODO semaforo
		log_info(logger_consola_cpu, "llego hasta aca sssssss");
		hayInterrupcion = true;
		imprimir_valores_registros(pcb->contexto->registros_cpu);
		enviar_pcb(pcb,cliente_fd,FINALIZAR);
		log_info(logger_consola_cpu,"entendi el mensaje EXIT");
		break;
	}
	recibi_archivo = false;
	instruccion_ejecutando= false;
}

void setear(t_estrucutra_cpu pos, uint32_t valor) {
    switch(pos) {
        case AX: pcb->contexto->registros_cpu->ax = valor; break;
        case BX: pcb->contexto->registros_cpu->bx = valor; break;
        case CX: pcb->contexto->registros_cpu->cx = valor; break;
        case DX: pcb->contexto->registros_cpu->dx = valor; break;
        default: log_info(logger, "Registro de destino no válido");
    }
}


//transformar en enum
t_estrucutra_cpu devolver_registro(char* registro){
	t_estrucutra_cpu v;
    if(strcmp(registro,"AX")==0 || strcmp(registro,"AX\n")==0){
        v = AX;
    } else if(strcmp(registro,"BX")==0|| strcmp(registro,"BX\n")==0){
        v = BX;
    } else if(strcmp(registro,"CX")==0|| strcmp(registro,"CX\n")==0){
        v = CX;
    } else if(strcmp(registro,"DX")==0|| strcmp(registro,"DX\n")==0){
        v = DX;
    } else {
        log_error(logger,"CUIDADO,CODIGO INVALIDO");
    }
    return v;
}

void sumar(t_estrucutra_cpu destino, t_estrucutra_cpu origen) {

    uint32_t valor_destino = obtener_valor(destino);


    uint32_t valor_origen = obtener_valor(origen);
    log_info(logger_consola_cpu,"el valor a sumar es %u",valor_destino);
    log_info(logger_consola_cpu,"el valor a sumar es %u",valor_origen);
    uint32_t resultado = valor_destino + valor_origen;
    setear(destino, resultado);
}




void restar(t_estrucutra_cpu destino, t_estrucutra_cpu origen) {
    uint32_t valor_destino = obtener_valor(destino);
    uint32_t valor_origen = obtener_valor(origen);
    uint32_t resultado = valor_destino - valor_origen;  // Ensure underflow handling if needed
    setear(destino, resultado);
}


uint32_t obtener_valor(t_estrucutra_cpu pos) {
    switch(pos) {
        case AX: return  pcb->contexto->registros_cpu->ax;
        case BX: return  pcb->contexto->registros_cpu->bx;
        case CX: return  pcb->contexto->registros_cpu->cx;
        case DX: return  pcb->contexto->registros_cpu->dx;
        default: log_info(logger, "Registro no reconocido"); return 0;
    }
}


void iterator(char* value) {
	log_info(logger,"%s", value);
}

void imprimir_valores_registros(t_registro_cpu* registros) {
    log_info(logger, "Valor de AX: %u", registros->ax);
    log_info(logger, "Valor de BX: %u", registros->bx);
    log_info(logger, "Valor de CX: %u", registros->cx);
    log_info(logger, "Valor de DX: %u", registros->dx);
}

