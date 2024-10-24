/*
 * configuracion.c
 *
 *  Created on: 10 sept. 2020
 *      Author: t126401
 */


#include <errores_proyecto.h>
#include "dialogos_json.h"
#include "user_interface.h"
#include "esp_log.h"
#include "esp_err.h"
//#include "configuracion_usuario.h"
#include "conexiones.h"
#include "cJSON.h"
#include "nvs_flash.h"
#include "nvslib.h"
#include "esp_wifi.h"
#include "configuracion.h"
#include "programmer.h"
#include "api_json.h"
#include "events_device.h"
#include "esp_app_format.h"
#include "esp_ota_ops.h"
#include "applib.h"


#include "esp_netif.h"

static const char *TAG = "configuracion.c";



esp_err_t crear_programas_defecto(DATOS_APLICACION *datosApp) {

	cJSON *array;
	char* texto = NULL;
	int longitud;
	ESP_LOGI(TAG, ""TRAZAR"CREAMOS EL ARRAY DE EJEMPLO", INFOTRAZA);
	array = cJSON_CreateArray();
	appuser_load_default_schedules(datosApp, array);

	texto = cJSON_Print(array);
	if (texto != NULL) {
		longitud = strlen(texto);
		ESP_LOGI(TAG, ""TRAZAR"%s\n longitud %d",INFOTRAZA, texto, longitud);
		escribir_dato_string_nvs(&datosApp->handle, CONFIG_CLAVE_PROGRAMACION, texto);
		cJSON_Delete(array);
		free(texto);

	} else {
		ESP_LOGW(TAG, ""TRAZAR" NO HAY PROGRAMAS", INFOTRAZA);
	}

	return ESP_OK;
}


esp_err_t configuracion_a_json(DATOS_APLICACION *datosApp, cJSON *conf) {


	int i;
	cJSON *array_topics = NULL;
	cJSON *item;


	ESP_LOGI(TAG, ""TRAZAR"(COMENZAMOS)", INFOTRAZA);
	cJSON_AddNumberToObject(conf, DEVICE , datosApp->datosGenerales->tipoDispositivo);
	cJSON_AddStringToObject(conf, MQTT_BROKER, datosApp->datosGenerales->parametrosMqtt.broker);
	cJSON_AddNumberToObject(conf, MQTT_PORT, datosApp->datosGenerales->parametrosMqtt.port);
	cJSON_AddStringToObject(conf, MQTT_USER, datosApp->datosGenerales->parametrosMqtt.user);
	cJSON_AddStringToObject(conf, MQTT_PASS, datosApp->datosGenerales->parametrosMqtt.password);
	cJSON_AddStringToObject(conf, MQTT_SUBSCRIBE, datosApp->datosGenerales->parametrosMqtt.subscribe);
	cJSON_AddStringToObject(conf, MQTT_PUBLISH, datosApp->datosGenerales->parametrosMqtt.publish);
	cJSON_AddNumberToObject(conf, DEVICE_STATUS, (uint8_t) datosApp->datosGenerales->status);

	for (i=0;i<CONFIG_NUM_TOPICS;i++) {
		if (i == 0) {
			array_topics = cJSON_CreateArray();
		}
		cJSON_AddItemToArray(array_topics, item = cJSON_CreateObject());
		cJSON_AddStringToObject(item, MQTT_PUBLISH, datosApp->datosGenerales->parametrosMqtt.topics[i].publish);
		cJSON_AddStringToObject(item, MQTT_SUBSCRIBE, datosApp->datosGenerales->parametrosMqtt.topics[i].subscribe);
		cJSON_AddBoolToObject(item, MQTT_STATUS_TOPIC, datosApp->datosGenerales->parametrosMqtt.topics[i].status);

	}
	cJSON_AddItemToObject(conf, MQTT_TOPICS , array_topics);
	cJSON_AddNumberToObject(conf, MQTT_QOS , datosApp->datosGenerales->parametrosMqtt.qos);
	cJSON_AddNumberToObject(conf, PROGRAM_STATE, datosApp->datosGenerales->estadoProgramacion);
	cJSON_AddStringToObject(conf, OTA_SW_VERSION, datosApp->datosGenerales->ota.swVersion->version);
	cJSON_AddBoolToObject(conf, MQTT_TLS, datosApp->datosGenerales->parametrosMqtt.tls);
	cJSON_AddBoolToObject(conf, USING_MQTT, datosApp->mqtt);
	cJSON_AddBoolToObject(conf, USING_WIFI, datosApp->wifi);
	cJSON_AddBoolToObject(conf, USING_NTP, datosApp->timing);
	cJSON_AddBoolToObject(conf, USING_SCHEDULES, datosApp->schedules);
    appuser_set_configuration_to_json(datosApp, conf);

    	ESP_LOGI(TAG, ""TRAZAR"JSON creado:", INFOTRAZA);
    	return JSON_OK;


}





