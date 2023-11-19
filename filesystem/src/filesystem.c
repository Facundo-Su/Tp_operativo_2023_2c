#include "filesystem.h"


int main(int argc, char *argv[]) {

	char *ruta_config =string_new();
	if (argc == 2) {
	} else {
		perror(" error con ruta leida de config");
	}

	ruta_config = "./filesystem.config"; //argv[0] es el nom del programa en si mismo
	logger_file_system = log_create("./filesystem.log", "FILESYSTEM", true,LOG_LEVEL_INFO);
	log_info(logger_file_system, "Soy el filesystem!");
	config_file_system = cargar_config(ruta_config);
	obtener_configuracion();
	log_info(logger_file_system, "se cargo la configuracion");


	//inicializo las estructuras del file sytem
	inicializar_fs();
	log_info(logger_file_system,"se inicializo las estructuras de file system");
	// iniciar_servidor_file_system(puerto_escucha);

	//-----------------------------------peticiones de kernel
	//TODO quitar cuando termine  de hacer pruebas

	int tamanio_fcb = abrir_archivo_fcb("prueba");
	log_info(logger_file_system, "archivo abierto, tamanio= %d", tamanio_fcb);

//	t_fcb *fcb_prueba;
//	crear_archivo_fcb("creado", fcb_prueba);
//	log_info(logger_file_system, "archivo creado ");
//	t_fat * fat=inicializar_fat();
//	int cont=0;
//
for(int i=0;i<fs->fat->tamanio_fat;i++){
	if(i<10){
		log_info(logger_file_system,"indice %i =%u",i,fs->fat->entradas[i]);
		}

	}

	//crear_archivo_bloque();

	//liberar_recursos_fs();

	return EXIT_SUCCESS;
}
void liberar_recursos_fs() {
	if(fs!=NULL){

		if(fs->fcb_list!=NULL){
			//TODO
		}
		if(fs->fat!=NULL){
			if(fs->fat->entradas!=NULL){
				free(fs->fat->entradas);
			}

			free(fs->fat);
		}
		if(fs->array_swap!=NULL){
			//TODO
		}

		free(fs);
	}
}
//crea el array de bloques swap
t_swap* inicializar_array_swap(){
	t_swap* swap_array=malloc(cant_bloq_swap* sizeof(t_swap));
	return swap_array;
}
//crea el array de X tamanio, reserva la pos [0], y deja el resto en 0=libre
t_fat* inicializar_fat() {
	t_fat *fat =malloc( sizeof(t_fat));
	fat->tamanio_fat= cant_total_bloq - cant_bloq_swap;
	fat->entradas=malloc(fat->tamanio_fat*sizeof(uint32_t));
	//reservo entrada inicial para boot
	fat->entradas[0] = RESERV_BOOT;
	//inicializo las entradas en 0=libre
	for (int i = 1; i < fat->tamanio_fat; i++){
		fat->entradas[i] = 0;
	}

	return fat;
}

t_archivo_bloques* inicializar_bloques() {
t_archivo_bloques *bloques = malloc(tam_bloque*cant_total_bloq);
	return bloques;
}
void inicializar_fs() {
	fs=malloc(sizeof(t_FS));
	fs->fcb_list = list_create();
	fs->fat = inicializar_fat();
	fs->bloques = inicializar_bloques();
	fs->array_swap=inicializar_array_swap();
}
void obtener_configuracion() {
	//IPS,PUERTOS
	ip_memoria = config_get_string_value(config_file_system, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config_file_system,
			"PUERTO_MEMORIA");
	puerto_escucha = config_get_string_value(config_file_system,
			"PUERTO_ESCUCHA");
	//RUTAS
	ruta_fcbs = string_new();
	ruta_fcbs = config_get_string_value(config_file_system, "PATH_FCB");
	string_append(&ruta_fcbs, "/");

	ruta_bloques = config_get_string_value(config_file_system, "PATH_BLOQUES");
	ruta_fat = config_get_string_value(config_file_system, "PATH_FAT");
	//TAMNIOS, CANTIDADES
	cant_bloq_swap = config_get_int_value(config_file_system,
			"CANT_BLOQUES_SWAP");
	cant_total_bloq = config_get_int_value(config_file_system,
			"CANT_BLOQUES_TOTAL");
	tam_bloque = config_get_int_value(config_file_system, "TAM_BLOQUE");

	//RETARDOS
	retardo_acceso_bloq = config_get_int_value(config_file_system,
			"RETARDO_ACCESO_BLOQUE");
	retardo_acceso_fat = config_get_int_value(config_file_system,
			"RETARDO_ACCESO_FAT");

}

