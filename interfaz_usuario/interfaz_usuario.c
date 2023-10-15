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
#include "code_application.h"
#include "configuracion.h"
#include "lv_rgb_main.h"
#include "lvgl.h"

#define CADENCIA_WIFI 250
#define CADENCIA_BROKER 300
#define CADENCIA_SMARTCONFIG 80
#define CADENCIA_SNTP 1000
#define CADENCIA_ALARMA 1500



static const char *TAG = "INTERFAZ_USUARIO";


/*
enum ESTADO_APP change_status_application(DATOS_APLICACION *datosApp) {

	enum ESTADO_APP estado_final;

	switch (datosApp->datosGenerales->estadoApp) {

	case ESPERA_FIN_ARRANQUE:
		if (datosApp->datosGenerales->nProgramacion == 0) {
			estado_final = NORMAL_SIN_PROGRAMACION;
		} else {
			estado_final = NORMAL_AUTO;
		}
	break;
	case STARTING:
		datosApp->datosGenerales->estadoApp = ESPERA_FIN_ARRANQUE;
		estado_final = ESPERA_FIN_ARRANQUE;
		break;


	default:
		estado_final = NORMAL_AUTO;
		break;
	}

	appuser_notify_app_status(datosApp, estado_final);



	return estado_final;
}
*/

esp_err_t appuser_set_default_config(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_set_default_config", INFOTRAZA);
	if (datosApp->datosGenerales->tipoDispositivo == DESCONOCIDO) {
		datosApp->datosGenerales->tipoDispositivo = CRONOTERMOSTATO;
	}

    //Escribe aqui el codigo de inicializacion por defecto de la aplicacion.
	// Esta funcion es llamada desde el componente configuracion defaultConfig.
	// Aqui puedes establecer los valores por defecto para tu aplicacion.

    datosApp->termostato.reintentosLectura = 5;
    datosApp->termostato.intervaloReintentos = 3;
    datosApp->termostato.margenTemperatura = 0.5;
    datosApp->termostato.intervaloLectura = 10;
    datosApp->termostato.tempUmbral = -1000;
    datosApp->termostato.tempUmbralDefecto = 21.5;
    datosApp->termostato.calibrado = -3.5;
    datosApp->termostato.master = true;
    datosApp->termostato.incdec = 0.1;
    memset(datosApp->termostato.sensor_remoto, 0, sizeof(datosApp->termostato.sensor_remoto));


	return ESP_OK;
}




esp_err_t appuser_set_action_without_schedule_active(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_set_action_without_schedule_active", INFOTRAZA);

    return ESP_OK;
}


esp_err_t appuser_notify_smartconfig(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_smartconfig", INFOTRAZA);

	//lv_smartconfig_notify(datosApp);

	//aplicar_temporizacion(CADENCIA_SMARTCONFIG, parapadeo_led, "smartconfig");


	return ESP_OK;
}

esp_err_t appuser_notify_application_started(DATOS_APLICACION *datosApp) {

	cJSON *informe;
	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_application_started. Estado Aplicacion: %d", INFOTRAZA, datosApp->datosGenerales->estadoApp);
	//datosApp->datosGenerales->estadoApp = change_status_application(datosApp);
	//ESP_LOGI(TAG, ""TRAZAR"appuser_notify_application_started. Estado final Aplicacion: %d", INFOTRAZA, datosApp->datosGenerales->estadoApp);

	datosApp->termostato.tempActual = -1000;
	lv_update_temperature(datosApp);

	//lv_screen_thermostat(datosApp);
	informe = appuser_send_spontaneous_report(datosApp, ARRANQUE_APLICACION, NULL);

	ESP_LOGI(TAG, ""TRAZAR" vamos a publicar el arranque del dispositivo", INFOTRAZA);
	if (informe != NULL) {
		publicar_mensaje_json(datosApp, informe, NULL);
		ESP_LOGI(TAG, ""TRAZAR" PUBLICADO", INFOTRAZA);
	}

	return ESP_OK;
}