/**
 * se guardaran las configuraciones en las claves que se indiquen
 */
esp_err_t guardar_configuracion(DATOS_APLICACION *datosApp, char * clave, char* configuracion) {

	esp_err_t error;

	error = escribir_dato_string_nvs(&datosApp->handle, clave, configuracion);
	if (error != ESP_OK) {
		ESP_LOGE(TAG, ""TRAZAR"Error %d al escribir el dato con clave %s", INFOTRAZA, error, clave);
	} else {
		ESP_LOGI(TAG, ""TRAZAR"Configuracion guardada", INFOTRAZA);
	}

	return error;
}


esp_err_t get_default_topics_config(DATOS_APLICACION *datosApp) {

	int i;
	char *mac;

	mac = get_my_id();

    strcpy(datosApp->datosGenerales->parametrosMqtt.publish, "/pub_");
    strcat(datosApp->datosGenerales->parametrosMqtt.publish, mac);
    strcpy(datosApp->datosGenerales->parametrosMqtt.subscribe, "/sub_");
    strcat(datosApp->datosGenerales->parametrosMqtt.subscribe, mac);


	strcpy(datosApp->datosGenerales->parametrosMqtt.topics[0].publish, "/pub_");
	strcat(datosApp->datosGenerales->parametrosMqtt.topics[0].publish, mac);
	strcpy(datosApp->datosGenerales->parametrosMqtt.topics[0].subscribe, "/sub_");
	strcat(datosApp->datosGenerales->parametrosMqtt.topics[0].subscribe, mac);
	datosApp->datosGenerales->parametrosMqtt.topics[0].status = true;


	strcpy(datosApp->datosGenerales->parametrosMqtt.topics[1].publish, "/pub_debug_");
	strcat(datosApp->datosGenerales->parametrosMqtt.topics[1].publish, mac);
	strcpy(datosApp->datosGenerales->parametrosMqtt.topics[1].subscribe, "/sub_debug_");
	strcat(datosApp->datosGenerales->parametrosMqtt.topics[1].subscribe, mac);
	datosApp->datosGenerales->parametrosMqtt.topics[1].status = false;

	for (i=2;i<CONFIG_NUM_TOPICS;i++) {
		strcpy(datosApp->datosGenerales->parametrosMqtt.topics[i].publish, "/sub_");
		strcpy(datosApp->datosGenerales->parametrosMqtt.topics[i].subscribe, "/pub_");
		datosApp->datosGenerales->parametrosMqtt.topics[i].status = false;
	}








	return ESP_OK;

}



/**
 * Esta funcion cargara valores de fabrica en la aplicacion.
 */
