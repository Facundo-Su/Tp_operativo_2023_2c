#include "filesystem.h"

int main(int argc, char *argv[]) {

	char *ruta_config = "./filesystem.config"; //argv[0] es el nom del programa en si mismo
	if (argc != 2) {
	} else {
		perror(" error con ruta leida de config");
	}

	logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);

	logger_file_system = log_create("./filesystem.log", "FILESYSTEM", true,
			LOG_LEVEL_INFO);

	log_info(logger_file_system, "Soy el filesystem!");

	config_file_system = cargar_config(ruta_config);
	obtener_configuracion();
	log_info(logger_file_system, "se cargo la configuracion");

	inicializar_fs();

	log_info(logger_file_system,"se inicializo las estructuras de file system");

	//TODO quitar cuando termine  de hacer pruebas
	int prueba;
	prueba=abrir_archivo_fcb("pastel");
	crear_archivo_fcb("tarta");
	truncar_archivo("tarta", 16*4);

	for(uint32_t i=0;i<fs->fat->tamanio_fat;i++){
		log_info(logger_file_system,"muestra la fat %i=%u",i,fs->fat->entradas[i]);
	}
	log_info(logger_file_system,"salida del for");
	int*prueba2=malloc(sizeof(int));
	*prueba2=9;

	escribir_bloque_fat(16*2, "tarta", *prueba2);

	uint32_t*lectura=leer_archivo_bloques_fat(16*2, "tarta");
	log_info(logger_file_system,"lectura de fat %i",*lectura);
//	log_info(logger_file_system,"muestra la fat %i",prueba);
//	int pruebaescritura=10;
//	escribir_bloque_fat(16*3,"chau",&pruebaescritura);
//    int marca_reservado=7;
//	void* buffer_bloque=fs->bloques;
//	log_info(logger_file_system,"contenido %i",*pruebaescritura);

//	memcpy(buffer_bloque,&marca_reservado,tam_bloque);
//	memcpy(buffer_bloque +(tam_bloque),&marca_reservado,tam_bloque);
//	memcpy(buffer_bloque+ (tam_bloque*2),&marca_reservado,tam_bloque);
//	int* lectura=malloc(tam_bloque);
//	memcpy(lectura,buffer_bloque+ (tam_bloque*2),tam_bloque);
//	log_info(logger_file_system,"contenido %i",*lectura);



//	 int marca_reservado=0;//ascii 0=caracter nulo
//	void* buffer_bloque=fs->bloques;
//
//	memset(buffer_bloque,marca_reservado,tam_bloque);
//	memset(buffer_bloque +(tam_bloque),marca_reservado,tam_bloque);
//	memset(buffer_bloque+ (tam_bloque*2),marca_reservado,tam_bloque);

	//iniciar_servidor_fs(puerto_escucha);
	//liberar_recursos_fs();

	//levantar_archivo_bloques();
	return EXIT_SUCCESS;
}
void probarBloques(){
	log_info(logger_file_system,"se inicializo las estructuras de file system");
	int var= 23;
	memcpy(fs->bloques,&var,sizeof(int));
	var=45;
	memcpy(fs->bloques + sizeof(int), &var, sizeof(int));
	var=67;
	memcpy(fs->bloques + sizeof(int)*2, &var, sizeof(int));
	var=68;
	memcpy(fs->bloques + sizeof(int)*3, &var, sizeof(int));
	var=69;
	memcpy(fs->bloques + sizeof(int)*4, &var, sizeof(int));
	log_info(logger_file_system, "puntero bloques : %p",fs->bloques);
	log_info(logger_file_system, "valor de bloques en 0 : %c",*((int *)fs->bloques));
	log_info(logger_file_system, "valor de bloques en 1 : %c",*((int *)fs->bloques+1));
	log_info(logger_file_system, "valor de bloques en 2 : %c",*((int *)fs->bloques+2));
	void* aux_final=fs->bloques+ (tam_bloque*cant_total_bloq);
	memcpy(aux_final,&var,1);
}
void porbarTruncarArchivo(char *nombre) {
	crear_archivo_fcb(nombre);

	//truncar_archivo("unoMas", tam_bloque*2);
	for (int i = 0; i < 20; i++) {
		log_info(logger_file_system, "%i =%u", i, fs->fat->entradas[i]);
	}

	truncar_archivo("nuevo", tam_bloque * 6);

	for (int i = 0; i < 20; i++) {
		log_info(logger_file_system, "ampliado %i =%u", i,
				fs->fat->entradas[i]);
	}
	truncar_archivo("nuevo", tam_bloque * 3);
	for (int i = 0; i < 20; i++) {
		log_info(logger_file_system, "reducido %i =%u", i,
				fs->fat->entradas[i]);
	}
	truncar_archivo("nuevo", tam_bloque * 7);
	for (int i = 0; i < 20; i++) {
		log_info(logger_file_system, "amplicar otra vez %i =%u", i,
				fs->fat->entradas[i]);
	}
}

