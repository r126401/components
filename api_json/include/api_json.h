/*
 * api_json.h
 *
 *  Created on: 21 sept. 2020
 *      Author: t126401
 */











#ifndef COMPONENTS_API_JSON_INCLUDE_API_JSON_H_
#define COMPONENTS_API_JSON_INCLUDE_API_JSON_H_


#include "mqtt_client.h"
//#include "configuracion_usuario.h"
#include "esp_err.h"
#include "esp_log.h"
#include "comandos_json.h"
#include "espota.h"
#include "logging.h"


typedef enum MESSAGE_TYPE {

	UNKNOWN_MESSAGE = -1,
	APPLICATION_MESSAGE,
	DEBUG_MESSAGE,
	APPLICATION_DEVICE_MESSAGE
}MESSAGE_TYPE;

typedef enum SPONTANEOUS_TYPE {
    STARTED,
    OP_LOCAL_RELAY,
    OP_REMOTE_RELAY,
    UPGRADE_FIRMWARE_FOTA,
    START_SCHEDULE,
    APPLICATION_COMMAND,
    CHANGE_TEMPERATURE,
    STATUS_REPORT,
    END_SCHEDULE,
    ALARM_REPORT,
    CAMBIO_UMBRAL_TEMPERATURA,
	CAMBIO_ESTADO_APLICACION,
    ERROR_REPORT,
	START_UPGRADE_OTA,
	END_UPGRADE_OTA,
	ERROR_UPGRADE_OTA,
	DEVICE_RECOVERED
}SPONTANEOUS_TYPE;


/**
 * @brief Conjunto de comandos que es capaz de aceptar el dispositivo.
 * Con la migracion a idf han quedado obsoletos algunos comandos corresondientes a funciones
 * especificar que habia que implementar.
 */
typedef enum COMMAND_TYPE { NO_JSON = -4,
                NO_IMPLEMENTED = -3,
                UNKNOWN_COMMAND = -2,
                SPONTANEOUS_REPORT = -1,
                DISPLAY_APP_CONF, //ok
                DISPLAY_WIFI_CONF, //ok
                DISPLAY_MQTT_CONF, //ok
                DISPLAY_NTP_CONF,//ok
                SYNCRHONIZE_NTP,//ok
                MODIFY_NTP_SERVER, //ok
                DISPLAY_SCHEDULES,//ok
                INSERT_SCHEDULE, //ok
                MODIFY_SCHEDULE,//ok
                DELETE_SCHEDULE,//ok
                RESET, //ok
                FACTORY_RESET, // OK
                MODIFY_APP_CONF,
                MODIFY_WIFI_CONF, //ok
                MODIFY_MQTT_CONF, //ok
                MODIFY_NTP_CONF, // ok
                TEST,
                STATUS_APP,
				APP_DATA,
                GRABAR_CONFIGURACION,//ok
                BACKUP_CONFIGURACION,//ok
                CARGAR_BACKUP,//ok
                SELECCIONAR_NTP_SERVER_ACTIVO, // ok
                PONER_EN_HORA, // OK
                SUBIR_UMBRAL,
                BAJAR_UMBRAL,
                UPGRADE_FIRMWARE_OTA,// 26
                DISPLAY_ACTIVE_ALARMS
}COMMAND_TYPE;

/**
 * @brief Implementacion de los codigos de respuesta que e enviaran al usuario de la aplicacion.
 */
typedef enum ANSWER_CODE {
    RESP_OK = 200,
    RESP_NOK = 400,
	RESP_RESTART = 100
}ANSWER_CODE;