esp_err_t cargar_configuracion_defecto(DATOS_APLICACION *datosApp) {

	esp_err_t error;
	ESP_LOGI(TAG, ""TRAZAR"Se cargan parametros comunes de defecto", INFOTRAZA);
#ifdef CONFIG_IDF_TARGET_ESP8266
	tcpip_adapter_init();
	esp_netif_init();
	esp_event_loop_create_default();
#endif

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    strcpy(datosApp->datosGenerales->parametrosMqtt.broker, CONFIG_BROKER_DEFECTO);
    datosApp->datosGenerales->tipoDispositivo = CONFIG_TIPO_DISPOSITIVO;
    datosApp->datosGenerales->parametrosMqtt.port = CONFIG_PUERTO_DEFECTO;
    strcpy(datosApp->datosGenerales->parametrosMqtt.user, CONFIG_USUARIO_BROKER_DEFECTO);
    strcpy(datosApp->datosGenerales->parametrosMqtt.password, CONFIG_PASSWORD_BROKER_DEFECTO);
    ESP_LOGE(TAG, ""TRAZAR"Se cargan parametros comunes de defecto antes", INFOTRAZA);
    get_default_topics_config(datosApp);
    ESP_LOGE(TAG, ""TRAZAR"Se cargan parametros comunes de defecto despues", INFOTRAZA);

    datosApp->datosGenerales->parametrosMqtt.qos = 0;
#ifdef CONFIG_MQTT_TLS
    set_mqtt_tls(datosApp, CONFIG_MQTT_TLS);
    if (get_mqtt_tls(datosApp)) {
    	set_default_certificate(datosApp);
    }
#endif

    ESP_LOGI(TAG, ""TRAZAR"PARAMETROS CARGADOS EN DATOSAPP", INFOTRAZA);
    datosApp->datosGenerales->estadoProgramacion = INVALID_PROG;
    datosApp->datosGenerales->nProgramacion=0;
    datosApp->datosGenerales->nProgramaCandidato = -1;
    datosApp->datosGenerales->programacion = NULL;
    datosApp->datosGenerales->status = DEVICE_NOT_CONFIGURED;
    config_default_services_device(datosApp);
    appuser_set_default_config(datosApp);
	ESP_LOGI(TAG, ""TRAZAR" SALVAMOS LA CONFIGURACION GENERAL A NVS...", INFOTRAZA);
	error = salvar_configuracion_general(datosApp);
	if (error != ESP_OK) {
		ESP_LOGE(TAG, ""TRAZAR"ERROR AL SALVAR LA CONFIGURACION", INFOTRAZA);
		return ERROR_REPORT;
	}
/*
    if (using_schedules(datosApp)) {

    	crear_programas_defecto(datosApp);
    }
	*/
	return ESP_OK;

}

