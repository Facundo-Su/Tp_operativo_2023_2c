
#include "cpu.h"

int main(int argc, char* argv[]) {

	char *rutaConfig = "cpu.config";

	config = cargarConfig(rutaConfig);

    logger = log_create("./cpu.log", "CPU", true, LOG_LEVEL_INFO);
    log_info(logger, "Soy la cpu!");

    //iniciar configuraciones
	 obtenerConfiguracion();

	iniciarConsola();


	terminar_programa(conexion_memoria, logger, config);
    return 0;
}

void iniciarConsola(){
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
				iniciarServidor(puerto_escucha);
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

void obtenerConfiguracion(){
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
}
int iniciarServidor(char *puerto){
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
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case ENVIARREGISTROCPU:
			t_list* valoresCpu = list_create();
			valoresCpu= recibir_paquete(cliente_fd);
			log_info(logger, "ME LLEGARON");
			break;

/*		case EJECUTARINSTRUCIONES:
			t_pcb* pcb = malloc(sizeof(t_pcb));
			pcb = recibir_pcb(cliente_fd);
			t_list* instrucciones = list_create();
			instrucciones = pcb->listaInstruciones;
			int cantidadInstrucciones = list_size(instrucciones);
			for(int i =0;i<cantidadInstrucciones;i++){
				t_instruccion* instruccion = list_get(instrucciones,i);
				ejecutar(pcb,instruccion,cliente_fd);
			}*/

		case RECIBIR_PCB:

			t_pcb* pcb = recibir_pcb(cliente_fd);
			log_info(logger_consola_cpu,"%d",pcb->pid);
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

void ejecutar(t_pcb* pcb, t_instruccion* instrucciones,int conexion){
	t_estrucutra_cpu registroAux;
	t_estrucutra_cpu registroAux2;
	char* parametro;
	char* parametro2;
	switch(instrucciones->nombre){
	case SET:
		parametro2= list_get(instrucciones,1);
		parametro= list_get(instrucciones,0);
		registroAux = devolver_registro(parametro);
		setear(pcb,registroAux,parametro2);
		break;
	case SUB:
		parametro= list_get(instrucciones,0);
		parametro2= list_get(instrucciones,1);
		registroAux = devolver_registro(parametro);
		registroAux2 = devolver_registro(parametro2);
		restar(pcb, registroAux, registroAux2);
		break;
	case SUM:
		parametro= list_get(instrucciones,0);
		parametro2= list_get(instrucciones,1);
		registroAux = devolver_registro(parametro);
		registroAux2 = devolver_registro(parametro2);
		sumar(pcb, registroAux, registroAux2);
		break;
	case EXIT:
		enviar_Pcb(pcb,conexion,FINALIZAR);
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

	valor_destino =obtenerValor(pcb, destino);
	valor_origen = obtenerValor(pcb, inicio);

    char resultado = valor_destino+ valor_origen;
    setear(pcb, destino, resultado);

}




void restar(t_pcb* pcb, t_estrucutra_cpu destino, t_estrucutra_cpu inicio) {
	char valor_destino;
	char valor_origen;

	valor_destino =obtenerValor(pcb, destino);
	valor_origen = obtenerValor(pcb, inicio);

    char resultado = valor_destino- valor_origen;
    setear(pcb, destino, resultado);
}



char* obtenerValor(t_pcb* pcb, t_estrucutra_cpu pos) {
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