/**
 * @brief Funcion que enlaza con el modulo mqtt y al que es llamada cuando se recibe un mensaje a procesar.
 * la funcion llamara a analizar_comando para procesar la peticion desde la aplicacion de usuarios.
 * @param datosApp es la estructura de la aplicacion.
 */
 void message_application_received(DATOS_APLICACION *datosApp, char *topic);


 /**
  * @brief analizar_comando: Es llamada desde mensaje_recibido y recibe la peticion del usuario.
  * La funcion analiza el tipo de comando y llama a la funcion correspondiente. Si el comando es > 50 llamara a la
  * funcion especifica de la aplicacion del dispositivo ya que el procesamiento es solo para la clase de dispositivo
  * concreta.
  * @param datosApp es la estructura de la aplicacion.
  * @param peticion. contiene el texto de la peticion.
  * @return respuesta. Se devuelve el texto a la funcion mensaje recibido quien enviará la respuesta procesada
  * al usuario.
  */
 cJSON* IRAM_ATTR analizar_comando(DATOS_APLICACION *datosApp, char* peticion);
 /**
  * @brief cabeceraRespuestaComando. Funcion que construye la cabecera comun para todas las respuestas de comandos.
  * @brief datosApp es la estructura de la aplicacion.
  * @brief peticion. Es la peticion del usuario.
  * @return. Devuelve el objeto json con los elementos cabecera que se van a devolver del comando pedido.
  */
 cJSON *cabeceraRespuestaComando(DATOS_APLICACION *datosApp, cJSON *peticion);
 /**
  * @brief cabeceraGeneral. Funcion que integra todos los parametros de los mensajes al usuario, sean respuesta a comandos
  * o sean informes espontaneos.
  * @param datosApp es la estructura de la aplicacion.
  * return Devuelve el objeto Json de la respuesta completa en la parte de cabecera.
  */
 cJSON* cabeceraGeneral(DATOS_APLICACION *datosApp);
 /**
  * @brief codigo_respuesta implementa los elementos json del resultado ok o no ok de la respuesta.
  * @param respueta. Objeto json con toda la respuesta.
  * @param codigo. Es el codigo de respuesta utilizado para insertar los elementos.
  *
  */
 void codigoRespuesta(cJSON *respuesta, enum ANSWER_CODE codigo);
 /**
  * @brief contenido estandard de la respuesta que se envia al usuario cuando el comando es erroneo o desconocido.
  * @param datosApp es la estructura de la aplicacion.
  * @param respuesta es el contenido de la respuesta al usuario.
  */
 esp_err_t visualizar_comando_desconocido(DATOS_APLICACION *datosApp, cJSON *respuesta);
 /**
   * @fn esp_err_t escribir_programa_actual(DATOS_APLICACION*, cJSON*)
  * @brief Esta funcion escribe en la respuesta el programa activo
  *
  * @pre
  * @post
  * @param datosApp
  * @param respuesta
  * @return
  */
 esp_err_t escribir_programa_actual(DATOS_APLICACION *datosApp, cJSON *respuesta);




 esp_err_t 	extraer_dato_float(cJSON *nodo, char *nombre_campo, float *dato);
 /**
   * @fn esp_err_t extraer_dato_string(cJSON*, char*, char*)
  * @brief Funcion de ayuda para extraer datos string del json
  *
  * @pre
  * @post
  * @param nodo
  * @param nombre_campo
  * @param dato
  * @return
  */
 esp_err_t extraer_dato_string(cJSON *nodo, char *nombre_campo, char* dato);
 /**
   * @fn esp_err_t extraer_dato_double(cJSON*, char*, double*)
  * @brief Funcion de ayuda para extraer dato double
  *
  * @pre
  * @post
  * @param nodo
  * @param nombre_campo
  * @param dato es un valor por referencia que extrae el dato
  * @return Devuelve ESP_FAIL si no puede extraer el campo
  */
 esp_err_t extraer_dato_double(cJSON *nodo, char *nombre_campo, double *dato);
 /**
   * @fn esp_err_t extraer_dato_int(cJSON*, char*, int*)
  * @brief Funcion que extrae un dato entero de la estructura json
  *
  * @pre
  * @post
  * @param nodo
  * @param nombre_campo
  * @param dato
  * @return Devuelve ESP_FAIL si no puede extraer el campo
  */
 esp_err_t extraer_dato_int(cJSON *nodo, char* nombre_campo, int *dato);
 /**
   * @fn esp_err_t extraer_dato_uint8(cJSON*, char*, uint8_t*)
  * @brief Funcion para extraer un dato uint8_t
  *
  * @pre
  * @post
  * @param nodo
  * @param nombre_campo
  * @param dato
  * @return Devuelve ESP_FAIL si no puede extraer el campo
  */
 esp_err_t extraer_dato_uint8(cJSON *nodo, char* nombre_campo, uint8_t *dato);
 /**
   * @fn esp_err_t extraer_dato_uint32(cJSON*, char*, uint32_t*)
  * @brief Funcion para extraer un dato uint32_t de la estructura json
  *
  * @pre
  * @post
  * @param nodo
  * @param nombre_campo
  * @param dato
  * @return Devuelve ESP_FAIL si no puede extraer el campo
  */
 esp_err_t extraer_dato_uint32(cJSON *nodo, char* nombre_campo, uint32_t *dato);
 /**
   * @fn int extraer_dato_hex(char*, int, uint8_t)
  * @brief Funcion para extraer un campo que esta en formato hex a entero uint32_t
  *
  * @pre
  * @post
  * @param texto
  * @param desplazamiento
  * @param tamano
  * @return Devuelve ESP_FAIL si no puede extraer el campo
  */
 int extraer_dato_hex(char* texto, int desplazamiento, uint8_t tamano);
 /**
   * @fn uint32_t hex2int(char*)
  * @brief Funcion de apoyo a la anterior que convierte un dato char* que es un hex a int
  *
  * @pre
  * @post
  * @param hex
  * @return Retorna el valor traducido
  */
 uint32_t hex2int(char *hex);