void liberar_fcb(t_fcb *fcb) {

	free(fcb);
}
void liberar_recursos_fs() {
	if (fs != NULL) {

		if (fs->fcb_list != NULL) {
			list_destroy_and_destroy_elements(fs->fcb_list,	(void*) liberar_fcb);
			free(ruta_fcbs);
		}
		if (fs->fat != NULL) {
			if (fs->fat->entradas != NULL) {
                munmap(fs->fat->entradas, fs->fat->tamanio_fat*tam_bloque);
			}

			free(fs->fat);
		}
		if (fs->array_swap != NULL) {
			free(fs->array_swap);
		}
		if(fs->bloques!=NULL){
			munmap(fs->bloques,cant_total_bloq*tam_bloque	);
		}
		free(fs);
	}
}
//crea el array de bloques swap
t_swap* inicializar_array_swap() {
	t_swap *swap_array = malloc(cant_bloq_swap * sizeof(t_swap));
	for(int i=0;i<cant_bloq_swap;i++){
		swap_array[i].libre=true;
	}
	return swap_array;
}
//crea el array de X tamanio, reserva la pos [0], y deja el resto en 0=libre
void inicializar_fat() {

	fs->fat= malloc(sizeof(t_fat));
	fs->fat->tamanio_fat = cant_total_bloq - cant_bloq_swap;
	levantar_fat();

}

