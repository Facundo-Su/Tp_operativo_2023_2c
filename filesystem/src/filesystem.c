
#include "filesystem.h"
#include<readline/readline.h>
int main() {

	char *ruta_config = "filesystem.config";

	config_file_system = cargar_config(ruta_config);

    logger_file_system = log_create("./filesystem.log", "FILESYSTEM", true, LOG_LEVEL_INFO);

    log_info(logger_file_system, "Soy el filesystem!");
	obtener_configuracion();
	//-----------------------------------peticiones de kernel
	char *nombre_fcb="prueba";//TODO quitar cuando termine  de hacer pruebas
	int tamanio_fcb=abrir_archivo_fcb(nombre_fcb);
	log_info(logger_file_system,"archivo abierto, tamanio= %d",tamanio_fcb);

	crear_archivo_fcb("creado");
	log_info(logger_file_system,"archivo creado ");

	//liberar_recursos_fs();

    return EXIT_SUCCESS;
}
void liberar_recursos_fs(){
	free(ruta_fcbs);
	config_destroy(config_file_system);
	log_destroy(logger_file_system);
}

void obtener_configuracion(){
	//IPS,PUERTOS
	ip_memoria = config_get_string_value(config_file_system, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config_file_system, "PUERTO_MEMORIA");
	puerto_escucha= config_get_string_value(config_file_system,"PUERTO_ESCUCHA");
	//RUTAS
	ruta_fcbs=string_new();
	ruta_fcbs=config_get_string_value(config_file_system,"PATH_FCB");
	string_append(&ruta_fcbs,"/");

	ruta_bloques=config_get_string_value(config_file_system,"PATH_BLOQUES");
	ruta_fat=config_get_string_value(config_file_system,"PATH_FAT");

	//TAMNIOS, CANTIDADES
	cant_bloques_swap=config_get_int_value(config_file_system,"CANT_BLOQUES_SWAP");
	cant_bloques_total=config_get_int_value(config_file_system,"CANT_BLOQUES_TOTAL");
	tam_bloque=config_get_int_value(config_file_system,"TAM_BLOQUE"	);
	//RETARDOS
	retardo_acceso_bloq=config_get_int_value(config_file_system,"RETARDO_ACCESO_BLOQUE");
	retardo_acceso_fat=config_get_int_value(config_file_system,"RETARDO_ACCESO_FAT");


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

//arma la ruta para acceder al fcb
char* armar_ruta_fcb(char* nombre_fcb){
	char *nueva_ruta=malloc(string_length(ruta_fcbs)+1);
	strcpy(nueva_ruta,ruta_fcbs);
	string_append(&nueva_ruta,nombre_fcb);
	string_append(&nueva_ruta, ".fcb");
	return nueva_ruta;
}
// abrir archivo; si el archivo existe se devuelve el tam, sino se informa que no existe
int abrir_archivo_fcb(char * nombre_fcb){
	//preparo la ruta del archivo fcb
	char* nueva_ruta=armar_ruta_fcb(nombre_fcb);
	int tamanio_archivo;
	//creo el config del archivo fcb
	t_config* config_fcb=config_create(nueva_ruta);
	if(config_fcb==NULL){
		log_info(logger_file_system,"no se pudo leer el tamanio de %s",nueva_ruta);
		exit(1);
	}else{
		tamanio_archivo=config_get_int_value(config_fcb,"TAMANIO_ARCHIVO");
		config_destroy(config_fcb);
		//TODO crear la entrada a la FAT y en la tabla de proceso

	}
	if(nueva_ruta!=NULL){
		free(nueva_ruta);
	}
	return tamanio_archivo;
}
//crear fcb; tamnaio=0 y sin bloque inicial. Devuelve ok al finalizar. Siemprese puede crear
void crear_archivo_fcb(char* nom_fcb){
	char* ruta_copia=armar_ruta_fcb(nom_fcb);
	log_info(logger_file_system, "mensaje %s",ruta_copia);
	//creo el archivo en el directorio de fcbs
	FILE* file_fcb=txt_open_for_append(ruta_copia);
	if(file_fcb!=NULL){
		//inicializo las keys del archivo
		txt_write_in_file(file_fcb, "NOMBRE_ARCHIVO=");
		txt_write_in_file(file_fcb, nom_fcb);
		txt_write_in_file(file_fcb, "\nTAMANIO_ARCHIVO=0");
		txt_write_in_file(file_fcb, "\nBLOQUE_INICIAL=");

		txt_close_file(file_fcb);
	}else{
		log_info(logger_file_system, "hubo problemas creando el archivo fcb");
	}
	if(ruta_copia!=NULL){
		free(ruta_copia);
	}
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
	config_set_value(config_fcb,"TAMANIO_ARCHIVO",fcb->tamanio_archivo);
	config_destroy(config_fcb);
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