// cada hilo creado ejecuta connection_handler
void* connection_handler(void *socket_conexion) {
	int cliente_fd = (int) socket_conexion;
	t_list *lista;
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
//		case CREAR_ARCHIVO:
//			break;
//		case LEER_ARCHIVO:
//		break;
		case TRUNCAR_ARCHIVO:
			break;
		case ESCRIBIR_ARCHIVO:
			break;
		case INICIAR_PROCESO: //reserva bloques y reenvia la lista de bloques asignados
			int cant_bloq = recibir_cantidad_req_bloq(cliente_fd);

			break;
		case FINALIZAR_PROCESO:
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			break;
		default:
			log_warning(logger,
					"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
}

int iniciar_servidor_file_system(char *puerto) {
	log_info(logger_file_system, "esperando conexiones");
	int servidor_fd = iniciar_servidor(puerto);

	//creo un hilo por cada conexion
	while (1) {
	int socket_cliente_conectado = esperar_cliente(servidor_fd);
		pthread_t hilo;
		if (socket_cliente_conectado != -1) {
			pthread_create(&hilo, NULL, (void*) connection_handler,(void*) socket_cliente_conectado);
			pthread_detach(hilo);

			return 1;
		} else {
			perror("error conectando cliente");
		}
	}
	return 0;

//	while (1) {
//		struct sockaddr_in client_addr;
//	    socklen_t client_addr_len=sizeof(client_addr);
//	    int client_socket=accept(servidor_fd,(struct sockaddr*)&client_addr,&client_addr_len);
//	    if(client_socket<0){
//	    	perror("error al aceptar la conexion");
//	    	continue;//continua ejecutando el sig ciclo del while a pesar del fallo
//	    }
//	    pthread_t thread;
//	        int *socket_desc = malloc(sizeof(int));
//
//	        *socket_desc = client_socket;
//
//	        if (pthread_create(&thread, NULL, connection_handler, (void *)socket_desc) < 0) {
//	            perror("No se pudo crear el hilo de la conexion");
//	            free(socket_desc);
//	            close(client_socket);
//	            continue;
//	        }
//	        pthread_detach(thread); // Liberar recursos cuando el hilo termine
//	    }
//	close(servidor_fd);
}
void iterator(char *value) {
	log_info(logger, "%s", value);
}

//--------------------funciones de file system

//arma la ruta para acceder al fcb
char* armar_ruta_fcb(char *nombre_fcb) {
	char *nueva_ruta = malloc(string_length(ruta_fcbs) + 1);
	strcpy(nueva_ruta, ruta_fcbs);
	string_append(&nueva_ruta, nombre_fcb);
	string_append(&nueva_ruta, ".fcb");
	return nueva_ruta;
}

t_fcb* buscar_fcb(char *nombre_a_buscar) {
	t_fcb* fcb=NULL;
	for(int i=0;i<list_size(fs->fcb_list);i++){
				t_fcb* fcb_aux=(t_fcb*)list_get(fs->fcb_list,i);
				if(strcmp(fcb_aux->nombre_archivo,nombre_a_buscar)==0){
				fcb=fcb_aux;
			}
		}
		return fcb;
}
char* intAString(int numero) {
    // Determina el tamaño del buffer necesario para la cadena
    int tamano_buffer = snprintf(NULL, 0, "%d", numero);

    // Aloca memoria para el buffer
    char *cadena = (char *)malloc(tamano_buffer + 1);

    // Convierte el entero a una cadena de caracteres
    snprintf(cadena, tamano_buffer + 1, "%d", numero);

    return cadena;
}
// abrir archivo; si el archivo existe se devuelve el tam, sino -1 y se informa que no existe
int abrir_archivo_fcb(char *nombre_fcb) {
	//preparo la ruta del archivo fcb
	char *nueva_ruta = armar_ruta_fcb(nombre_fcb);
	int tamanio_archivo=-1;
	log_info(logger_file_system, "ruta copia %s",nueva_ruta);
	//creo el config del archivo fcb
	t_config  *config_fcb=config_create(nueva_ruta);
	if (config_fcb == NULL) {
		//no se encuentra el fcb
		log_info(logger_file_system, "no se pudo encontrar el fcb de  %s",nombre_fcb);

	} else {
		t_fcb* fcb_abierto=malloc(sizeof(t_fcb));
		fcb_abierto->nombre_archivo=nombre_fcb;
		fcb_abierto->bloq_inicial_archivo=config_get_int_value(config_fcb,"BLOQUE_INICIAL");
		fcb_abierto->tamanio_archivo=config_get_int_value(config_fcb,"TAMANIO_ARCHIVO");
		fcb_abierto->cant_aperturas++;

		tamanio_archivo = fcb_abierto->tamanio_archivo;
		//agrego a la lista fcbs abiertos
		list_add(fs->fcb_list,fcb_abierto);
		//crea la entrada a la FAT y le asigna los bloques
		asignar_entradas_fat(fcb_abierto);
		char *inicial_a_guardar =intAString(fcb_abierto->bloq_inicial_archivo);
		config_set_value(config_fcb,"TAMANIO_ARCHIVO",inicial_a_guardar);
		config_save(config_fcb);

		config_destroy(config_fcb);
		free(inicial_a_guardar);
		free(nueva_ruta);
	}
	return tamanio_archivo;
}

int buscar_entrada_libre_fat(){
	int index_entrada_libre;
	for(int i=0;i<fs->fat->tamanio_fat;i++){
		if(fs->fat->entradas[i]==0){
			index_entrada_libre=i;
			break;
		}
	}
	return index_entrada_libre;
}

int calcular_bloq_necesarios_fcb(int tam_bytes){
	int cant_bloq_necesarios;
	if(tam_bytes%tam_bloque==0){
		  cant_bloq_necesarios=tam_bytes/tam_bloque;
	 }else{
		 //los bytes restantes no llegan a ocupar 1 bloque
		 if(tam_bytes>tam_bloque){
			 cant_bloq_necesarios=tam_bytes/tam_bloque + 1;
		 }else{
			 //cant bytes<tamanio bloque
			 cant_bloq_necesarios=1;
		 }

	 }
	return cant_bloq_necesarios;
}
void asignar_entradas_fat(t_fcb* fcb_guardar){
	 int cant_bloq_necesarios=calcular_bloq_necesarios_fcb(fcb_guardar->tamanio_archivo);

	//asigno entrada al bloque inicial
	 int entrada_libre=buscar_entrada_libre_fat();
	 fcb_guardar->bloq_inicial_archivo=entrada_libre;
	 //marco la entrada asignada
	 fs->fat->entradas[entrada_libre]=MARCA_ASIG;
	 for(int i=1;i<cant_bloq_necesarios;i++){
		 int siguiente_entrada_libre=buscar_entrada_libre_fat();
		 //hago que la entrada libre anterior apunte a la nueva, marco la nueva entrada
		 fs->fat->entradas[entrada_libre]=siguiente_entrada_libre;
		 fs->fat->entradas[siguiente_entrada_libre]=MARCA_ASIG;
		 entrada_libre=siguiente_entrada_libre;
		 if(i==cant_bloq_necesarios){
			 fs->fat->entradas[entrada_libre]=EOFF;
		 }
	 }
		 //para el ultimo bloque uso EOF
	 	 fs->fat->entradas[entrada_libre]=EOFF;

}
//crear fcb; tamanio=0 y sin bloque inicial. Devuelve ok al finalizar. Siempre se puede crear
void crear_archivo_fcb(char *nom_fcb, t_fcb *fcb_creado) {
	char *ruta_copia = armar_ruta_fcb(nom_fcb);
	//creo el archivo en el directorio de fcbs
	FILE *file_fcb = txt_open_for_append(ruta_copia);
	if (file_fcb != NULL) {
		//inicializo las keys del archivo
		txt_write_in_file(file_fcb, "NOMBRE_ARCHIVO=");
		txt_write_in_file(file_fcb, nom_fcb);
		txt_write_in_file(file_fcb, "\nTAMANIO_ARCHIVO=0");
		txt_write_in_file(file_fcb, "\nBLOQUE_INICIAL=");
		//creo el fcb para administrarlo
		fcb_creado = malloc(sizeof(t_fcb));
		fcb_creado->nombre_archivo = nom_fcb;
		fcb_creado->tamanio_archivo = 0;

		txt_close_file(file_fcb);
		free(ruta_copia);
	} else {
		log_info(logger_file_system, "hubo problemas creando el archivo fcb %s",nom_fcb);
	}

}

void actualizar_tam_fcb(t_fcb *fcb, int tamanio_nuevo) {
	fcb->tamanio_archivo = fcb->tamanio_archivo + tamanio_nuevo;
}
//recibe el nuevo tamanio del archivo y modif el mismo
void truncar_archivo(t_fcb *fcb_para_modif, int nuevo_tamanio) {
	//TODO hacer reasignacion de bloques con tamanio nuevo
	if (fcb_para_modif->tamanio_archivo < nuevo_tamanio) {
		reducir_tam_archivo(fcb_para_modif, nuevo_tamanio);

	} else {
		//siempre se puede ampliar el archivo
		ampliar_tam_archivo(fcb_para_modif, nuevo_tamanio);

	}
}
//TODO investigar como hacer la asignacion de bloques
//ampliar_tam_archivo: actualiza tam en archivo fcb y asigna bloques neces para direccionar el nuevo tam
void ampliar_tam_archivo(t_fcb *fcb_para_modif, int tamanio_nuevo) {
	//modifico el tam del archivo
	fcb_para_modif->tamanio_archivo = tamanio_nuevo;
}
//TODO Poner semaforo. modif_tam: se abre el config, se modif el fcb  y luego se lo cierrra.
void guardar_tam_fcb(t_fcb *fcb) {
	char *ruta_ini = strcpy(ruta_ini, ruta_fcbs);
	string_append(&ruta_ini, fcb->nombre_archivo);

	t_config *config_fcb = config_create(ruta_ini);
	if (config_fcb != NULL) {
		config_set_value(config_fcb, "TAMANIO_ARCHIVO", fcb->tamanio_archivo);
		config_destroy(config_fcb);
	} else {
		log_info(logger_file_system,
				"no se pudo modificar el tamanio del archivo");
	}
}
//TODO implementar
void reducir_tam_archivo(t_fcb *fcb_para_modif, int tamanio) {

}
//leer archivo: lee la info de un bloque a partir del puntero y la envia a memoria
void leer_archivo() {
}
//se crear el archivo que contendra los bloques fat y swap
//archivo_lboque=swap + fat, tam_total esta en config y donde debe ubicarse tambien
void crear_archivo_bloque() {
	FILE *file_bloq = fopen(ruta_bloques, "wb+");
	int inicio_fat = cant_total_bloq - cant_bloq_swap;
	if (file_bloq == NULL) {
		log_info(logger_file_system, "");
		exit(1);
	} else {

		// Dividir el archivo en dos particiones
		//fat
		log_info(logger_file_system,
				"escribiendo en la posicion inicial de fat:%i", inicio_fat);
		fseek(file_bloq, inicio_fat, SEEK_SET);

		const char *cadena = "Hola, mundo!";
		int longitudCadena = strlen(cadena);
		// Escribir la cadena de caracteres en el archivo como datos binarios
		int elementosEscritos = fwrite(cadena, 1, longitudCadena, file_bloq);

		close(file_bloq);
	}
}

int recibir_cantidad_req_bloq(int socket_cliente) {
	int cantidad_bloq;
	if (recv(socket_cliente, &cantidad_bloq, sizeof(int), MSG_WAITALL) > 0)
		return cantidad_bloq;
	else {
		close(socket_cliente);
		return -1;
	}
}

//devuelve la lista de boques asignada al proceso
t_list* iniciar_proceso(uint32_t cant_bloques) {
	t_list *bloq_asignados = list_create();
	int index_bloq = buscar_bloq_libre_swap();
	list_add(bloq_asignados, index_bloq);
	return bloq_asignados;
}
//busco en la fat entrada con valor 0=libre y devuelvo el indice
int buscar_bloq_libre_swap() {
	int num_bloque;
	//primero reservado para boot
	for (int i = 0; i < cant_bloq_swap; i++) {
		if (fs->array_swap[i].libre) {
				num_bloque=i;
			break;
		}
	}
	return num_bloque;
}