esp_err_t json_a_datos_aplicacion(DATOS_APLICACION *datosApp, char *datos) {

	cJSON *nodo;
	cJSON *array_topics;
	cJSON *object;
	int i;
	int size;

	nodo = cJSON_Parse(datos);
	if (nodo != NULL) {

		ESP_LOGI(TAG, ""TRAZAR"La configuracion es :%s", INFOTRAZA, datos);
		extraer_dato_string(nodo, MQTT_BROKER, datosApp->datosGenerales->parametrosMqtt.broker);
		extraer_dato_int(nodo, MQTT_PORT, &datosApp->datosGenerales->parametrosMqtt.port);
		extraer_dato_string(nodo, MQTT_USER, datosApp->datosGenerales->parametrosMqtt.user);
		extraer_dato_string(nodo, MQTT_PASS, datosApp->datosGenerales->parametrosMqtt.password);
		extraer_dato_string(nodo, MQTT_PUBLISH, datosApp->datosGenerales->parametrosMqtt.publish);
		extraer_dato_string(nodo, MQTT_SUBSCRIBE, datosApp->datosGenerales->parametrosMqtt.subscribe);
		extraer_dato_int(nodo, MQTT_QOS, &datosApp->datosGenerales->parametrosMqtt.qos);
		extraer_dato_int(nodo, PROGRAM_STATE, (int*) &datosApp->datosGenerales->estadoProgramacion);
		extraer_dato_int(nodo, DEVICE, &datosApp->datosGenerales->tipoDispositivo );
		extraer_dato_uint8(nodo, MQTT_TLS, (uint8_t*) &datosApp->datosGenerales->parametrosMqtt.tls);
		extraer_dato_uint8(nodo, DEVICE_STATUS, (uint8_t*) &datosApp->datosGenerales->status);
		extraer_dato_bool(nodo, USING_MQTT, &datosApp->mqtt);
		extraer_dato_bool(nodo, USING_WIFI, &datosApp->wifi);
		extraer_dato_bool(nodo, USING_NTP, &datosApp->timing);
		extraer_dato_bool(nodo, USING_SCHEDULES, &datosApp->schedules);


		array_topics = cJSON_GetObjectItem(nodo, MQTT_TOPICS);

		size = cJSON_GetArraySize(array_topics);
		if (array_topics != NULL) {
			for (i=0;i<size;i++) {
				object = cJSON_GetArrayItem(array_topics, i);
				extraer_dato_string(object, MQTT_PUBLISH, datosApp->datosGenerales->parametrosMqtt.topics[i].publish);
				extraer_dato_string(object, MQTT_SUBSCRIBE, datosApp->datosGenerales->parametrosMqtt.topics[i].subscribe);
				extraer_dato_bool(object, MQTT_STATUS_TOPIC, (bool*) &datosApp->datosGenerales->parametrosMqtt.topics[i].status);
				ESP_LOGE(TAG, ""TRAZAR"publish: %s, subscribe: %s, estado: %d", INFOTRAZA,
						datosApp->datosGenerales->parametrosMqtt.topics[i].publish,
						datosApp->datosGenerales->parametrosMqtt.topics[i].subscribe,
						datosApp->datosGenerales->parametrosMqtt.topics[i].status);

			}
		}

		appuser_json_to_configuration(datosApp, nodo);
	} else {
		ESP_LOGE(TAG, ""TRAZAR"Error al extraer los datos del json", INFOTRAZA);
		return ESP_FAIL;
	}


	cJSON_Delete(nodo);
	ESP_LOGI(TAG, ""TRAZAR"Datos cargados en la estructura datosApp", INFOTRAZA);
	return ESP_OK;


}



/**
 * Se cargaran las configuraciones que se indiquen
 */
esp_err_t leer_configuracion(DATOS_APLICACION *datosApp, char* clave, char* valor) {

	size_t longitud = 2000;
	esp_err_t error;
	ESP_LOGI(TAG, ""TRAZAR"Se lee la configuracion con clave %s", INFOTRAZA, clave);
	if ((error = leer_dato_string_nvs(&datosApp->handle, clave, valor, longitud)) != ESP_OK) {
		ESP_LOGE(TAG, ""TRAZAR"Error al leer la configuracion con clave %s", INFOTRAZA, clave);
	}

	return error;
}




/**
 * @brief Rutina principal que trata la configuracion de arranque del dispositivo.
 * Inicializa la libreria nvs
 * Carga la configuracion datosApp desde nvs o la crea por defecto si no existe
 * Devuelve el estado de la inicializacion.
 *
 */