esp_err_t appuser_start_ota(DATOS_APLICACION *datosApp) {


	ESP_LOGI(TAG, ""TRAZAR"appuser_start_ota", INFOTRAZA);
	//datosApp->datosGenerales->ota.puerto = 80;
	ESP_LOGI(TAG, ""TRAZAR"PUERTO: %d", INFOTRAZA, datosApp->datosGenerales->ota.puerto);
	//ESP_LOGI(TAG, ""TRAZAR"PUERTO OTRA VEZ: %d", datosApp->datosGenerales->ota.puerto);
	ESP_LOGI(TAG, ""TRAZAR"servidor ota: %s\n, puerto: %d\n, url: %s, version %s", INFOTRAZA,
			datosApp->datosGenerales->ota.server, datosApp->datosGenerales->ota.puerto, datosApp->datosGenerales->ota.url, datosApp->datosGenerales->ota.swVersion->version);


	return ESP_OK;
}

esp_err_t appuser_get_date_sntp(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_get_date_sntp", INFOTRAZA);

	return ESP_OK;
}
esp_err_t appuser_error_get_date_sntp(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_error_get_date_sntp", INFOTRAZA);
	lv_update_alarm_device(datosApp);

	return ESP_OK;
}

esp_err_t appuser_sntp_ok(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_sntp_ok", INFOTRAZA);
	lv_update_alarm_device(datosApp);

	return ESP_OK;

}



esp_err_t appuser_notify_connecting_wifi(DATOS_APLICACION *datosApp) {


	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_connecting_wifi", INFOTRAZA);
	//aplicar_temporizacion(CADENCIA_WIFI, parapadeo_led, "wifi");
	//lv_init_thermostat(datosApp);

	switch (datosApp->datosGenerales->estadoApp) {

	case FACTORY:
		datosApp->datosGenerales->estadoApp = STARTING;
		break;

	case STARTING:
		//lv_send_lcd_commands(CONNECTING_WIFI);
		lv_connecting_to_wifi_station(datosApp);
		lv_timer_handler();
		break;
	default:
		break;

	}



	return ESP_OK;
}

esp_err_t appuser_notify_wifi_connected_ok(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_wifi_connected_ok", INFOTRAZA);
	lv_update_alarm_device(datosApp);


	return ESP_OK;
}

esp_err_t appuser_notify_error_wifi_connection(DATOS_APLICACION *datosApp) {

	static uint8_t fail = 0;

	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_error_wifi_connection", INFOTRAZA);
	lv_update_alarm_device(datosApp);

	if (datosApp->datosGenerales->estadoApp == STARTING) {
		lv_update_button_wifi(true);

		fail ++;
		ESP_LOGI(TAG, ""TRAZAR"FAIL VALE %d", INFOTRAZA, fail);
		if (fail == 2) {
			lv_update_button_wifi(false);
		}

		if (fail > 5) {
			lv_update_button_wifi(true);
			fail = 5;
		}
	}



	return ESP_OK;

}



esp_err_t appuser_notify_connecting_broker_mqtt(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_connecting_broker_mqtt", INFOTRAZA);
	return ESP_OK;
}
esp_err_t appuser_notify_broker_connected_ok(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_broker_connected_ok", INFOTRAZA);
	lv_update_alarm_device(datosApp);

	return ESP_OK;
}
esp_err_t appuser_notify_broker_disconnected(DATOS_APLICACION *datosApp) {
	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_broker_disconnected", INFOTRAZA);
	lv_update_alarm_device(datosApp);


	return ESP_OK;
}





void appuser_end_schedule(DATOS_APLICACION *datosApp) {

    cJSON * respuesta = NULL;
    ESP_LOGI(TAG, ""TRAZAR"appuser_end_schedule", INFOTRAZA);
    datosApp->termostato.tempUmbral = datosApp->termostato.tempUmbralDefecto;
    lv_update_temperature(datosApp);
    respuesta = appuser_send_spontaneous_report(datosApp, RELE_TEMPORIZADO, NULL);
    if (respuesta != NULL) {
    	publicar_mensaje_json(datosApp, respuesta, NULL);
    }
    ESP_LOGI(TAG, ""TRAZAR"FIN DE LA TEMPORIZACION. SE PASA A LA TEMPERATURA DE DEFECTO", INFOTRAZA);

}

