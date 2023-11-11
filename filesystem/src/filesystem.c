
#include "filesystem.h"
#include<readline/readline.h>

int main(int argc,char* argv[]) {

	char *ruta_config=string_new();
    if (argc == 2) {
	 ruta_config= argv[1];//argv[0] es el nom del programa en si mismo
    }else{
    	 printf(" error con ruta leida de config");
    }

	config_file_system = cargar_config(ruta_config);

    logger_file_system = log_create("./filesystem.log", "FILESYSTEM", true, LOG_LEVEL_INFO);

    log_info(logger_file_system, "Soy el filesystem!");

	obtener_configuracion();
	log_info(logger_file_system	, "se cargo la configuracion"	);
	//inicializo las estructuras del file sytem
	inicializar_fs();
	log_info(logger_file_system, "se inicializo las estructuras de file system");
	//-----------------------------------peticiones de kernel
	char *nombre_fcb="prueba";//TODO quitar cuando termine  de hacer pruebas

	int tamanio_fcb=abrir_archivo_fcb(nombre_fcb);
	log_info(logger_file_system,"archivo abierto, tamanio= %d",tamanio_fcb);

	t_fcb* fcb_prueba;
	crear_archivo_fcb("creado",fcb_prueba);
	log_info(logger_file_system,"archivo creado ");

	crear_archivo_bloques();

	log_info(logger_file_system, "inicio server file system");
	//TODO descomentar cuando solucione la conexion con los modulos
	//iniciar_servidor_file_system(puerto_escucha);


	//liberar_recursos_fs();

    return EXIT_SUCCESS;
}
void liberar_recursos_fs(){
	free(ruta_fcbs);
	config_destroy(config_file_system);
	log_destroy(logger_file_system);
}
//inicializo las estructuras de FS
t_fat* inicializar_fat(){
	const int long_entradas=cant_total_bloq-cant_bloq_swap;
	t_fat* fat=(t_fat*)malloc(long_entradas * sizeof(uint32_t));
	//reservo entrada inicial para boot
		fat->entradas[0]=RESERV_BOOT;
	//inicializo las entradas en 0
	for (int i=1;i<long_entradas;i++){
		fat->entradas[i]=0;
	}
	return fat;
}

t_archivo_bloques* inicializar_bloques(){
	//TODO controlar que el tamanio de bloque que dan sea en bytes
    t_archivo_bloques *bloques = (t_archivo_bloques*)malloc(tam_bloque*cant_total_bloq);

    // Inicializar bloques con '\0'
    for (uint32_t i = 0; i < cant_total_bloq; ++i) {
        memset(fs->bloques[i].datos, '\0', sizeof(fs->bloques[i].datos));
    }
    return bloques;
}
void inicializar_fs(){
	fs->fat=inicializar_fat();
	fs->bloques=inicializar_bloques();
	fs->fcb_list=list_create();
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
	 cant_bloq_swap=config_get_int_value(config_file_system,"CANT_BLOQUES_SWAP");
	 cant_total_bloq=config_get_int_value(config_file_system,"CANT_BLOQUES_TOTAL");
	 tam_bloque=config_get_int_value(config_file_system,"TAM_BLOQUE"	);

	//RETARDOS
	retardo_acceso_bloq=config_get_int_value(config_file_system,"RETARDO_ACCESO_BLOQUE");
	retardo_acceso_fat=config_get_int_value(config_file_system,"RETARDO_ACCESO_FAT");

}