esp_err_t init_global_parameters(DATOS_APLICACION *datosApp) {


	esp_err_t error;
	char *datos;


#ifndef CONFIG_IDF_TARGET_ESP8266
    datosApp->datosGenerales->ota.swVersion = esp_app_get_description();
#else
    datosApp->datosGenerales->ota.swVersion = esp_ota_get_app_description();
#endif




    ESP_LOGI(TAG, ""TRAZAR"Version de la aplicacion: %s", INFOTRAZA, datosApp->datosGenerales->ota.swVersion->version);

	error = inicializar_nvs(CONFIG_NAMESPACE, &datosApp->handle);
	if (error != ESP_OK) {
		ESP_LOGE(TAG, ""TRAZAR" Error al inicializar nvs", INFOTRAZA);
		return ESP_FAIL;

	}

	datos = calloc(CONFIG_TAMANO_BUFFER_LECTURA, sizeof(char));

#ifdef CONFIG_FACTORY_DATA
		error = cargar_configuracion_defecto(datosApp);
		ESP_LOGI(TAG, ""TRAZAR" Cargada configuracion de defecto", INFOTRAZA);
		salvar_configuracion_general(datosApp);
#else


		// leemos la configuracion general desde nvs
		if ((error = leer_configuracion(datosApp, CONFIG_CLAVE_CONFIGURACION_GENERAL, datos)) == ESP_OK) {
			ESP_LOGI(TAG, ""TRAZAR" Configuracion general leida correctamente",INFOTRAZA);
			// cargamos en la estructura dinamica la configuracion leida
			error = json_a_datos_aplicacion(datosApp, datos);
		} else {
			ESP_LOGW(TAG, ""TRAZAR"La configuracion no se ha cargado. Se carga la de defecto.", INFOTRAZA);
			error = cargar_configuracion_defecto(datosApp);

			if (error != ESP_OK) {
				send_event(__func__,EVENT_ERROR_APP);
				free(datos);
				ESP_LOGE(TAG, ""TRAZAR"Error al cargar la configuracion de defecto!!!!!!!!!!!!!!!!.", INFOTRAZA);
				return error;

			}

		}

		if (get_app_status_device(datosApp) == DEVICE_NOT_CONFIGURED) {
			send_event(__func__ , EVENT_FACTORY);
		}


		if (using_mqtt(datosApp) && get_mqtt_tls(datosApp)) {
			if ((error = leer_configuracion(datosApp, CONFIG_CLAVE_CERTIFICADO_TLS, datos)) == ESP_OK) {
				set_new_certificate(datosApp, datos, 1461);
			} else {
				send_event(__func__, EVENT_ERROR_APP);
				return error;
			}

		}

		if (using_schedules(datosApp) == true) {
			// leemos la configuracion de programas desde nvs
			if ((error = leer_configuracion(datosApp, CONFIG_CLAVE_PROGRAMACION, datos)) == ESP_OK) {
				ESP_LOGI(TAG, ""TRAZAR"Programas leidos desde nvs", INFOTRAZA);
				// cargamos en la estructura de programacion dinamica la configuracion leida
				cargar_programas(datosApp, datos);
			} else {
				ESP_LOGW(TAG, ""TRAZAR"No se ha encontrado programacion en el dispositivo", INFOTRAZA);
				error = ESP_OK;
			}
		} else {
			ESP_LOGW(TAG, ""TRAZAR"No se esta usando la configuracion de schedules", INFOTRAZA);
		}


#endif


	free(datos);
	send_event(__func__,EVENT_DEVICE_READY);
	return ESP_OK;
}

esp_err_t guardar_programas(DATOS_APLICACION *datosApp, char* clave) {

	char* valor;
	esp_err_t error = ESP_OK;;
	cJSON *respuesta = NULL;

	respuesta = cJSON_CreateArray();


	crear_programas_json(datosApp, respuesta);


	valor = cJSON_Print(respuesta);
	error = escribir_dato_string_nvs(&datosApp->handle, clave, valor);
	if (error == ESP_OK) {
		ESP_LOGI(TAG, ""TRAZAR"Programas guardados correctamente\n %s", INFOTRAZA, valor);
	} else {
		ESP_LOGE(TAG, ""TRAZAR"No se han podido guardar los programas en nvs", INFOTRAZA);
	}
	free(valor);
	cJSON_Delete(respuesta);


	return error;

}