esp_err_t appuser_start_schedule(DATOS_APLICACION *datosApp) {

	cJSON *respuesta;
	enum ESTADO_RELE accion;

	ESP_LOGI(TAG, ""TRAZAR"appuser_start_schedule", INFOTRAZA);
	datosApp->termostato.tempUmbral = datosApp->datosGenerales->programacion[datosApp->datosGenerales->nProgramaCandidato].temperatura;

    if (calcular_accion_termostato(datosApp, &accion) == ACCIONAR_TERMOSTATO) {
    	operacion_rele(datosApp, TEMPORIZADA, accion);

    }

	respuesta = appuser_send_spontaneous_report(datosApp, CAMBIO_DE_PROGRAMA, NULL);
	if (respuesta != NULL) {
		publicar_mensaje_json(datosApp, respuesta, NULL);
	}
	lv_update_threshold(datosApp);
	// actualizar los intervalos del lcd
	lv_update_bar_schedule(datosApp);



	return ESP_OK;

	return ESP_OK;
}

esp_err_t appuser_notify_error_device(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_local_alarm", INFOTRAZA);
	lv_update_alarm_device(datosApp);


	return ESP_OK;
}


esp_err_t appuser_notify_device_ok(DATOS_APLICACION *datosApp) {
	
	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_local_alarm", INFOTRAZA);
	lv_update_alarm_device(datosApp);


	return ESP_OK;
}


cJSON* appuser_send_spontaneous_report(DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoInforme, cJSON *comandoOriginal) {


    cJSON *respuesta = NULL;

    ESP_LOGI(TAG, ""TRAZAR"appuser_send_spontaneous_report", INFOTRAZA);
    respuesta = cabecera_espontaneo(datosApp, tipoInforme);
    switch(tipoInforme) {
        case ARRANQUE_APLICACION:
        case ACTUACION_RELE_LOCAL:
        case CAMBIO_DE_PROGRAMA:
        case RELE_TEMPORIZADO:
        case CAMBIO_TEMPERATURA:
            cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
            cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
#ifdef CONFIG_DHT22
            cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
#endif
            cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->termostato.tempUmbral);
            cJSON_AddBoolToObject(respuesta, MASTER, datosApp->termostato.master);
            cJSON_AddStringToObject(respuesta, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
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


	ESP_LOGI(TAG, ""TRAZAR"appuser_load_schedule_extra_data", INFOTRAZA);
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

	ESP_LOGI(TAG, ""TRAZAR" VAMOS A CALCULAR LA TEMPERATURA UMBRAL", INFOTRAZA);
	extraer_dato_double(nodo, UMBRAL_TEMPERATURA, &programa_actual->temperatura);
	datosApp->termostato.tempUmbral = programa_actual->temperatura;
	ESP_LOGI(TAG, ""TRAZAR" UMBRAL :%lf", INFOTRAZA, programa_actual->temperatura);
	lv_update_threshold(datosApp);


	return ESP_OK;

}

esp_err_t appuser_set_configuration_to_json(DATOS_APLICACION *datosApp, cJSON *conf) {

	//cJSON_AddNumberToObject(conf, DEVICE , INTERRUPTOR);
	ESP_LOGI(TAG, ""TRAZAR"appuser_set_configuration_to_json", INFOTRAZA);

    cJSON_AddNumberToObject(conf, MARGEN_TEMPERATURA, datosApp->termostato.margenTemperatura);
    cJSON_AddNumberToObject(conf, INTERVALO_LECTURA, datosApp->termostato.intervaloLectura);
    cJSON_AddNumberToObject(conf, INTERVALO_REINTENTOS, datosApp->termostato.intervaloReintentos);
    cJSON_AddNumberToObject(conf, REINTENTOS_LECTURA, datosApp->termostato.reintentosLectura);
    cJSON_AddNumberToObject(conf, CALIBRADO, datosApp->termostato.calibrado);
    cJSON_AddBoolToObject(conf, MASTER, datosApp->termostato.master);
    cJSON_AddStringToObject(conf, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
    cJSON_AddNumberToObject(conf, UMBRAL_DEFECTO, datosApp->termostato.tempUmbralDefecto);
    cJSON_AddNumberToObject(conf, INCDEC, datosApp->termostato.incdec);
	return ESP_OK;
}

esp_err_t appuser_json_to_configuration(DATOS_APLICACION *datosApp, cJSON *datos) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_json_to_configuration", INFOTRAZA);
	extraer_dato_double(datos, MARGEN_TEMPERATURA, &datosApp->termostato.margenTemperatura);
	extraer_dato_uint8(datos, INTERVALO_LECTURA, &datosApp->termostato.intervaloLectura);
	extraer_dato_uint8(datos, INTERVALO_REINTENTOS, &datosApp->termostato.intervaloReintentos);
	extraer_dato_uint8(datos, REINTENTOS_LECTURA, &datosApp->termostato.reintentosLectura);
	extraer_dato_double(datos, CALIBRADO, &datosApp->termostato.calibrado);
	extraer_dato_uint8(datos,  MASTER, (uint8_t*) &datosApp->termostato.master);
	extraer_dato_string(datos, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
	extraer_dato_float(datos,  UMBRAL_DEFECTO, &datosApp->termostato.tempUmbralDefecto);
	extraer_dato_float(datos, INCDEC, &datosApp->termostato.incdec);


	return ESP_OK;
}

esp_err_t appuser_load_default_schedules(DATOS_APLICACION *datosApp, cJSON *array) {

	cJSON *item = NULL;

	ESP_LOGI(TAG, ""TRAZAR"appuser_load_default_schedules", INFOTRAZA);
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "000000007f11");
	cJSON_AddNumberToObject(item, UMBRAL_TEMPERATURA, 21.5);
	cJSON_AddNumberToObject(item, DURATION_PROGRAM, 3600);
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "001200007f11");
	cJSON_AddNumberToObject(item, UMBRAL_TEMPERATURA, 22.5);
	cJSON_AddNumberToObject(item, DURATION_PROGRAM, 3600);
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "001900007f11");
	cJSON_AddNumberToObject(item, UMBRAL_TEMPERATURA, 23.5);
	cJSON_AddNumberToObject(item, DURATION_PROGRAM, 13800);
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "001038007f11");
	cJSON_AddNumberToObject(item, UMBRAL_TEMPERATURA, 24.5);
	cJSON_AddNumberToObject(item, DURATION_PROGRAM, 3600);
	cJSON_AddItemToArray(array, item = cJSON_CreateObject());
	cJSON_AddStringToObject(item, PROGRAM_ID, "021715000120081611");
	cJSON_AddNumberToObject(item, UMBRAL_TEMPERATURA, 25.5);
	cJSON_AddNumberToObject(item, DURATION_PROGRAM, 3600);

	return ESP_OK;
}

