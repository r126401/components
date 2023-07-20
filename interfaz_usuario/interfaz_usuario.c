/*
 * interfaz_usuario.c
 *
 *  Created on: 21 sept. 2020
 *      Author: t126401
 */

#include "dialogos_json.h"
#include "interfaz_usuario.h"
#include "configuracion_usuario.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "api_json.h"
#include "driver/gpio.h"
#include "conexiones_mqtt.h"
#include "programmer.h"
#include "conexiones.h"
#include "nvslib.h"
#include "alarmas.h"
#include "funciones_usuario.h"
#include "esp_timer.h"

#include "lv_factory_reset.h"
#include "lv_init_thermostat.h"
#include "lv_thermostat.h"

#define CADENCIA_WIFI 250
#define CADENCIA_BROKER 300
#define CADENCIA_SMARTCONFIG 80
#define CADENCIA_SNTP 1000
#define CADENCIA_ALARMA 1500



static const char *TAG = "INTERFAZ_USUARIO";






void parapadeo_led() {

	static bool luz = false;

	if (luz == false) {
		gpio_set_level(CONFIG_GPIO_PIN_LED, ON);
		luz = true;
	} else {
		gpio_set_level(CONFIG_GPIO_PIN_LED, OFF);
		luz = false;
	}


}

void eliminar_temporizacion(char* nombre) {
/*
	if (temporizador_nuevo != NULL) {
		ESP_LOGW(TAG, ""TRAZAR" elminando temporizador %s ", INFOTRAZA, nombre);
		esp_timer_stop(temporizador_nuevo);
		esp_timer_delete(temporizador_nuevo);
		temporizador_nuevo = NULL;
	}
*/
}

void aplicar_temporizacion(int cadencia, esp_timer_cb_t funcion, char* nombre) {
/*
	esp_timer_create_args_t temporizador;

	ESP_LOGW(TAG, ""TRAZAR" aplicando temporizador %s ", INFOTRAZA, nombre);
	eliminar_temporizacion(nombre);
	temporizador.arg = NULL;
	temporizador.callback = funcion;
	temporizador.name = nombre;

	esp_timer_create(&temporizador, &temporizador_nuevo);
	esp_timer_start_periodic(temporizador_nuevo, cadencia * 1000);
*/

}

esp_err_t appuser_set_default_config(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"Ejecutando configuraciones adicionales de la aplicacion por defecto...", INFOTRAZA);
	datosApp->datosGenerales->tipoDispositivo = INTERRUPTOR;
    //Escribe aqui el codigo de inicializacion por defecto de la aplicacion.
	// Esta funcion es llamada desde el componente configuracion defaultConfig.
	// Aqui puedes establecer los valores por defecto para tu aplicacion.

	return ESP_OK;
}




esp_err_t appuser_set_action_without_schedule_active(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"Ejecutando acciones de programa por defecto", INFOTRAZA);

    return ESP_OK;
}


esp_err_t appuser_notify_smartconfig(DATOS_APLICACION *datosApp) {


	lv_smartconfig_notify(datosApp);

	//aplicar_temporizacion(CADENCIA_SMARTCONFIG, parapadeo_led, "smartconfig");


	return ESP_OK;
}

esp_err_t appuser_notify_application_started(DATOS_APLICACION *datosApp) {

	cJSON *informe;

	informe = appuser_send_spontaneous_report(datosApp, ARRANQUE_APLICACION, NULL);

	ESP_LOGI(TAG, ""TRAZAR" vamos a publicar el arranque del dispositivo", INFOTRAZA);
	if (informe != NULL) {
		publicar_mensaje_json(datosApp, informe, NULL);
		ESP_LOGI(TAG, ""TRAZAR" PUBLICADO", INFOTRAZA);
	}
	datosApp->datosGenerales->estadoApp = NORMAL_AUTO;

	return ESP_OK;
}

esp_err_t appuser_start_ota(DATOS_APLICACION *datosApp) {


	//datosApp->datosGenerales->ota.puerto = 80;
	ESP_LOGI(TAG, ""TRAZAR"PUERTO: %d", INFOTRAZA, datosApp->datosGenerales->ota.puerto);
	//ESP_LOGI(TAG, ""TRAZAR"PUERTO OTRA VEZ: %d", datosApp->datosGenerales->ota.puerto);
	ESP_LOGI(TAG, ""TRAZAR"servidor ota: %s\n, puerto: %d\n, url: %s, version %s", INFOTRAZA,
			datosApp->datosGenerales->ota.server, datosApp->datosGenerales->ota.puerto, datosApp->datosGenerales->ota.url, datosApp->datosGenerales->ota.swVersion->version);


	return ESP_OK;
}

esp_err_t appuser_get_date_sntp() {

	//aplicar_temporizacion(CADENCIA_SNTP, parapadeo_led, "sntp");
	return ESP_OK;
}
esp_err_t appuser_error_get_date_sntp() {

	//aplicar_temporizacion(CADENCIA_SNTP, parapadeo_led, "sntp_error");
	return ESP_OK;
}