esp_err_t cargar_programas(DATOS_APLICACION *datosApp, char *programas) {

	cJSON *array = NULL;
	cJSON *nodo;
	cJSON *item;
	uint8_t elementos = 0;
	int i;
	char *dato;
	char tipo[2];
	TIME_PROGRAM programa_actual;

	array = cJSON_Parse(programas);

	if (array == NULL) {
		ESP_LOGE(TAG, ""TRAZAR"El fichero de programacion nvs no es valido", INFOTRAZA);
		return NO_SCHEDULE;
	}


	elementos = cJSON_GetArraySize(array);

	ESP_LOGI(TAG, ""TRAZAR"El array tiene %d elementos", INFOTRAZA, elementos);
	for(i=0;i<elementos;i++) {
		ESP_LOGI(TAG, ""TRAZAR"PASAMOS POR EL BUCLE %d", INFOTRAZA, datosApp->datosGenerales->nProgramacion);
		nodo = cJSON_GetArrayItem(array, i);
		item = cJSON_GetObjectItem(nodo, PROGRAM_ID);
		dato = cJSON_GetStringValue(item);
		strncpy(tipo, dato, 2);
		programa_actual.tipo = atoi(tipo);
		//cargar_programas_general(datosApp, programas, &programa_actual);
		programa_actual.programacion.tm_hour = extraer_dato_tm(dato, 2, 2);
		programa_actual.programacion.tm_min = extraer_dato_tm(dato,4, 2);
		programa_actual.programacion.tm_sec = extraer_dato_tm(dato, 6, 2);

		switch(programa_actual.tipo) {
		case DIARIA:
			ESP_LOGI(TAG, ""TRAZAR"%s, Programa diario", INFOTRAZA, dato);
			programa_actual.mascara = extraer_dato_hex(dato, 8,2);
			programa_actual.estadoPrograma = extraer_dato_tm(dato, 10, 1);
			//programa_actual.accion = extraer_dato_tm(dato, 11, 1);
			break;
		case SEMANAL:
			ESP_LOGI(TAG, ""TRAZAR"%s, Programa semanal", INFOTRAZA, dato);

			break;
		case FECHADA:
			ESP_LOGI(TAG, ""TRAZAR"%s, Programa fechado", INFOTRAZA, dato);
			programa_actual.programacion.tm_year = extraer_dato_tm(dato, 8, 4);
			programa_actual.programacion.tm_mon = extraer_dato_tm(dato, 12, 2);
			programa_actual.programacion.tm_mday = extraer_dato_tm(dato, 14, 2);
			programa_actual.estadoPrograma = extraer_dato_tm(dato, 16, 1);
			//programa_actual.accion = extraer_datosalvar_configuracion_general
		}
		appuser_load_schedule_extra_data(datosApp, &programa_actual, nodo);

		visualizartiempo(programa_actual.programacion);
		datosApp->datosGenerales->programacion = crearPrograma(&programa_actual, datosApp->datosGenerales->programacion, &datosApp->datosGenerales->nProgramacion);

	}
	datosApp->datosGenerales->nProgramacion = elementos;
	ESP_LOGI(TAG, ""TRAZAR"CONFIGURACION DE PROGRAMAS CARGADA CORRECTAMENTE. PROGRAMACION VALIDA", INFOTRAZA);
	if (datosApp->datosGenerales->estadoProgramacion == INVALID_PROG) {
		datosApp->datosGenerales->estadoProgramacion = VALID_PROG;
	}

	return ESP_OK;
}

esp_err_t salvar_configuracion_general(DATOS_APLICACION *datosApp) {

	esp_err_t error;
	cJSON *configuracion = NULL;
	char* texto_configuracion = NULL;
	ESP_LOGI(TAG, ""TRAZAR"SALVAR CONFIGURACION", INFOTRAZA);
	configuracion = cJSON_CreateObject();
	error = configuracion_a_json(datosApp, configuracion);

	if (error != JSON_OK) {
		ESP_LOGI(TAG, ""TRAZAR" ERROR AL SALVAR LA CONFIGURACION", INFOTRAZA);
	} else {
		ESP_LOGI(TAG, ""TRAZAR" VAMOS A CREAR EL JSON ANTES DE SALVAR", INFOTRAZA);
		if (configuracion == NULL) {
			ESP_LOGE(TAG, ""TRAZAR" la configuracion es nula", INFOTRAZA);
			return ESP_FAIL;
		}
		texto_configuracion = cJSON_Print(configuracion);
		if(texto_configuracion != NULL) {
			error = guardar_configuracion(datosApp, CONFIG_CLAVE_CONFIGURACION_GENERAL, texto_configuracion);
			free(texto_configuracion);
		} else {
			ESP_LOGE(TAG, ""TRAZAR" NO SE HA PODIDO GUARDAR LA CONFIGURACION", INFOTRAZA);
		}
		cJSON_Delete(configuracion);

	}

	if (get_mqtt_tls(datosApp)) {
		guardar_configuracion(datosApp, CONFIG_CLAVE_CERTIFICADO_TLS, get_certificate(datosApp));
	}


	return error;
}