esp_err_t appuser_get_schedule_extra_data(TIME_PROGRAM *programa_actual, cJSON *nodo) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_get_schedule_extra_data", INFOTRAZA);
	extraer_dato_double(nodo, UMBRAL_TEMPERATURA, &programa_actual->temperatura);
	extraer_dato_double(nodo, HUMEDAD, &programa_actual->humedad);
    if(extraer_dato_int(nodo, DURATION_PROGRAM, (int*) &programa_actual->duracion) != ESP_OK) {
    	programa_actual->duracion = 0;
    }



	return ESP_OK;
}

esp_err_t appuser_modify_schedule_extra_data(TIME_PROGRAM *programa_actual,cJSON *nodo) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_modify_schedule_extra_data", INFOTRAZA);
	extraer_dato_double(nodo, UMBRAL_TEMPERATURA, &programa_actual->temperatura);
	extraer_dato_double(nodo, HUMEDAD, &programa_actual->humedad);
	extraer_dato_uint32(nodo, DURATION_PROGRAM, &programa_actual->duracion);


	return ESP_OK;
}

esp_err_t appuser_reporting_schedule_extra_data(TIME_PROGRAM *programa_actual, cJSON *nodo) {


	ESP_LOGI(TAG, ""TRAZAR"appuser_reporting_schedule_extra_data", INFOTRAZA);
    cJSON_AddNumberToObject(nodo, UMBRAL_TEMPERATURA, programa_actual->temperatura);
    if (programa_actual->duracion > 0) {
        cJSON_AddNumberToObject(nodo, DURATION_PROGRAM, programa_actual->duracion);

    }

	return ESP_OK;
}