esp_err_t appuser_sntp_ok() {
/*
	eliminar_temporizacion("sntp");
	gpio_set_level(CONFIG_GPIO_PIN_LED, ON);
	*/
	return ESP_OK;

}



esp_err_t appuser_notify_connecting_wifi(DATOS_APLICACION *datosApp) {


	ESP_LOGI(TAG, ""TRAZAR" appuser notificando connecting wifi", INFOTRAZA);
	//aplicar_temporizacion(CADENCIA_WIFI, parapadeo_led, "wifi");
	lv_init_data_init_thermostat(datosApp);

	switch (datosApp->datosGenerales->estadoApp) {

	case ARRANQUE_FABRICA:
		datosApp->datosGenerales->estadoApp = NORMAL_ARRANCANDO;
		break;

	case NORMAL_ARRANCANDO:
		lv_connecting_to_wifi_station(datosApp);
		break;
	default:
		break;

	}



	return ESP_OK;
}

esp_err_t appuser_notify_wifi_connected_ok() {


	//eliminar_temporizacion("wifi");
	//os_timer_disarm(&temporizador_general);
	//gpio_set_level(CONFIG_GPIO_PIN_LED, ON);


	return ESP_OK;
}

esp_err_t appuser_notify_connecting_broker_mqtt() {
/*
	ESP_LOGI(TAG, ""TRAZAR"BROKER CONECTANDO", INFOTRAZA);
	aplicar_temporizacion(CADENCIA_BROKER, parapadeo_led, "mqtt");
	*/
	return ESP_OK;
}
esp_err_t appuser_notify_broker_connected_ok() {

	//eliminar_temporizacion("mqtt");
	//gpio_set_level(CONFIG_GPIO_PIN_LED, ON);

	return ESP_OK;
}
esp_err_t appuser_notify_broker_disconnected() {

	//aplicar_temporizacion(CADENCIA_BROKER, parapadeo_led, "mqtt_error");
	return ESP_OK;
}





void appuser_end_schedule(void *arg) {


}

esp_err_t appuser_start_schedule(DATOS_APLICACION *datosApp) {



	return ESP_OK;
}

esp_err_t appuser_notify_local_alarm(DATOS_APLICACION *datosApp, uint8_t indice) {

	if (datosApp->alarmas[indice].estado_alarma == ALARMA_OFF) {
		//eliminar_temporizacion();
		//os_timer_disarm(&temporizador_general);
		ESP_LOGI(TAG, ""TRAZAR" ALARMA OFF NOTIFICADA", INFOTRAZA);
	} else {
		//aplicar_temporizacion(CADENCIA_ALARMA, parapadeo_led);
		/*
		os_timer_disarm(&temporizador_general);
		os_timer_setfn(&temporizador_general, (os_timer_func_t*) parapadeo_led, NULL);
		os_timer_arm(&temporizador_general, CADENCIA_ALARMA, true);
		*/
		ESP_LOGI(TAG, ""TRAZAR" ALARMA ON NOTIFICADA", INFOTRAZA);

	}


	return ESP_OK;
}

cJSON* appuser_send_spontaneous_report(DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoInforme, cJSON *comandoOriginal) {


    cJSON *respuesta = NULL;


    respuesta = cabecera_espontaneo(datosApp, tipoInforme);
    switch(tipoInforme) {
        case ARRANQUE_APLICACION:
            printf("generarReporte--> enviando arranqueAplicacion");
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
            escribir_programa_actual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);
            break;
        case ACTUACION_RELE_LOCAL:
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
            escribir_programa_actual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);

            break;
        case CAMBIO_DE_PROGRAMA:
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
            escribir_programa_actual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);
            break;
        case RELE_TEMPORIZADO:
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
            escribir_programa_actual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);
            break;
        default:
            codigoRespuesta(respuesta, RESP_NOK);
            printf("enviarReporte--> Salida no prevista\n");
            break;
    }

    return respuesta;

}
esp_err_t appuser_load_schedule_extra_data(DATOS_APLICACION *datosApp, TIME_PROGRAM *programa_actual, cJSON *nodo) {

	cJSON *item;
	char* dato;


	item = cJSON_GetObjectItem(nodo, PROGRAM_ID);
	dato = cJSON_GetStringValue(item);
	if (item != NULL) {
		switch(programa_actual->tipo) {
		case DIARIA:
			programa_actual->accion = extraer_dato_tm(dato, 11, 1);
			break;
		case SEMANAL:
			break;
		case FECHADA:
			programa_actual->accion = extraer_dato_tm(dato, 17, 1);
			break;
		}


	}

	item = cJSON_GetObjectItem(nodo, DURATION_PROGRAM);
	if (item != NULL) {
		programa_actual->duracion = item->valueint;
		ESP_LOGI(TAG, ""TRAZAR"DURACION = %ld", INFOTRAZA, programa_actual->duracion);
	} else {
		programa_actual->duracion = 0;
		ESP_LOGI(TAG, ""TRAZAR"NO SE GUARDA DURACION: %ld", INFOTRAZA, programa_actual->duracion);
	}




	return ESP_OK;

}

