
#include "cpu.h"

int main(int argc, char* argv[]) {

	char *rutaConfig = "./cpu.config";

	config = cargar_config(rutaConfig);

    logger = log_create("./cpu.log", "CPU", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy la cpu!");

    //iniciar configuraciones
	 obtener_configuracion();

	iniciar_consola();


	terminar_programa(conexion_memoria, logger, config);
    return 0;
}

void iniciar_consola(){
	logger_consola_cpu = log_create("./cpuConsola.log", "consola", 1, LOG_LEVEL_INFO);
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
				log_info(logger_consola_cpu, "generar conexion con memoria\n");
				conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
				break;
			case '2':
				log_info(logger_consola_cpu, "enviar mensaje a memoria\n");
				enviar_mensaje("cpu a memoria", conexion_memoria);
				break;
			case '3':
				log_info(logger_consola_cpu, "se inicio el servidor\n");
				iniciar_servidor_cpu(puerto_escucha);
				break;
			case '4':
				FILE* archivos = fopen("./test.txt","r");
				log_info(logger_consola_cpu, "estos son los valores\n");
				leer_pseudocodigo(archivos);
				printf("estos son los valores %i",list_size(instrucciones));
				break;
			default:
				log_info(logger_consola_cpu,"no corresponde a ninguno");
				exit(2);
		}
		free(valor);
	}

}

void obtener_configuracion(){
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
}
int iniciar_servidor_cpu(char *puerto){
	int servidor_fd = iniciar_servidor(puerto);
	log_info(logger, "Servidor listo para recibir al cliente");
	cliente_fd = esperar_cliente(servidor_fd);

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			enviar_mensaje("hola", cliente_fd);
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
			hayInterrupcion = false;
			//ejecutar_ciclo_de_instruccion(pcb);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;

}

void ejecutar_ciclo_de_instruccion(t_pcb* pcb){
//pide a memoria
	while(!hayInterrupcion){
		//fetch(pcb);
	}

}

void fetch(t_pcb* pcb){
	t_instruccion* instruccion_a_realizar = list_get(pcb->lista_instruciones,pcb->contexto->pc);
	pcb->contexto->pc+=1;
	decode(pcb,instruccion_a_realizar);
}

void decode(t_pcb* pcb,t_instruccion* instrucciones){
	t_estrucutra_cpu registro_aux;
	t_estrucutra_cpu registro_aux2;
	char * recurso;
	char* parametro;
	char* parametro2;
	switch(instrucciones->nombre){
	case SET:
		parametro2= list_get(instrucciones->parametros,1);
		parametro= list_get(instrucciones->parametros,0);
		registro_aux = devolver_registro(parametro);
		setear(pcb,registro_aux,parametro2);
		log_info(logger_consola_cpu,"se termino de ejecutar la operacion del pid %i :",pcb->pid);
		//ADormir(x segundo);
		break;
	case SUB:
		parametro= list_get(instrucciones->parametros,0);
		parametro2= list_get(instrucciones->parametros,1);
		registro_aux = devolver_registro(parametro);
		registro_aux2 = devolver_registro(parametro2);
		restar(pcb, registro_aux, registro_aux2);
		log_info(logger_consola_cpu,"se termino de ejecutar la operacion del pid %i :",pcb->pid);
		break;
	case SUM:
		parametro= list_get(instrucciones->parametros,0);
		parametro2= list_get(instrucciones->parametros,1);
		registro_aux = devolver_registro(parametro);
		registro_aux2 = devolver_registro(parametro2);
		sumar(pcb, registro_aux, registro_aux2);
		log_info(logger_consola_cpu,"se termino de ejecutar la operacion del pid %i :",pcb->pid);
		break;
	case JNZ:
		parametro = list_get(instrucciones->parametros,0);
		parametro2 =list_get(instrucciones->parametros,1);
		registro_aux = devolver_registro(parametro);
		char* valorObtenido = obtener_valor(pcb, registro_aux);
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
		recurso= list_get(instrucciones->parametros,0);
		enviar_pcb(pcb,cliente_fd,EJECUTAR_WAIT);
		enviar_mensaje(recurso,cliente_fd);
		break;
	case SIGNAL:
		recurso = list_get(instrucciones->parametros,0);
		enviar_pcb(pcb,cliente_fd,EJECUTAR_SIGNAL);
		enviar_mensaje(recurso,cliente_fd);
		break;
	case MOV_IN:
		log_info(logger_consola,"entendi el mensaje MOV_IN");
		break;
	case MOV_OUT:
		log_info(logger_consola,"entendi el mensaje MOV_OUT");
		break;
	case F_OPEN:
		log_info(logger_consola,"entendi el mensaje F_OPEN");
		break;
	case F_CLOSE:
		log_info(logger_consola,"entendi el mensaje F_CLOSE");
		break;
	case F_SEEK:
		log_info(logger_consola,"entendi el mensaje F_SEEK");
		break;
	case F_READ:
		log_info(logger_consola,"entendi el mensaje F_READ");
		break;
	case F_WRITE:
		log_info(logger_consola,"entendi el mensaje F_WRITE");
		break;
	case F_TRUNCATE:
		hayInterrupcion = true;
		log_info(logger_consola,"entendi el mensaje F_TRUNCATE");
		break;
	case EXIT:
		hayInterrupcion = true;
		enviar_pcb(pcb,cliente_fd,FINALIZAR);
		log_info(logger_consola,"entendi el mensaje EXIT");
		break;
	}
}