esp_err_t appuser_modify_local_configuration_application(cJSON *root, DATOS_APLICACION *datosApp, cJSON *respuesta) {

	   cJSON *nodo;

	   ESP_LOGI(TAG, ""TRAZAR"appuser_modify_local_configuration_application", INFOTRAZA);
	   nodo = cJSON_GetObjectItem(root, APP_PARAMS);
	   if(nodo == NULL) {
	       codigoRespuesta(respuesta, RESP_NOK);
	       return ESP_FAIL;
	   }

	   extraer_dato_uint8(nodo, MQTT_TLS, (uint8_t*) &datosApp->datosGenerales->parametrosMqtt.tls);
	   if (datosApp->datosGenerales->parametrosMqtt.tls == false) {
		   datosApp->datosGenerales->parametrosMqtt.port = 1883;
		   strcpy(datosApp->datosGenerales->parametrosMqtt.broker, (const char*) "mqtt://jajicaiot.ddns.net");
	   } else {
		   datosApp->datosGenerales->parametrosMqtt.port = 8883;
		  	   strcpy(datosApp->datosGenerales->parametrosMqtt.broker, (const char*) "mqtts://jajicaiot.ddns.net");
	   }
	   extraer_dato_double(nodo, MARGEN_TEMPERATURA, &datosApp->termostato.margenTemperatura);
	   extraer_dato_uint8(nodo, INTERVALO_LECTURA, &datosApp->termostato.intervaloLectura);
	   extraer_dato_uint8(nodo, INTERVALO_REINTENTOS, &datosApp->termostato.intervaloReintentos);
	   extraer_dato_uint8(nodo, REINTENTOS_LECTURA, &datosApp->termostato.reintentosLectura);
	   extraer_dato_double(nodo, CALIBRADO, &datosApp->termostato.calibrado);
	   extraer_dato_float(nodo, UMBRAL_DEFECTO, &datosApp->termostato.tempUmbralDefecto);
	   extraer_dato_float(nodo, INCDEC, &datosApp->termostato.incdec);
	   if ((extraer_dato_uint8(nodo,  MASTER, (uint8_t*) &datosApp->termostato.master)) == ESP_OK) {

		   if (datosApp->termostato.master == false) {

			   extraer_dato_string(nodo,  SENSOR_REMOTO, &datosApp->termostato.sensor_remoto);
			   cJSON_AddStringToObject(respuesta, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
		   } else {
			   strcpy(datosApp->termostato.sensor_remoto, "");
		   }
		   cJSON_AddBoolToObject(respuesta, MASTER, datosApp->termostato.master);




	   }
	   codigoRespuesta(respuesta, RESP_OK);

	   ESP_LOGI(TAG, ""TRAZAR" parametros de configuracion  modificados", INFOTRAZA);
	   return salvar_configuracion_general(datosApp);

}


esp_err_t notify_end_starting(DATOS_APLICACION *datosApp) {

	cJSON *informe;

	ESP_LOGI(TAG, ""TRAZAR"notify_end_starting", INFOTRAZA);
	if (datosApp->datosGenerales->estadoApp == ESPERA_FIN_ARRANQUE) {

		//lv_cargar_pantalla_principal();
		//calcular_estado_aplicacion(datosApp);
		//appuser_cambiar_modo_aplicacion(datosApp, estado_app);
		informe = appuser_send_spontaneous_report(datosApp, ARRANQUE_APLICACION, NULL);

		if (informe != NULL) {
			publicar_mensaje_json(datosApp, informe, NULL);
		}

	}



	return ESP_OK;

}

esp_err_t appuser_received_message_extra_subscription(DATOS_APLICACION *datosApp) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_received_message_extra_subscription", INFOTRAZA);
	ESP_LOGI(TAG, ""TRAZAR" mensaje del topic: %s", INFOTRAZA, datosApp->handle_mqtt->topic);
	cJSON *respuesta;
	char* texto_respuesta;
	double dato;
	char topic[55] = {0};
	//ets_timer_disarm(&temporizador_lectura_remota);
	strncpy(topic, datosApp->handle_mqtt->topic, datosApp->handle_mqtt->topic_len);
	texto_respuesta = (char*) calloc((datosApp->handle_mqtt->data_len + 1), sizeof(char));
	strncpy(texto_respuesta,datosApp->handle_mqtt->data, datosApp->handle_mqtt->data_len);
	respuesta = cJSON_Parse(texto_respuesta);
	if (respuesta != NULL) {
		extraer_dato_double(respuesta, TEMPERATURA, &dato);
		datosApp->termostato.tempActual = (float) dato;
		extraer_dato_double(respuesta, HUMEDAD, &dato);
		datosApp->termostato.humedad = (float) dato;
		free(texto_respuesta);

	}
	ESP_LOGI(TAG, ""TRAZAR" temperatura remota :%lf, humedad remota:%lf", INFOTRAZA, datosApp->termostato.tempActual,datosApp->termostato.humedad);
	cJSON_Delete(respuesta);

	//registrar_alarma(datosApp, NOTIFICACION_ALARMA_SENSOR_REMOTO, ALARMA_SENSOR_REMOTO, ALARMA_OFF, flag_envio);
	send_event(EVENT_DEVICE_OK);

	notify_end_starting(datosApp);
	return ESP_OK;
}
/*
void nemonicos_alarmas(DATOS_APLICACION *datosApp, int i) {

	ESP_LOGI(TAG, ""TRAZAR"nemonicos_alarmas", INFOTRAZA);
	
	strncpy(datosApp->alarmas[ALARM_DEVICE].nemonico, MNEMONIC_ALARM_DEVICE, 50);
	strncpy(datosApp->alarmas[ALARM_APP].nemonico, MNEMONIC_ALARM_APP, 50);
	strncpy(datosApp->alarmas[ALARM_NVS].nemonico, MNEMONIC_ALARM_NVS, 50);
	strncpy(datosApp->alarmas[ALARM_WIFI].nemonico, MNEMONIC_ALARM_WIFI, 50);
	strncpy(datosApp->alarmas[ALARM_NTP].nemonico, MNEMONIC_ALARM_NTP, 50);
	strncpy(datosApp->alarmas[ALARM_MQTT].nemonico, MNEMONIC_ALARM_MQTT, 50);




}
*/