void inicializar_fs() {
	fs = malloc(sizeof(t_FS));
	fs->fcb_list = list_create();
	levantar_fcbs();
	log_info(logger_file_system, "Se levanto el archivo FCBs");
	levantar_archivo_bloques();
	 inicializar_fat();

	fs->array_swap = inicializar_array_swap();
}
void obtener_configuracion() {
	//IPS,PUERTOS
	ip_memoria = config_get_string_value(config_file_system, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config_file_system,
			"PUERTO_MEMORIA");
	puerto_escucha = config_get_string_value(config_file_system,
			"PUERTO_ESCUCHA");
	//RUTAS

	char *aux_fcb= config_get_string_value(config_file_system, "PATH_FCB");
	ruta_fcbs=malloc(strlen(aux_fcb)+2);
	strcpy(ruta_fcbs,aux_fcb);
	strcat(ruta_fcbs,"/");


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
void* procesar_conexion(void* conexion1) {
	t_list *lista;
	int *conexion = (int*)conexion1;
	int cliente_fd = *conexion;

	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			enviar_mensaje("saludo desde file system", cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me un paquete");

			break;
		case ABRIR_ARCHIVO:
			t_list* nose = recibir_paquete(cliente_fd);
			char *nombre_nose = list_get(nose,0);
			log_info(logger_file_system, "llegue %i vez ",prueba);
			prueba++;
			log_info(logger_file_system, "Abrir Archivo: <%s>",nombre_nose);

			int tamanio_archivo = abrir_archivo_fcb(nombre_nose);
			enviar_tamanio_archivo(tamanio_archivo, cliente_fd);
			break;
		case CREAR_ARCHIVO:
			lista=recibir_paquete(cliente_fd);
			char* nombre_a_crear=list_get(lista,0);

			log_info(logger_file_system, "Crear Archivo: <%s>",nombre_a_crear);
			crear_archivo_fcb(nombre_a_crear);
			enviar_respuesta_crear_archivo(cliente_fd);
			break;
		case LEER_ARCHIVO:
			//recibe punteto desde el cual leer
			lista=recibir_paquete(cliente_fd);
			char *nombre_arc = list_get(lista,0);
			int *puntero = list_get(lista,1);

			log_info(logger_file_system	, "Leer Archivo: < %s > - Puntero: < %i > - Memoria: < falta>",nombre_arc,*puntero);

			void*leido=leer_archivo_bloques_fat(*puntero, nombre_arc);

			break;
		case DATOS_SWAP:
			lista = recibir_paquete(cliente_fd);
			int* posicion_swap_datos_swap = list_get(lista,0);
			void* datos_swap_retornar = malloc(tam_bloque);

			datos_swap_retornar=leer_bloque_swap(*posicion_swap_datos_swap);
			enviar_bloque_para_memoria(datos_swap_retornar,cliente_fd);
			break;
		case REMPLAZAR_PAGINA:
			lista = recibir_paquete(cliente_fd);
			int* posicion_swap_datos_swap2 = list_get(lista,0);
			void* datos_swap_retornar2 = list_get(lista,1);

			reemplazar_bloq_swap(*posicion_swap_datos_swap2,datos_swap_retornar2);
			break;
		case TRUNCAR_ARCHIVO:
			lista=recibir_paquete(cliente_fd);

			char *nombre = list_get(lista,0);

			int *tamanio_nuevo = list_get(lista,1);

			log_info(logger_file_system, "Truncar Archivo: < %s > - Tamaño: < %i >",nombre,*tamanio_nuevo);

			truncar_archivo(nombre, tamanio_nuevo);
			enviar_respuesta_truncar(cliente_fd);//1=OK
			break;
		case ESCRIBIR_ARCHIVO:
			lista=recibir_paquete(cliente_fd);

			break;
		case INICIAR_PROCESO: //reserva bloques y reenvia la lista de bloques asignados
			lista = recibir_paquete(cliente_fd);
			int *cant_bloq = list_get(lista,0);
			log_info(logger_file_system, "Cantidad de bloques swap a reservar %i",*cant_bloq);
			t_list *lista_asignados = iniciar_proceso(*cant_bloq);
			enviar_bloques_asignados_swap(lista_asignados,cliente_fd);
			break;
		case FINALIZAR_PROCESO:
			lista=recibir_paquete(cliente_fd);
			int* tam_lista=list_get(lista,0);
			t_list* bloq_a_liberar=list_create();

			for(int i=1;i< *tam_lista;i++){
				int *bloq=list_get(lista,i);
				list_add(bloq_a_liberar,	*bloq);
			}

			finalizar_proceso(bloq_a_liberar);
			log_info(logger_file_system, "Proceso finalizado, bloq swap a liberados: < %i>",*tam_lista);
			list_destroy(bloq_a_liberar);
			break;
		default:
			log_warning(logger,
					"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
}
enviar_bloque_para_memoria(void* datos_swap_retornar,int cliente_fd){
	t_paquete* paquete=crear_paquete(DATOS_SWAP);
	agregar_a_paquete(paquete, datos_swap_retornar,tam_bloque);
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}


void enviar_respuesta_truncar(int socket_cliente){
	t_paquete* paquete=crear_paquete(OK_TRUNCAR_ARCHIVO);

	agregar_a_paquete(paquete, 1,sizeof(int) );
	enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);
}
t_fcb* devolver_fcb(char *nombre) {

	for (int i = 0; i < list_size(fs->fcb_list); i++) {
		t_fcb *fcb_buscado = list_get(fs->fcb_list, i);
		if (strcmp(fcb_buscado->nombre_archivo, nombre) == 0) {
			log_info(logger_file_system, "se encontro el archivo %s",
					fcb_buscado->nombre_archivo);

			return fcb_buscado;
		}

	}

	log_info(logger_file_system,
			"El archivo buscado no se encuentra en la lista.");
	return NULL;
}
int recibir_entero(int socket_cliente) {
	int valor;
	if (recv(socket_cliente, &valor, sizeof(int), MSG_WAITALL) > 0)
		return valor;
	else {
		close(socket_cliente);
		return -1;
	}
}

void enviar_respuesta_crear_archivo(int cliente_fd) {

	t_paquete *paquete = crear_paquete(RESPUESTA_CREAR_ARCHIVO);
	agregar_a_paquete(paquete, 0, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}
void enviar_tamanio_archivo(int tamanio, int cliente_fd) {

	t_paquete *paquete = crear_paquete(RESPUESTA_ABRIR_ARCHIVO);

	agregar_a_paquete(paquete, &tamanio, sizeof(int));
	enviar_paquete(paquete, cliente_fd);
	eliminar_paquete(paquete);
}
void enviar_bloques_asignados_swap(t_list *lista_asignados, int socket_cliente) {

	t_paquete *paquete = crear_paquete(RESPUESTA_INICIAR_PROCESO_FS);
	int cantidad_de_bloque = list_size(lista_asignados);
	log_info(logger_file_system,"%i",list_size(lista_asignados));

	agregar_a_paquete(paquete, &cantidad_de_bloque, sizeof(int));
	for (int i = 0; i < list_size(lista_asignados); i++) {
		int* valor =list_get(lista_asignados, i);
		agregar_a_paquete(paquete,&valor,sizeof(int));
	}
	enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);
}

void iniciar_servidor_fs(char *puerto) {
	int fs_fd = iniciar_servidor(puerto);
	log_info(logger_file_system, "Servidor listo para recibir al cliente");

	while (1) {
		int cliente_fd = esperar_cliente(fs_fd);
		pthread_t atendiendo;
		pthread_create(&atendiendo, NULL, (void*) procesar_conexion,(void*) &cliente_fd);
		pthread_detach(atendiendo);

	}
}

//--------------------funciones de file system

//arma la ruta para acceder al fcb
char* armar_ruta_fcb(char *nombre_fcb) {
	char *nueva_ruta = malloc(string_length(ruta_fcbs)+string_length(nombre_fcb)+1);
	strcpy(nueva_ruta, ruta_fcbs);
	string_append(&nueva_ruta, nombre_fcb);
	string_append(&nueva_ruta, ".fcb");
	return nueva_ruta;
}

t_fcb* buscar_fcb(char *nombre_a_buscar) {
	t_fcb *fcb = NULL;
	for (int i = 0; i < list_size(fs->fcb_list); i++) {
		t_fcb *fcb_aux = (t_fcb*) list_get(fs->fcb_list, i);
		if (strcmp(fcb_aux->nombre_archivo, nombre_a_buscar) == 0) {
			fcb = fcb_aux;
		}
	}
	return fcb;
}
char* intAString(int numero) {

	// Determina el tamaño del buffer necesario para la cadena
	int tamano_buffer = snprintf(NULL, 0, "%d", numero);

	// Aloca memoria para el buffer
	char *cadena = (char*) malloc(tamano_buffer + 1);

	// Convierte el entero a una cadena de caracteres
	snprintf(cadena, tamano_buffer + 1, "%d", numero);

	return cadena;
}
char* recibir_nombre_archivo(int socket_cliente) {
	int size;
	char *nombre = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el archivo %s", nombre);
	//free(nombre);
	return nombre;
}
// abrir archivo; si el archivo existe se agrega a list_fcb y devuelve el tam sino -1
int abrir_archivo_fcb(char *nombre_fcb) {
	//preparo la ruta del archivo fcb
	int tamanio=-1;
	t_fcb* fcb=NULL;
	fcb=devolver_fcb(nombre_fcb);
	if(fcb!=NULL){
		tamanio=fcb->tamanio_archivo;
		return tamanio;
	}
	return tamanio;

}

uint32_t buscar_entrada_libre_fat() {
	uint32_t index_entrada_libre;
	for (uint32_t i = 1; i < fs->fat->tamanio_fat; i++) {
		if (fs->fat->entradas[i] == 0 && fs->fat->entradas[i ]!=UINT32_MAX) {
			log_info(logger_file_system, "se encontro la entrada %u", fs->fat->entradas[i]);
			index_entrada_libre = i;
			break;
		}
	}
	return index_entrada_libre;
}

int calcular_bloq_necesarios_fcb(int tam_bytes) {
	int cant_bloq_necesarios;
	if (tam_bytes % tam_bloque == 0) {
		cant_bloq_necesarios = tam_bytes / tam_bloque;
	} else {
		//los bytes restantes no llegan a ocupar 1 bloque
		if (tam_bytes > tam_bloque) {
			cant_bloq_necesarios = tam_bytes / tam_bloque + 1;
		} else {
			//cant bytes<tamanio bloque
			cant_bloq_necesarios = 1;
		}

	}
	return cant_bloq_necesarios;
}
void asignar_entradas_fat(t_fcb *fcb_guardar) {
	int cant_bloq_necesarios = calcular_bloq_necesarios_fcb(
			fcb_guardar->tamanio_archivo);
	log_info(logger_file_system, "cantidad  de bloq a asignar a %s %i",fcb_guardar->nombre_archivo, cant_bloq_necesarios);
	//asigno entrada al bloque inicial
	int entrada_libre = buscar_entrada_libre_fat();
	fcb_guardar->bloq_inicial_archivo = entrada_libre;
	//marco la entrada asignada
	fs->fat->entradas[entrada_libre] = MARCA_ASIG
	;
	for (uint32_t i = 1; i < cant_bloq_necesarios; i++) {
		uint32_t siguiente_entrada_libre = buscar_entrada_libre_fat();
		//hago que la entrada libre anterior apunte a la nueva, marco la nueva entrada
		fs->fat->entradas[entrada_libre] = siguiente_entrada_libre;
		fs->fat->entradas[siguiente_entrada_libre] = MARCA_ASIG
		;
		entrada_libre = siguiente_entrada_libre;
		if (i == cant_bloq_necesarios) {
			fs->fat->entradas[entrada_libre] = EOFF
			;
		}

	}
	fs->fat->entradas[entrada_libre] = EOFF
	escribir_fcb_en_archivo(fcb_guardar);
	//para el ultimo bloque uso EOF
	;

}
//crear fcb; tamanio=0. Devuelve ok al finalizar. Siempre se puede crear
void crear_archivo_fcb(char *nom_fcb) {
	t_fcb *fcb_creado = malloc(sizeof(t_fcb));
	char *ruta_copia = armar_ruta_fcb(nom_fcb);
	//creo el archivo en el directorio de fcbs
	FILE *file_fcb = txt_open_for_append(ruta_copia);
	if (file_fcb != NULL) {
		//inicializo las keys del archivo
		txt_write_in_file(file_fcb, "\nNOMBRE_ARCHIVO=");
		txt_write_in_file(file_fcb, nom_fcb);
		txt_write_in_file(file_fcb, "\nTAMANIO_ARCHIVO=0");

		//creo el fcb para administrarlo
		fcb_creado = malloc(sizeof(t_fcb));
		fcb_creado->nombre_archivo = nom_fcb;
		fcb_creado->tamanio_archivo = 0;

		//se asigna un bloque para luego truncar por ampliacion
		uint32_t entrada = buscar_entrada_libre_fat();
		char *string_entrada = intAString(entrada);
		fcb_creado->bloq_inicial_archivo = entrada;
		fs->fat->entradas[entrada] = UINT32_MAX;

		log_info(logger_file_system, "entrada reservada en crear %u", entrada);
		txt_write_in_file(file_fcb, "\nBLOQUE_INICIAL=");
		txt_write_in_file(file_fcb, string_entrada);
		txt_close_file(file_fcb);
		list_add(fs->fcb_list, fcb_creado);

		free(ruta_copia);

	} else {
		log_info(logger_file_system, "hubo problemas creando el archivo fcb %s",
				nom_fcb);
	}
}

void actualizar_lista_fcbs(t_list *lista_fcbs) {
	for (int i = 0; i < list_size(lista_fcbs); i++) {
		t_fcb *fcb_a_guardar = list_get(lista_fcbs, i);
		char *ruta_fcb = armar_ruta_fcb(fcb_a_guardar->nombre_archivo);
		t_config *config_fcb = cargar_config(ruta_fcb);
		char *string_de_inicio = intAString(
				fcb_a_guardar->bloq_inicial_archivo);
		char *string_de_tamanio = intAString(fcb_a_guardar->tamanio_archivo);
		config_set_value(config_fcb, "BLOQUE_INICIAL", string_de_inicio);
		config_set_value(config_fcb, "TAMANIO_INICIAL", string_de_tamanio);
	}
}
//recibe el nuevo tamanio del archivo y modif el mismo
void truncar_archivo(char *nombre, int nuevo_tamanio_bytes) {

	t_fcb *fcb = devolver_fcb(nombre);

	if (fcb->tamanio_archivo < nuevo_tamanio_bytes) {
		ampliar_tam_archivo(fcb, nuevo_tamanio_bytes);

	} else {
		reducir_tam_archivo(fcb, nuevo_tamanio_bytes);
		//siempre se puede ampliar el archivo
	}
	escribir_fcb_en_archivo(fcb);
}
void escribir_fcb_en_archivo(t_fcb *fcb) {
	char *ruta_fcb = armar_ruta_fcb(fcb->nombre_archivo);
	char *string_inicio = intAString(fcb->bloq_inicial_archivo);
	t_config *config_fcb = cargar_config(ruta_fcb);
	config_set_value(config_fcb, "BLOQUE_INICIAL", string_inicio);
	char *string_tamanio = intAString(fcb->tamanio_archivo);
	config_set_value(config_fcb, "TAMANIO_ARCHIVO", string_tamanio);
	config_save(config_fcb);
}
//ampliar_tam_archivo:se reservan nuevos bloques y se "concatenan" a los ya asignados
void ampliar_tam_archivo(t_fcb *fcb, int tamanio_nuevo_bytes) {

	int bloques_archivo = calcular_bloq_necesarios_fcb(fcb->tamanio_archivo);
	uint32_t indice = fcb->bloq_inicial_archivo;
	log_info(logger_file_system, "bloque inicial %u",
			fcb->bloq_inicial_archivo);
	//me paro en el ultimo bloque
	while (fs->fat->entradas[indice] != UINT32_MAX) {
		log_info(logger_file_system, "Acceso FAT - Entrada: <%u> - Valor: <%u>", indice,fs->fat->entradas[indice]);
		indice = fs->fat->entradas[indice];

	}
	//caso de archivo nuevo creado
	if (fcb->tamanio_archivo == 0) {
		fcb->tamanio_archivo = tam_bloque;
	}

	//asigno nuevos bloques
	int bytes = tamanio_nuevo_bytes - fcb->tamanio_archivo;
	int cant_nueva_bloques = bytes / tam_bloque;

	for (uint32_t i = 0; i < cant_nueva_bloques; i++) {
		uint32_t bl_libre = buscar_entrada_libre_fat();
		fs->fat->entradas[indice] = bl_libre;
		indice = bl_libre;
		fs->fat->entradas[indice] = EOFF
		;
	}
	fcb->tamanio_archivo = tamanio_nuevo_bytes;
}

void reducir_tam_archivo(t_fcb *fcb, int tamanio_nuevo_bytes) {

	uint32_t indice_final = fcb->bloq_inicial_archivo;
	uint32_t anterior_indice = EOFF
	;

	//me paro en el ultimo bloque
	while (fs->fat->entradas[indice_final] != UINT32_MAX) {
		anterior_indice = indice_final;
		indice_final = fs->fat->entradas[indice_final];

	}
	//libero bloques desde el final
	int diferencia_bytes = fcb->tamanio_archivo - tamanio_nuevo_bytes;
	int cant_bloq_a_liberar = calcular_bloq_necesarios_fcb(diferencia_bytes);
	int cont = 0;
	while (cont < cant_bloq_a_liberar) {
		fs->fat->entradas[indice_final] = 0;
		fs->fat->entradas[anterior_indice] = EOFF;

		//me paro nuevamente en el ultimo bloque
		indice_final = fcb->bloq_inicial_archivo;
		anterior_indice = EOFF;
		while (fs->fat->entradas[indice_final] != UINT32_MAX) {
			log_info(logger_file_system, "Acceso FAT - Entrada: <%u> - Valor: <%u>", indice_final,fs->fat->entradas[indice_final]);
			anterior_indice = indice_final;
			indice_final = fs->fat->entradas[indice_final];
		}

		cont++;
	}
	fcb->tamanio_archivo = tamanio_nuevo_bytes;
}
uint32_t obtener_bloque_por_indice(t_fcb *fcb, uint32_t indice_bloque) {
	uint32_t bloque = 0;
	bloque = fcb->bloq_inicial_archivo;
	while (bloque <= indice_bloque) {
		bloque = fs->fat->entradas[bloque];
	}
	return bloque;
}

//leer archivo: lee la info de un bloque a partir del puntero y la envia a memori
void* leer_archivo_bloques_fat(int puntero, char *nombre) {

	uint32_t numero_bloque = puntero / tam_bloque;
	t_fcb* fcb=devolver_fcb(nombre);

	uint32_t bloque=fcb->bloq_inicial_archivo;

	//busco en fat las pos de bloque
	int cont=1;
	while(cont<numero_bloque){
		bloque=fs->fat->entradas[bloque];
		cont++;
	}

	log_info(logger_file_system, "bloque a leer de fat %u", bloque);


	//leo
	void *datos = malloc(tam_bloque);
	void *puntero_buffer = fs->bloques;
	void* puntero_fat = puntero_buffer +(cant_bloq_swap*tam_bloque);
	usleep(retardo_acceso_bloq*1000);
	memcpy(datos, puntero_fat+(bloque*tam_bloque), tam_bloque);

	return datos;
}

void escribir_bloque_fat(int puntero, char* nombre,void* a_escribir){
	uint32_t numero_bloque = puntero / tam_bloque;
	t_fcb* fcb=devolver_fcb(nombre);

	uint32_t bloque=fcb->bloq_inicial_archivo;

	int cont=1;
	while(cont<numero_bloque){
		usleep(retardo_acceso_fat*1000);
		log_info(logger_file_system, "Acceso FAT - Entrada: <%u> - Valor: <%u>", bloque,fs->fat->entradas[bloque]);
		bloque=fs->fat->entradas[bloque];//obtengo el bloque
		cont++;

	}

	void* puntero_fat= fs->bloques +(cant_bloq_swap*tam_bloque);
	int* valor_entero = (int*)puntero_fat;
	if(puntero_fat<fs->bloques+(tam_bloque*cant_total_bloq)){
		usleep(retardo_acceso_bloq*1000);
		memcpy(puntero_fat+(tam_bloque*bloque),a_escribir,tam_bloque);
		log_info(logger_file_system, "bloque a escrito de fat %u", bloque);
	}else{
		log_info(logger_file_system,"exceso");
	}

}
void reemplazar_bloq_swap(int num_bloque,void *a_escribir) {

	log_info(logger_file_system, "Acceso a bloque swap <%u>", num_bloque);
	if(fs->bloques+(tam_bloque*num_bloque)==NULL){

		log_info(logger_file_system, "es puntero es nulo");
	}
	//int *a = (int*)a_escribir;
//	log_info(logger_file_system, "se recibio para escribir el puntero %i",*a	);
	void* buffer_bloque=fs->bloques;
	usleep(retardo_acceso_bloq*1000);
	memcpy(buffer_bloque+(tam_bloque*num_bloque),a_escribir,tam_bloque);
	log_info(logger_file_system, "se escribio en el bloq de swap :%u",num_bloque);

}
void escribir_bloque_swap(int puntero,void *a_escribir) {

	uint32_t num_bloque = puntero / tam_bloque;
	log_info(logger_file_system, "Acceso a bloque swap <%u>", num_bloque);
	if(fs->bloques+(tam_bloque*num_bloque)==NULL){
		log_info(logger_file_system, "es puntero es nulo");
	}
	//int *a = (int*)a_escribir;
//	log_info(logger_file_system, "se recibio para escribir el puntero %i",*a	);
	void* buffer_bloque=fs->bloques;
	usleep(retardo_acceso_bloq*1000);
	memcpy(buffer_bloque+(tam_bloque*num_bloque),a_escribir,tam_bloque);
	log_info(logger_file_system, "se escribio en el bloq de swap :%u",num_bloque);

}
void poner_bloq_swap_reservado(uint32_t num_bloque) {
	log_info(logger_file_system, "Acceso a bloque swap <%u>", num_bloque);
	int marca_reservado=0;
	if(fs->bloques+(tam_bloque*num_bloque)==NULL){

		log_info(logger_file_system, "puntero swap es nulo");
	}
	void* buffer_bloque=fs->bloques;

	memset(buffer_bloque,marca_reservado,tam_bloque);

}

void* leer_bloque_swap(int puntero){
	uint32_t num_bloq=puntero/tam_bloque;
	void* datos=malloc(tam_bloque);
	void* buffer_bloques=fs->bloques;
	usleep(retardo_acceso_bloq*1000);
	memcpy(datos,buffer_bloques+(tam_bloque*num_bloq),tam_bloque	);
	return datos;


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
t_list* iniciar_proceso(int cant_bloques) {
	t_list *bloq_asignados = list_create();
	asignar_bloques_swap(bloq_asignados, cant_bloques);
	return bloq_asignados;

}

void asignar_bloques_swap(t_list *bloques_asignados, int cant_bloques) {
	for (int i = 0; i < cant_bloques; i++) {
		int index_bloq = buscar_bloq_libre_swap();

		log_warning(logger_file_system," bloque a asignar %i",index_bloq);
		fs->array_swap[index_bloq].libre = false;
		poner_bloq_swap_reservado(index_bloq);//pone 0 en el bloq archivo
		list_add(bloques_asignados, index_bloq);
	}


}

//busco en la fat entrada con valor 0=libre y devuelvo el indice
int buscar_bloq_libre_swap() {
	int num_bloque=0;
	//primero reservado para boot
	for (int i = 0; i < cant_bloq_swap; i++) {
		if (fs->array_swap[i].libre==true) {

			num_bloque = i;
			break;
		}
	}
	return num_bloque;
}

//TODO se marca con "/0" los bloque de swap asignados al proceso

void finalizar_proceso(t_list *lista_liberar) {

	for (int indice_a_borrar = 0; indice_a_borrar < list_size(lista_liberar);indice_a_borrar++) {

		int indice_bloque = list_get(lista_liberar, indice_a_borrar);
		fs->array_swap[indice_bloque].libre = true;

	}
}

void levantar_fat() {

	int file_descrip_bloques = open(ruta_fat, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (file_descrip_bloques == -1) {
		perror("Error al abrir el archivo FAT");

	}

	int tamanio =abs(cant_total_bloq - cant_bloq_swap) * sizeof(uint32_t);
	log_info(logger_file_system, "Levantado archivo fat: %i bytes",	tamanio);

	// Establecer el tamaño del archivo si es necesario
	if (ftruncate(file_descrip_bloques, tamanio) == -1) {
		perror("Error al establecer el tamaño del archivo FAT");
		close(file_descrip_bloques);
		exit(EXIT_FAILURE);
	}
	uint32_t *bloq = NULL;
	bloq = mmap(NULL, tamanio, PROT_READ | PROT_WRITE, MAP_SHARED,	file_descrip_bloques, 0);

	if (bloq != MAP_FAILED) {
		log_info(logger_file_system,"El archivo FAT se ha mapeado correctamente en la memoria.");
		if(bloq[0]==UINT32_MAX){
			fs->fat->entradas=bloq;
		}else{
			fs->fat->entradas=bloq;
			//reservo entrada inicial para boot
			fs->fat->entradas[0] = RESERV_BOOT		;

			//inicializo las entradas en 0=libre
			for (uint32_t i = 1; i < fs->fat->tamanio_fat; i++) {
				fs->fat->entradas[i] = 0;
			}

		}

	} else {
		perror("Error al mapear el archivo FAT");
		close(file_descrip_bloques);

	}

	// Cerrar el archivo después de asignar el mapeo
	close(file_descrip_bloques);

}
void levantar_archivo_bloques() {
	int file_descrip_bloques = open(ruta_bloques, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (file_descrip_bloques == -1) {
		perror("Error al abrir el archivo de bloques");
		exit(EXIT_FAILURE);
	}
	int tamanio = tam_bloque*cant_total_bloq;
	log_info(logger_file_system, "Levantado archivo_bloques: %i bytes",tamanio);

	// Establecer el tamaño del archivo si es necesario
	if (ftruncate(file_descrip_bloques, tamanio) == -1) {
		perror("Error al establecer el tamaño del archivo de bloques");
		close(file_descrip_bloques);
		exit(EXIT_FAILURE);
	}

	void *bloq = mmap(NULL, tamanio, PROT_READ | PROT_WRITE, MAP_SHARED,file_descrip_bloques, 0);

	if (bloq != MAP_FAILED) {
		log_info(logger_file_system,
				"El archivo bloques se ha mapeado correctamente en la memoria.");

		// Asignar el puntero a bloques de fs
		fs->bloques = bloq;
	} else {
		perror("Error al mapear el archivo de bloques");
		close(file_descrip_bloques);
		exit(EXIT_FAILURE);
	}

	// Cerrar el archivo después de asignar el mapeo
	close(file_descrip_bloques);
}
void levantar_fcbs() {
	DIR *directorio_archivos = opendir(ruta_fcbs);
	struct dirent *fcb;

	if (directorio_archivos == NULL) {
		log_error(logger, "No se pudo abrir el directorio de fcbs");
		exit(1);
	}

	while ((fcb = readdir(directorio_archivos)) != NULL) {
		if (strcmp(fcb->d_name, ".") == 0 || strcmp(fcb->d_name, "..") == 0) {
			continue;
		}
		char *ruta_fcb = malloc(strlen(ruta_fcbs) + strlen(fcb->d_name) + 1);
		strcpy(ruta_fcb, ruta_fcbs);
		strcat(ruta_fcb, fcb->d_name);
		t_config *config_fcb = cargar_config(ruta_fcb);
		t_fcb *fcb_datos = malloc(sizeof(t_fcb));

		char *nom = config_get_string_value(config_fcb, "NOMBRE_ARCHIVO");
		fcb_datos->nombre_archivo = malloc(strlen(nom) + 1);
		strcpy(fcb_datos->nombre_archivo, nom);
		fcb_datos->bloq_inicial_archivo = config_get_int_value(config_fcb,
				"BLOQUE_INICIAL");
		fcb_datos->tamanio_archivo = config_get_int_value(config_fcb,
				"TAMANIO_ARCHIVO");
		list_add(fs->fcb_list, fcb_datos);

		config_destroy(config_fcb);
		free(ruta_fcb);
	}

	closedir(directorio_archivos);
}