void setear(t_pcb* pcb, t_estrucutra_cpu pos, char* valor) {
    switch(pos) {
        case AX: memcpy(&(pcb->contexto->registros_cpu->AX), valor, strlen(valor)+1);
                 break;
        case BX: memcpy(&(pcb->contexto->registros_cpu->BX), valor, strlen(valor)+1);
                 break;
        case CX: memcpy(&(pcb->contexto->registros_cpu->CX), valor, strlen(valor)+1);
                 break;
        case DX: memcpy(&(pcb->contexto->registros_cpu->DX), valor, strlen(valor)+1);
                 break;
        default: log_info(logger, "Registro de destino no válido");
    }
}


//transformar en enum
t_estrucutra_cpu devolver_registro(char* registro){
	t_estrucutra_cpu v;
    if(strcmp(registro,"AX")==0){
        v = AX;
    } else if(strcmp(registro,"BX")==0){
        v = BX;
    } else if(strcmp(registro,"CX")==0){
        v = CX;
    } else if(strcmp(registro,"DX")==0){
        v = DX;
    } else {
        log_error(logger,"CUIDADO,CODIGO INVALIDO");
    }
    return v;
}

void sumar(t_pcb* pcb, t_estrucutra_cpu destino, t_estrucutra_cpu inicio) {
	char valor_destino;
	char valor_origen;

	valor_destino =obtener_valor(pcb, destino);
	valor_origen = obtener_valor(pcb, inicio);

    char resultado = valor_destino+ valor_origen;
    setear(pcb, destino, resultado);

}




void restar(t_pcb* pcb, t_estrucutra_cpu destino, t_estrucutra_cpu inicio) {
	char valor_destino;
	char valor_origen;

	valor_destino =obtener_valor(pcb, destino);
	valor_origen = obtener_valor(pcb, inicio);

    char resultado = valor_destino- valor_origen;
    setear(pcb, destino, resultado);
}



char* obtener_valor(t_pcb* pcb, t_estrucutra_cpu pos) {
    switch(pos) {
        case AX: return (char) pcb->contexto->registros_cpu->AX;
        case BX: return (char) pcb->contexto->registros_cpu->BX;
        case CX: return (char) pcb->contexto->registros_cpu->CX;
        case DX: return (char) pcb->contexto->registros_cpu->DX;
        default: log_info(logger, "Registro no reconocido"); return NULL;
    }
}


void iterator(char* value) {
	log_info(logger,"%s", value);
}