esp_err_t appuser_notify_app_status(DATOS_APLICACION *datosApp, enum ESTADO_APP estado) {

	char status[50] = {0};

	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_app_status", INFOTRAZA);

	switch(datosApp->datosGenerales->estadoApp) {

	case NORMAL_AUTO:
		strcpy(status, "AUTO");
		break;
	case NORMAL_AUTOMAN:
		strcpy(status, "AUTO*");
		break;
	case NORMAL_MANUAL:
		strcpy(status, "MANUAL");
		break;
	case STARTING:
		strcpy(status, "STARTING");
		break;
	case NORMAL_SIN_PROGRAMACION:
		strcpy(status, "NO ACTIVO");
		break;
	case UPGRADE_EN_PROGRESO:
		strcpy(status, "UPGRADE EN PROGRESO");
		break;
	case NORMAL_SINCRONIZANDO:
		strcpy(status, "SINCRONIZANDO");
		break;
	case ESPERA_FIN_ARRANQUE:
		strcpy(status, "----");
		break;
	case FACTORY:
		strcpy(status, "FACTORY");
		break;
	case NORMAL_FIN_PROGRAMA_ACTIVO:
		strcpy(status, "AUTO");
		break;
	case ERROR_APP:
		strcpy(status, "ERROR_APP");
		break;
	case DEVICE_ALONE:
		strcpy(status, "TERMOSTATO");
		break;
	case CHECK_PROGRAMS:
		strcpy(status, "CHECK");
		break;


	}


	lv_update_status_application(status);
	return ESP_OK;
}


void appuser_notify_schedule_events(DATOS_APLICACION *datosApp) {

	//ESP_LOGI(TAG, ""TRAZAR"appuser_notify_schedule_events", INFOTRAZA);
	
	static bool start = true;
	char fecha_actual[10] = {0};
	time_t now;
	struct tm fecha;


    time(&now);
    localtime_r(&now, &fecha);
    if ((start == true) || (fecha.tm_sec == 0)) {
    	sprintf(fecha_actual, "%02d:%02d", fecha.tm_hour, fecha.tm_min);
    	ESP_LOGI(TAG, ""TRAZAR"hora actualizada: %s", INFOTRAZA, fecha_actual);
    	lv_update_hour(fecha_actual);
    	start = false;
    	lv_update_bar_schedule(datosApp);
    }





}


