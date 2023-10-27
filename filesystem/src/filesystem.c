
#include "filesystem.h"
#include<readline/readline.h>
int main(int argc, char* argv[]) {

	char *ruta_config = "./filesystem.config";

	config = cargar_config(ruta_config);

    logger_file_system = log_create("./filesystem.log", "FILESYSTEM", true, LOG_LEVEL_INFO);

    log_info(logger_file_system, "Soy el filesystem!");
    //obtener datos de .config
	obtener_configuracion();

	//-----------------------------------peticiones de kernel

	//añado "/" a la ruta principal para acceder a los bloques
	strcat(ruta_fcbs, "/");
//	char *nombre_fcb="prueba.fcb";
//	int tamanio_fcb=tamanio_fcb=abrir_archivo_fcb(nombre_fcb);
//	log_info(logger_file_system,"se leyo el tamanio %d",tamanio_fcb);
	log_info(logger_file_system,"abriendo  archivo fcb");
	crear_archivo_fcb("archivoPrueba.fcb");

	t_fcb* fcb_para_modif;
	//truncar_archivo(fcb_para_modif);

	terminar_programa(conexion_memoria, logger, config);
    return EXIT_SUCCESS;
}


void obtener_configuracion(){
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	puerto_escucha= config_get_string_value(config,"PUERTO_ESCUCHA");
	ruta_fcbs=config_get_string_value(config,"PATH_FCB");
}

//
//void iniciar_consola(){
//	logger_consola_filesystem = log_create("./filesystemConsola.log", "consola", 1, LOG_LEVEL_INFO);
//	char* valor;
//
//	while(1){
//		log_info(logger_consola_filesystem,"ingrese la operacion que deseas realizar"
//				"\n 1. generar conexion"
//				"\n 2. enviar mensaje"
//				"\n 3. iniciarComoServidor");
//		valor = readline("<");
//		switch (*valor) {
//			case '1':
//				log_info(logger_consola_filesystem, "generar conexion con memoria\n");
//				conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
//				break;
//			case '2':
//				log_info(logger_consola_filesystem, "enviar mensaje a memoria\n");
//				enviar_mensaje("filesystem a memoria", conexion_memoria);
//				break;
//			case '3':
//				log_info(logger_consola_filesystem, "se inicio el servidor\n");
//				iniciar_servidor(puerto_escucha);
//				break;
//			default:
//				log_info(logger_consola_filesystem,"no corresponde a ninguno");
//				exit(2);
//		}
//		free(valor);
//	}
//
//}


int iniciar_servidor_file_system(char *puerto){
	int servidor_fd = iniciar_servidor(puerto);
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(servidor_fd);

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			enviar_mensaje("te respondi el mensaje", cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
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
void iterator(char* value) {
	log_info(logger,"%s", value);
}

//--------------------funciones de file system

// abrir archivo; si el archivo existe se devuelve el tam, sino se informa que no existe
int abrir_archivo_fcb(char * nombre_fcb){

	char* nueva_copia_ruta;
	strcpy(nueva_copia_ruta,ruta_fcbs);//nueva=./utnso/
	strcat(nueva_copia_ruta, nombre_fcb);//ruta= ./utnso/nombre.fcb
	int tamanio_archivo;

	//creo el config del archivo fcb
	t_config* config_fcb=config_create(nueva_copia_ruta);

	if(config_fcb==NULL){
		log_info(logger_file_system,"no se encontro el archivo .fcb para leer el tamanio");
		tamanio_archivo=-1;
	}else{
		tamanio_archivo=config_get_int_value(config_fcb,"TAMANIO_ARCHIVO");
		log_info(logger_file_system,"se leyo el archivo de tamanio %i",tamanio_archivo);
		config_destroy(config_fcb);
	}
	return tamanio_archivo;
}
//crear fcb; tamnaio=0 y sin bloque inicial. Devuelve ok al finalizar. Siemprese puede crear
void crear_archivo_fcb(char* nom_fcb){

	char* ruta_copia=strcpy(ruta_copia,ruta_fcbs);
	strcat(ruta_copia,nom_fcb);
	//creo el archivo en el directorio de fcbs
	//inicializo las keys del archivo
	FILE* file_fcb=fopen(ruta_copia,"w");
	if(file_fcb){
		t_config* config_nuevo_fcb=cargar_config(ruta_copia);
		dictionary_put(config_nuevo_fcb->properties, "NOMBRE_ARCHIVO", nom_fcb);
		dictionary_put(config_nuevo_fcb->properties, "TAMANIO_ARCHIVO", "0");
		dictionary_put(config_nuevo_fcb->properties, "BLOQUE_INICIAL", "");


log_info(logger_file_system, "nombreqw %i",dictionary_size(config_nuevo_fcb->properties) );
		//guardo la config en un archivo ubicado en la ruta indicada
		config_save_in_file(config_nuevo_fcb,ruta_copia);

		log_info(logger_file_system,"se creo el archivo fcb ");

		fclose(file_fcb);
		config_destroy(config_nuevo_fcb);

	}else{
		log_info(logger_file_system, "hubo problemas creando el archivo fcb");
	}

}
void destruir_elem_diccionario(void* elem){
	free(elem);
}

void truncar_archivo(t_fcb* fcb_para_modif){
	ampliar_tam_archivo(fcb_para_modif);
	reducir_tam_archivo(fcb_para_modif);
}
//TODO investigar como hacer la asignacion de bloques
//ampliar_tam_archivo: actualiza tam en archivo fcb y asigna bloques neces para direccionar el nuevo tam
void ampliar_tam_archivo(t_fcb* fcb_para_modif){
	//modifico el tam del archivo
	uint32_t nuevo_tamanio;
	fcb_para_modif->tamanio_archivo=nuevo_tamanio;
}
//TODO Poner semaforo. modif_tam: se abre el config, se modif el fcb  y luego se lo cierrra.
void modificar_tam_fcb(t_fcb* fcb){

	char* ruta_ini=strcpy(ruta_ini, ruta_fcbs);
	string_append(&ruta_ini,fcb->nombre_archivo);

	t_config* config_fcb=config_create(ruta_ini);
	if(config_fcb!=NULL){
	config_set_value(config_fcb,"TAMANIO_ARCHIVO",(char*)fcb->tamanio_archivo);
	config_destroy(config);
	}else{
		log_info(logger_file_system, "no se pudo modificar el tamanio del archivo");
	}
}
//TODO implementar
void reducir_tam_archivo(t_fcb *fcb_para_modif){

}
//leer archivo: lee la info de un bloque a partir del puntero y la envia a memoria
void* leer_archivo(){

}