/**
 * @fn int extraer_dato_tm(char*, int, uint8_t)
 * @brief Esta funion extrae un dato de estructura tm
 *
 * @pre
 * @post
 * @param id_programa
 * @param desplazamiento
 * @param tamano
 * @return
 */
 int extraer_dato_tm(char *id_programa, int desplazamiento, uint8_t tamano);
/**
 * @fn esp_err_t visualizar_datos_aplicacion(struct DATOS_APLICACION*, cJSON*)
 * @brief Funcion que devuelve al usuario los parametros de usuario de la aplicacion en formato json
 *
 * @pre
 * @post
 * @param datosApp
 * @param respuesta
 * @return Devuelve ESP_FAIL en caso de error
 */

 /**
   * @fn esp_err_t extraer_dato_bool(cJSON*, char*, bool*)
  * @brief
  *
  * @param nodo
  * @param nombre_campo
  * @param dato
  * @return
  */
 esp_err_t extraer_dato_bool(cJSON *nodo, char *nombre_campo, bool *dato);

 esp_err_t visualizar_datos_aplicacion(struct DATOS_APLICACION *datosApp, cJSON *respuesta);
 /**
   * @fn esp_err_t visualizar_datos_wifi(DATOS_APLICACION*, cJSON*)
  * @brief Esta funcion devuelve la configuracion wifi del dispositivo
  *
  * @pre
  * @post
  * @param datosApp
  * @param respuesta
  * @return
  */
 esp_err_t visualizar_datos_wifi(DATOS_APLICACION *datosApp, cJSON *respuesta);
 /**
   * @fn esp_err_t visualizar_programas(struct DATOS_APLICACION*, cJSON*)
  * @brief Esta funcion visualiza la lista de programas del dispositivo
  *
  * @pre
  * @post
  * @param datosApp
  * @param respuesta
  * @return
  */
 esp_err_t visualizar_programas(struct DATOS_APLICACION *datosApp, cJSON *respuesta);
 /**
   * @fn esp_err_t visualizar_configuracion_mqtt(DATOS_APLICACION*, cJSON*)
  * @brief Esta funcion visualiza la configuracion mqtt almacenada en el dispositivo
  *
  * @pre
  * @post
  * @param datosApp
  * @param respuesta
  * @return
  */
 esp_err_t visualizar_configuracion_mqtt(DATOS_APLICACION *datosApp, cJSON *respuesta);
 /**
   * @fn esp_err_t crear_programas_json(DATOS_APLICACION*, cJSON*)
  * @brief Funcion que crea la configuracion json a partir de la estructura TIME_PROGRAM
  *
  * @pre
  * @post
  * @param datosApp
  * @param respuesta
  * @return
  */
 esp_err_t crear_programas_json(DATOS_APLICACION *datosApp, cJSON *respuesta);
 /**
   * @fn esp_err_t insertar_nuevo_programa(cJSON*, DATOS_APLICACION*, cJSON*)
  * @brief Esta funcion recibe la peticion de crear un nuevo programa y la inserta en la estructura TIME_PROGRAM
  * guardandola ademas en la configuracion del dispositivo.
  *
  * @pre
  * @post
  * @param peticion
  * @param datosApp
  * @param respuesta
  * @return
  */
 esp_err_t insertar_nuevo_programa(cJSON *peticion,DATOS_APLICACION *datosApp, cJSON *respuesta);
 /**
   * @fn esp_err_t borrar_programa(cJSON*, struct DATOS_APLICACION*, cJSON*)
  * @brief Elimina un programa del dispositivo.
  *
  * @pre
  * @post
  * @param peticion
  * @param datosApp
  * @param respuesta
  * @return
  */
 esp_err_t borrar_programa(cJSON *peticion,struct DATOS_APLICACION *datosApp, cJSON *respuesta);
 /**
   * @fn esp_err_t modificar_programa(cJSON*, struct DATOS_APLICACION*, cJSON*)
  * @brief Modifica un programa del dispositivo
  *
  * @pre
  * @post
  * @param peticion
  * @param datosApp
  * @param respuesta
  * @return
  */
 esp_err_t modificar_programa(cJSON *peticion,struct DATOS_APLICACION *datosApp, cJSON *respuesta);
 /**
   * @fn cJSON cabecera_espontaneo*(DATOS_APLICACION*, enum TIPO_INFORME)
  * @brief Crea la cabecera especifica para el envio de un mensaje espontaneo desde el dispositivo a la aplicacion
  *
  * @pre
  * @post
  * @param datosApp
  * @param tipo_report
  * @return
  */
 cJSON* cabecera_espontaneo(DATOS_APLICACION *datosApp, enum SPONTANEOUS_TYPE tipo_report);
 /**
   * @fn esp_err_t ejecutar_reset(DATOS_APLICACION*, cJSON*)
  * @brief Funcion que ejecuta reset en el dispositivo
  *
  * @pre
  * @post
  * @param datosApp
  * @param respuesta
  * @return
  */
 esp_err_t ejecutar_reset(DATOS_APLICACION *datosApp, cJSON *respuesta);
 /**
   * @fn esp_err_t ejecutar_factory_reset(DATOS_APLICACION*, cJSON*)
  * @brief funcion que ejecuta factory reset
  *
  * @pre
  * @post
  * @param datosApp
  * @param respuesta
  * @return
  */
 esp_err_t ejecutar_factory_reset(DATOS_APLICACION *datosApp, cJSON *respuesta);
 /**
   * @fn esp_err_t upgrade_ota(cJSON*, struct DATOS_APLICACION*, cJSON*)
  * @brief invoca el upgrade del dispositivo via ota.
  *
  * @pre
  * @post
  * @param peticion
  * @param datosApp
  * @param respuesta
  * @return
  */
 esp_err_t upgrade_ota(cJSON *peticion, struct DATOS_APLICACION *datosApp, cJSON *respuesta);


 /**
   * @fn esp_err_t reinicio_fabrica(DATOS_APLICACION*)
  * @brief Esta funcion realiza las acciones para limpiar del dispositivo todas las configuraciones anteriores.
  *
  * @pre
  * @post
  * @param datosApp es la estructura de la aplicacion
  * @return ESP_OK si la accion se realiza con exito.
  */
 esp_err_t reinicio_fabrica(DATOS_APLICACION *datosApp);
 esp_err_t modificar_configuracion_app(cJSON *peticion,struct DATOS_APLICACION *datosApp, cJSON *respuesta);
 esp_err_t notificar_evento_ota(DATOS_APLICACION *datosApp, ESTADO_OTA codeOta);
 esp_err_t notificar_evento_alarma(DATOS_APLICACION *datosApp, int tipo_alarma, char* mnemonico_alarma);
 esp_err_t visualizar_alarmas_activas(DATOS_APLICACION *datosApp, cJSON *respuesta);
 void tarea_upgrade_firmware(DATOS_APLICACION *datosApp);
 void upgrade_ota_esp8266(DATOS_APLICACION *datosApp);
 char* report_2_mnemonic(SPONTANEOUS_TYPE report);
 esp_err_t send_spontaneous_report(DATOS_APLICACION *datosApp, enum SPONTANEOUS_TYPE tipoInforme);
 esp_err_t create_header_report(DATOS_APLICACION *datosApp, cJSON *message, SPONTANEOUS_TYPE spontaneous_type);

#endif /* COMPONENTS_API_JSON_INCLUDE_API_JSON_H_ */