void display_status_application(DATOS_APLICACION *datosApp, cJSON *respuesta) {

	ESP_LOGI(TAG, ""TRAZAR"display_status_application", INFOTRAZA);
	//gpio_rele_in();
    cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
    cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
    cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
    cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->termostato.tempUmbral);
    cJSON_AddNumberToObject(respuesta, MARGEN_TEMPERATURA, datosApp->termostato.margenTemperatura);
    cJSON_AddNumberToObject(respuesta, INTERVALO_LECTURA, datosApp->termostato.intervaloLectura);
    cJSON_AddNumberToObject(respuesta, INTERVALO_REINTENTOS, datosApp->termostato.intervaloReintentos);
    cJSON_AddNumberToObject(respuesta, REINTENTOS_LECTURA, datosApp->termostato.reintentosLectura);
    cJSON_AddNumberToObject(respuesta, CALIBRADO, datosApp->termostato.calibrado);
    cJSON_AddBoolToObject(respuesta, MASTER, datosApp->termostato.master);
    cJSON_AddStringToObject(respuesta, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
    cJSON_AddNumberToObject(respuesta, UMBRAL_DEFECTO, datosApp->termostato.tempUmbralDefecto);


    //appUser_parametrosAplicacion(datosApp, respuesta);
    escribir_programa_actual(datosApp, respuesta);
    codigoRespuesta(respuesta,RESP_OK);



}


void display_act_remote_relay(DATOS_APLICACION *datosApp, cJSON *respuesta) {

	ESP_LOGI(TAG, ""TRAZAR"display_act_remote_relay", INFOTRAZA);
    cJSON_AddNumberToObject(respuesta, DLG_COD_RESPUESTA, DLG_OK_CODE);
    //gpio_rele_in();
    cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
    cJSON_AddNumberToObject(respuesta, TEMPERATURA, datosApp->termostato.tempActual);
    cJSON_AddNumberToObject(respuesta, HUMEDAD, datosApp->termostato.humedad);
    cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->termostato.tempUmbral);
    escribir_programa_actual(datosApp, respuesta);
    codigoRespuesta(respuesta,DLG_OK_CODE);
}


bool modify_threshold_temperature(cJSON *peticion, DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON *nodo = NULL;
    cJSON *campo = NULL;



    ESP_LOGI(TAG, ""TRAZAR"modify_threshold_temperature", INFOTRAZA);
    nodo = cJSON_GetObjectItem(peticion, MODIFICAR_APP);
   if(nodo == NULL) {
       return NULL;
   }

    printf("modify_threshold_temperature-->comienzo\n");
    campo = cJSON_GetObjectItem(nodo, UMBRAL_TEMPERATURA);
       if((campo != NULL) && (campo->type == cJSON_Number)) {
           printf("modificando umbral\n");
           datosApp->termostato.tempUmbral = campo->valuedouble;
           /*
           if (datosApp->datosGenerales->estadoApp == NORMAL_AUTO) {
        	   appuser_cambiar_modo_aplicacion(datosApp, NORMAL_AUTOMAN);
           }
           */
           lv_update_device(datosApp);
           //datosApp->datosGenerales->estadoApp = NORMAL_AUTOMAN;
           //guardarConfiguracion(datosApp, 0);
           accionar_termostato(datosApp);
           cJSON_AddNumberToObject(respuesta, UMBRAL_TEMPERATURA, datosApp->termostato.tempUmbral);
           cJSON_AddNumberToObject(respuesta, APP_COMAND_ESTADO_RELE, gpio_get_level(CONFIG_GPIO_PIN_RELE));
           codigoRespuesta(respuesta, RESP_OK);
       } else {
           codigoRespuesta(respuesta, RESP_NOK);
       }
    return true;
}