esp_err_t set_upgrade_data(DATOS_APLICACION *datosApp) {

	esp_err_t error;
	char *texto;
	cJSON *ota;
	ota = cJSON_CreateObject();
	ESP_LOGI(TAG, ""TRAZAR"(OTA A JSON)", INFOTRAZA);
	cJSON_AddStringToObject(ota, OTA_SERVER, datosApp->datosGenerales->ota.server);
	cJSON_AddStringToObject(ota, OTA_URL, datosApp->datosGenerales->ota.url);
	cJSON_AddStringToObject(ota, OTA_FILE, datosApp->datosGenerales->ota.file);
	cJSON_AddStringToObject(ota, OTA_SW_VERSION , datosApp->datosGenerales->ota.newVersion);
	cJSON_AddNumberToObject(ota, OTA_PORT, datosApp->datosGenerales->ota.puerto);
	texto = cJSON_Print(ota);
	if (texto != NULL) {
		ESP_LOGI(TAG, ""TRAZAR" UPGRADE FIRMWARE PREPARADO %s", INFOTRAZA, texto);
		//error = escribir_dato_string_nvs(&datosApp->handle, "UPGRADE_FIRMWARE", texto);
		error = guardar_configuracion(datosApp, "UPGRADE", texto);
		if (error != ESP_OK) {
			ESP_LOGE(TAG, ""TRAZAR" UPGRADE FIRMWARE ERROR %d", INFOTRAZA, error);
			return error;
		}

	} else {
		error = ESP_FAIL;
	}
	free(texto);
	cJSON_Delete(ota);



	return error;
}

esp_err_t get_upgrade_data(DATOS_APLICACION *datosApp) {

	esp_err_t error;
	cJSON *nodo;
	char datos[CONFIG_TAMANO_BUFFER_LECTURA];
	error = leer_configuracion(datosApp, "UPGRADE", datos);
	if (error == ESP_OK) {
		nodo = cJSON_Parse(datos);
		extraer_dato_string(nodo, OTA_SERVER, datosApp->datosGenerales->ota.server);
		extraer_dato_string(nodo, OTA_URL, datosApp->datosGenerales->ota.url);
		extraer_dato_string(nodo, OTA_FILE, datosApp->datosGenerales->ota.file);
		extraer_dato_string(nodo, OTA_SW_VERSION, datosApp->datosGenerales->ota.newVersion);
		extraer_dato_int(nodo, OTA_PORT, &datosApp->datosGenerales->ota.puerto);
	}

	return error;
}

esp_err_t is_factory() {


	ESP_LOGI(TAG, ""TRAZAR"(is_factory)", INFOTRAZA);
	wifi_config_t conf_wifi = {0};
#ifdef CONFIG_IDF_TARGET_ESP8266
	tcpip_adapter_init();
#else

#endif
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);
	esp_wifi_get_config(WIFI_IF_STA, &conf_wifi);
	int i;
	for (i=0;i<32;i++) {
		if (conf_wifi.sta.ssid[i] != 0) {
			ESP_LOGW(TAG, ""TRAZAR" WIFI CONFIGURADA %s, %s", INFOTRAZA, (char*) conf_wifi.sta.ssid, (char*) conf_wifi.sta.password);
			return ESP_FAIL;

		}
	}

	ESP_LOGW(TAG, ""TRAZAR" WIFI NO CONFIGURADA", INFOTRAZA);


	return ESP_OK;


}