// cada hilo creado ejecuta connection_handler
void* connection_handler(void* socket_conexion){
	log_info(logger_file_system, "se conecto un cliente a FS");
	int cliente_fd =*(int *)socket_conexion;
	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			enviar_mensaje("saludo desde file system", cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case ABRIR_ARCHIVO:

			break;
		case CREAR_ARCHIVO:
			break;
		case TRUNCAR_ARCHIVO:
			break;
		case LEER_ARCHIVO:
		case ESCRIBIR_ARCHIVO:
			break;
		case INICIAR_PROCESO:
			break;
		case FINALIZAR_PROCESO:
			break;

		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			break;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
}
void iniciar_servidor_file_system(char *puerto){
	log_info(logger_file_system, "esperando conexiones");
	int servidor_fd = iniciar_servidor(puerto);
	//creo un hilo por cada conexion
	while (1) {
		struct sockaddr_in client_addr;
	    socklen_t client_addr_len=sizeof(client_addr);
	    int client_socket=accept(servidor_fd,(struct sockaddr*)&client_addr,&client_addr_len);
	    if(client_socket<0){
	    	perror("error al aceptar la conexion");
	    	continue;//continua ejecutando el sig ciclo del while a pesar del fallo
	    }
	    pthread_t thread;
	        int *socket_desc = malloc(sizeof(int));

	        *socket_desc = client_socket;

	        if (pthread_create(&thread, NULL, connection_handler, (void *)socket_desc) < 0) {
	            perror("No se pudo crear el hilo de la conexion");
	            free(socket_desc);
	            close(client_socket);
	            continue;
	        }
	        pthread_detach(thread); // Liberar recursos cuando el hilo termine
	    }
	close(servidor_fd);
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
//TODO implementar
t_fcb* contiene_fcb(char* nombre_a_buscar){

	t_fcb* fcb_encontrado;
	return fcb_encontrado;
}
// abrir archivo; si el archivo existe se devuelve el tam, sino se informa que no existe
int abrir_archivo_fcb(char * nombre_fcb){
	//preparo la ruta del archivo fcb
	char* nueva_ruta=armar_ruta_fcb(nombre_fcb);
	int tamanio_archivo;
	//creo el config del archivo fcb
	t_fcb * fcb_encontrado=contiene_fcb(nombre_fcb);
	if(fcb_encontrado==NULL){
		//no se encuentra el fcb
		log_info(logger_file_system,"no se pudo encontrar el archivo %s",nueva_ruta);
		exit(1);
	}else{
		// se devuelve el tamanio del fcb
		uint32_t tamanio=fcb_encontrado->tamanio_archivo;
		//TODO crear la entrada a la FAT y en la tabla de proceso
		return tamanio;
	}
	if(nueva_ruta!=NULL){
		free(nueva_ruta);
	}
	return tamanio_archivo;
}
//crear fcb; tamnaio=0 y sin bloque inicial. Devuelve ok al finalizar. Siemprese puede crear
void crear_archivo_fcb(char* nom_fcb,t_fcb* fcb_creado){
	char* ruta_copia=armar_ruta_fcb(nom_fcb);
	//creo el archivo en el directorio de fcbs
	FILE* file_fcb=txt_open_for_append(ruta_copia);
	if(file_fcb!=NULL){
		//inicializo las keys del archivo
		txt_write_in_file(file_fcb, "NOMBRE_ARCHIVO=");
		txt_write_in_file(file_fcb, nom_fcb);
		txt_write_in_file(file_fcb, "\nTAMANIO_ARCHIVO=0");
		txt_write_in_file(file_fcb, "\nBLOQUE_INICIAL=");
		//creo el fcb del porceso para administrarlo
		fcb_creado=malloc(sizeof(t_fcb));
		fcb_creado->nombre_archivo=nom_fcb;
		fcb_creado->tamanio_archivo=0;
		txt_close_file(file_fcb);
	}else{
		log_info(logger_file_system, "hubo problemas creando el archivo fcb");
	}
	if(ruta_copia!=NULL){
		free(ruta_copia);
	}
}


void actualizar_tam_fcb(t_fcb *fcb,int tamanio_adicional){
	fcb->tamanio_archivo=fcb->tamanio_archivo+tamanio_adicional;
}
//recibe el nuevo tamanio del archivo y modif el mismo
void truncar_archivo(t_fcb* fcb_para_modif,int nuevo_tamanio){
	//TODO hacer reasignacion de bloques con tamanio nuevo
	if(fcb_para_modif->tamanio_archivo<nuevo_tamanio){
	reducir_tam_archivo(fcb_para_modif,nuevo_tamanio);

	}else{
		//siempre se puede ampliar el archivo
		ampliar_tam_archivo(fcb_para_modif,nuevo_tamanio);

	}
}
//TODO investigar como hacer la asignacion de bloques
//ampliar_tam_archivo: actualiza tam en archivo fcb y asigna bloques neces para direccionar el nuevo tam
void ampliar_tam_archivo(t_fcb* fcb_para_modif,int tamanio_nuevo){
	//modifico el tam del archivo
	fcb_para_modif->tamanio_archivo=tamanio_nuevo;
}
//TODO Poner semaforo. modif_tam: se abre el config, se modif el fcb  y luego se lo cierrra.
void guardar_tam_fcb(t_fcb* fcb){
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
void leer_archivo(){
}
//se crear el archivo que contendra los bloques fat y swap
//archivo_lboque=swap + fat, tam_total esta en config y donde debe ubicarse tambien
void crear_archivo_bloque(t_fcb* fcb){
	FILE *file_bloq=fopen(ruta_bloques,"wb+");
	int inicio_fat=cant_total_bloq-cant_bloq_swap;
	if(file_bloq==NULL){
		log_info(logger_file_system,"");
		exit(1);
	}else{

	 // Dividir el archivo en dos particiones
		//fat
		log_info(logger_file_system	,"escribiendo en la posicion inicial de fat:%i",inicio_fat);
	    fseek(file_bloq, inicio_fat, SEEK_SET);

	    const char *cadena = "Hola, mundo!";
	    int longitudCadena = strlen(cadena);
	    // Escribir la cadena de caracteres en el archivo como datos binarios
	    int elementosEscritos = fwrite(cadena, 1, longitudCadena, file_bloq);

	    close(file_bloq);
	}
}

//devuelve la lista de boques asignada al proceso
t_list* iniciar_proceso(uint32_t cant_bloques){
	t_list* bloq_asignados=list_create();

	int index_bloq=buscar_bloq_libre();
	list_add(bloq_asignados,index_bloq);
return bloq_asignados;
}
int buscar_bloq_libre(){
	//primero reservado para boot
	for(int i=1;i< fs->fat->tamanio_fat ;i++){
		if(fs->fat->entradas[i]==0){
			return i;
		}

	}
}