esp_err_t appuser_set_configuration_to_json(DATOS_APLICACION *datosApp, cJSON *conf) {

	//cJSON_AddNumberToObject(conf, DEVICE , INTERRUPTOR);
	ESP_LOGI(TAG, ""TRAZAR" CONFIGURACION A JSON DEL DISPOSITIVO...", INFOTRAZA);
	return ESP_OK;
}

esp_err_t appuser_json_to_configuration(DATOS_APLICACION *datosApp, cJSON *datos) {

	return ESP_OK;
}

esp_err_t appuser_load_default_schedules(DATOS_APLICACION *datosApp, cJSON *array) {

	cJSON *item = NULL;

	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "000000006700");
	cJSON_AddNumberToObject(item, DURATION_PROGRAM, 420);
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "001200007f10");
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "001555007f11");
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "001038007f11");
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "021715000120081611");

	return ESP_OK;
}

esp_err_t appuser_get_schedule_extra_data(TIME_PROGRAM *programa_actual, cJSON *nodo) {


    if(extraer_dato_int(nodo, DURATION_PROGRAM, (int*) &programa_actual->duracion) != ESP_OK) {
    	programa_actual->duracion = 0;
    }


	return ESP_OK;
}

esp_err_t appuser_modify_schedule_extra_data(TIME_PROGRAM *programa_actual,cJSON *nodo) {

	extraer_dato_uint32(nodo, DURATION_PROGRAM, &programa_actual->duracion);


	return ESP_OK;
}

esp_err_t appuser_reporting_schedule_extra_data(TIME_PROGRAM *programa_actual, cJSON *nodo) {

    if (programa_actual->duracion > 0) {
        cJSON_AddNumberToObject(nodo, DURATION_PROGRAM, programa_actual->duracion);

    }

	return ESP_OK;
}

esp_err_t appuser_modify_local_configuration_application(cJSON *root, DATOS_APLICACION *datosApp, cJSON *respuesta) {

	return ESP_OK;

}

esp_err_t appuser_received_message_extra_subscription(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR" mensaje del topic: %s", INFOTRAZA, datosApp->handle_mqtt->topic);
	return ESP_OK;
}

void nemonicos_alarmas(DATOS_APLICACION *datosApp, int i) {

	switch(i) {
	case 0:
		strncpy(datosApp->alarmas[i].nemonico, "WiFi................", 50);
		break;
	case 1:
		strncpy(datosApp->alarmas[i].nemonico, "Servidor Mqtt.......", 50);
		break;
	case 2:
		strncpy(datosApp->alarmas[i].nemonico, "Reloj...............", 50);
		break;
	case 3:
		strncpy(datosApp->alarmas[i].nemonico, "Nvs.................", 50);
		break;
	case 4:
		strncpy(datosApp->alarmas[i].nemonico, "Sensor local........", 50);
		break;
	case 5:
		strncpy(datosApp->alarmas[i].nemonico, "sensor remoto.......", 50);
		break;
	default:
		strncpy(datosApp->alarmas[i].nemonico, "alarma no registrada", 50);
		break;

	}


}

esp_err_t appuser_notify_app_status(DATOS_APLICACION *datosApp, enum ESTADO_APP estado) {

	return ESP_OK;
}


void appuser_notify_schedule_events(DATOS_APLICACION *datosApp) {

}


esp_err_t appuser_set_command_application(cJSON *peticion, int nComando, DATOS_APLICACION *datosApp, cJSON *respuesta) {

	return ESP_OK;
}

esp_err_t appuser_notify_error_wifi_connection(DATOS_APLICACION *datosApp) {

	if (datosApp->datosGenerales->estadoApp == NORMAL_ARRANCANDO) {

		ESP_LOGE(TAG, ""TRAZAR"ERROR EN LA CONEXION WIFI EN FASE DE ARRANQUE. ESTADO %d", INFOTRAZA,  datosApp->datosGenerales->estadoApp);
	}


	return ESP_OK;

}

void app_user_notify_scan_done(DATOS_APLICACION *datosApp, wifi_ap_record_t *ap_info, uint16_t *ap_count) {


	ESP_LOGI(TAG, ""TRAZAR"RECIBIDAS %d redes en app", INFOTRAZA, *ap_count);
	lv_create_layout_search_ssid(datosApp, ap_info, ap_count);
	/*
	if (*ap_count == 0) {
		return;
	}
    for (i = 0; i < *ap_count; i++) {
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
        ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
        ESP_LOGI(TAG, "Channel \t\t%d", ap_info[i].primary);
    }
*/

}