esp_err_t select_temperature_sensor(cJSON *peticion, DATOS_APLICACION *datosApp, cJSON *respuesta) {

	cJSON *nodo = NULL;
    bool master = true;

    ESP_LOGI(TAG, ""TRAZAR"select_temperature_sensor", INFOTRAZA);
    nodo = cJSON_GetObjectItem(peticion, MODIFICAR_SENSOR_TEMPERATURA);
   if(nodo == NULL) {
	   ESP_LOGE(TAG, ""TRAZAR" NO SE ENCUENTRA EL PATRON DE SENSOR EN EL COMANDO", INFOTRAZA);
	   codigoRespuesta(respuesta, RESP_NOK);
	   return ESP_FAIL;
   }



   if (extraer_dato_uint8(nodo,  MASTER, (uint8_t*) &master) != ESP_OK) {
	   ESP_LOGE(TAG, ""TRAZAR" NO VIENE EL CAMPO MASTER EN LA PETICION", INFOTRAZA);
	   codigoRespuesta(respuesta, RESP_NOK);
	   return ESP_FAIL;
   }

   if (master) {
	   ESP_LOGI(TAG, ""TRAZAR" Se modifica el sensor para que el master sea el dispositivo", INFOTRAZA);
	   memset(datosApp->termostato.sensor_remoto, 0,sizeof(datosApp->termostato.sensor_remoto));
   } else {
	   ESP_LOGE(TAG, ""TRAZAR"ante: %ld\n", INFOTRAZA, esp_get_free_heap_size());
	   //strcpy(datosApp->termostato.sensor_remoto, cJSON_GetObjectItem(nodo, SENSOR_REMOTO)->valuestring);
	   extraer_dato_string(nodo, SENSOR_REMOTO, datosApp->termostato.sensor_remoto);
	   ESP_LOGE(TAG, ""TRAZAR"despues: %ld\n", INFOTRAZA, esp_get_free_heap_size());
	   ESP_LOGW(TAG, ""TRAZAR" Se selecciona el sensor remoto a :%s", INFOTRAZA, datosApp->termostato.sensor_remoto);
   }
   datosApp->termostato.master = master;
   salvar_configuracion_general(datosApp);
   codigoRespuesta(respuesta, RESP_OK);

	return ESP_OK;
}


esp_err_t appuser_set_command_application(cJSON *peticion, int nComando, DATOS_APLICACION *datosApp, cJSON *respuesta) {

	ESP_LOGI(TAG, ""TRAZAR"appuser_set_command_application", INFOTRAZA);
    switch(nComando) {
        case OPERAR_RELE:
            operacion_rele(datosApp, MANUAL, INDETERMINADO);
            display_act_remote_relay(datosApp, respuesta);
            break;

        case STATUS_DISPOSITIVO:
            display_status_application(datosApp, respuesta);
            break;
        case MODIFICAR_UMBRAL:
        	modify_threshold_temperature(peticion, datosApp, respuesta);
        	//modify_threshold_temperature(peticion, datosApp, respuesta);
        	break;
			/*
            if ((modificarUmbralTemperatura(peticion, datosApp, respuesta) == true)) {
            	accionar_termostato(datosApp);
            }
            */
        case SELECCIONAR_SENSOR:
        	select_temperature_sensor(peticion, datosApp, respuesta);
        	break;

        default:
            visualizar_comando_desconocido(datosApp, respuesta);
            break;
    }




	return ESP_OK;
}



void appuser_notify_scan_done(DATOS_APLICACION *datosApp, wifi_ap_record_t *ap_info, uint16_t *ap_count) {


	ESP_LOGI(TAG, ""TRAZAR"appuser_notify_scan_done", INFOTRAZA);
	ESP_LOGI(TAG, ""TRAZAR"RECIBIDAS %d redes en app", INFOTRAZA, *ap_count);
	lv_create_layout_search_ssid(ap_info, ap_count);


}

void appuser_process_event(DATOS_APLICACION *datosApp, EVENT_TYPE event) {

	switch (datosApp->datosGenerales->estadoApp) {

	case ERROR_APP:
		break;

	case DEVICE_ALONE:
		break;
	case NORMAL_AUTO:
		break;
	case NORMAL_AUTOMAN:
		break;
	case NORMAL_MANUAL:
		break;
	case STARTING:
		break;
	case NORMAL_SIN_PROGRAMACION:
		break;
	case UPGRADE_EN_PROGRESO:
		break;
	case NORMAL_SINCRONIZANDO:
		break;
	case ESPERA_FIN_ARRANQUE:
		break;
	case FACTORY:
		break;
	case NORMAL_FIN_PROGRAMA_ACTIVO:
		break;
	case CHECK_PROGRAMS:
		break;
	default:
		break;



	}



}





