/*
 * alarmas.c
 *
 *  Created on: 10 oct. 2020
 *      Author: t126401
 */


#include "alarmas.h"
#include "ntp.h"
#include "stdint.h"
#include "configuracion_usuario.h"
#include "api_json.h"
#include "interfaz_usuario.h"
#include "alarm_data.h"


xQueueHandle event_queue;
static const char *TAG = "ALARMAS";


char* event2mnemonic(EVENT_TYPE event) {

	static char mnemonic[50] = {0};


	switch (event) {

	case EVENT_WARNING_DEVICE:
		strcpy(mnemonic, "EVENT_WARNING_DEVICE");
		break;
	case EVENT_ERROR_DEVICE:
		strcpy(mnemonic, "EVENT_ERROR_DEVICE");
		break;
	case EVENT_ERROR_APP:
		strcpy(mnemonic, "EVENT_ERROR_APP");
		break;
	case EVENT_ERROR_NVS:
		strcpy(mnemonic, "EVENT_ERROR_NVS");
		break;
	case EVENT_ERROR_LCD:
		strcpy(mnemonic, "EVENT_ERROR_LCD");
		break;
	case EVENT_ERROR_NTP:
		strcpy(mnemonic, "EVENT_ERROR_NTP");
		break;
	case EVENT_ERROR_WIFI:
		strcpy(mnemonic, "EVENT_ERROR_WIFI");
		break;
	case EVENT_ERROR_MQTT:
		strcpy(mnemonic, "EVENT_ERROR_MQTT");
		break;
	case EVENT_DEVICE_OK:
		strcpy(mnemonic, "EVENT_DEVICE_OK");
		break;
	case EVENT_APP_OK:
		strcpy(mnemonic, "EVENT_APP_OK");
		break;
	case EVENT_NVS_OK:
		strcpy(mnemonic, "EVENT_NVS_OK");

		break;
	case EVENT_LCD_OK:
		strcpy(mnemonic, "EVENT_LCD_OK");
		break;
	case EVENT_NTP_OK:
		strcpy(mnemonic, "EVENT_NTP_OK");
		break;
	case EVENT_WIFI_OK:
		strcpy(mnemonic, "EVENT_WIFI_OK");
		break;
	case EVENT_MQTT_OK:
		strcpy(mnemonic, "EVENT_MQTT_OK");
		break;
	case EVENT_INSERT_SCHEDULE:
		strcpy(mnemonic, "EVENT_INSERT_SCHEDULE");
		break;
	case EVENT_MODIFY_SCHEDULE:
		strcpy(mnemonic, "EVENT_MODIFY_SCHEDULE");
		break;
	case EVENT_DELETE_SCEDULE:
		strcpy(mnemonic, "EVENT_DELETE_SCEDULE");
		break;
	case EVENT_START_SCHEDULE:
		strcpy(mnemonic, "EVENT_START_SCHEDULE");
		break;
	case EVENT_END_SCHEDULE:
		strcpy(mnemonic, "EVENT_END_SCHEDULE");
		break;
	case EVENT_NONE_SCHEDULE:
		strcpy(mnemonic, "EVENT_NONE_SCHEDULE");
		break;
	case EVENT_FACTORY:
		strcpy(mnemonic, "EVENT_FACTORY");
		break;
	case EVENT_UPGRADE_FIRMWARE:
		strcpy(mnemonic, "EVENT_UPGRADE_FIRMWARE");
		break;




	}

	return mnemonic;

}

char* status_alarm_to_mnemonic(ALARM_STATUS alarm) {

	static char mnemonic_alarm[16] = {0};

	switch (alarm) {

	case ALARM_UNDEFINED:
		strcpy(mnemonic_alarm, "ALARM_UNDEFINED");
	break;
	case ALARM_OFF:
		strcpy(mnemonic_alarm, "ALARM_OFF");
	break;
	case ALARM_WARNING:
		strcpy(mnemonic_alarm, "ALARM_WARNING");
	break;
	case ALARM_ON:
		strcpy(mnemonic_alarm, "ALARM_ON");
	break;


	}

	return mnemonic_alarm;


}


static esp_err_t registrar_alarma(DATOS_APLICACION *datosApp, char* mnemonico_alarma, ALARM_TYPE tipo_alarma, enum ALARM_STATUS estado_alarma, bool flag_envio) {

	//Registramos la alarma.


	ESP_LOGE(TAG, ""TRAZAR"registrar_alarma", INFOTRAZA);
	datosApp->alarmas[tipo_alarma].estado_alarma = estado_alarma;
	datosApp->alarmas[tipo_alarma].fecha_alarma = datosApp->datosGenerales->clock.time;
	if (flag_envio == true) {
		notificar_evento_alarma(datosApp, tipo_alarma, mnemonico_alarma);
	}


	ESP_LOGE(TAG, ""TRAZAR" ALARMA  %s ESTADO %s REGISTRADA", INFOTRAZA, datosApp->alarmas[tipo_alarma].nemonico, status_alarm_to_mnemonic(estado_alarma));




	appuser_notify_local_alarm(datosApp, tipo_alarma);
	return ESP_OK;
}


void process_event_warning_device(DATOS_APLICACION *datosApp) {

}

void process_event_error_device(DATOS_APLICACION *datosApp) {

}

void process_event_error_app(DATOS_APLICACION *datosApp) {


}

void process_event_error_nvs(DATOS_APLICACION *datosApp) {


	switch(datosApp->datosGenerales->estadoApp) {

	case NORMAL_AUTO:
		esp_restart();
		break;
	case NORMAL_AUTOMAN:
		esp_restart();
		break;
	case NORMAL_MANUAL:
		esp_restart();
		break;
	case STARTING:
		esp_restart();
		break;
	case NORMAL_SIN_PROGRAMACION:
		esp_restart();
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
	case ERROR_APP:
		break;
	case DEVICE_ALONE:
		break;
	case CHECK_PROGRAMS:
		break;


	}


}

void process_event_nvs_ok(DATOS_APLICACION *datosApp) {


	switch(datosApp->datosGenerales->estadoApp) {


	case STARTING:
		//datosApp->alarmas[ALARM_NVS].estado_alarma = ALARM_OFF;
		registrar_alarma(datosApp, MNEMONIC_ALARM_NVS, ALARM_NVS, ALARM_OFF, true);
		break;

	default:
		registrar_alarma(datosApp, MNEMONIC_ALARM_NVS, ALARM_NVS, ALARM_OFF, true);
		break;
	}





}

void process_event_wifi_ok(DATOS_APLICACION *datosApp) {

	switch(datosApp->datosGenerales->estadoApp) {

	case NORMAL_AUTOMAN:
	case NORMAL_MANUAL:
	case NORMAL_SIN_PROGRAMACION:
	case UPGRADE_EN_PROGRESO:
	case NORMAL_SINCRONIZANDO:
	case ESPERA_FIN_ARRANQUE:
	case NORMAL_AUTO:
	case ERROR_APP:
	case DEVICE_ALONE:
	case CHECK_PROGRAMS:
		registrar_alarma(datosApp, MNEMONIC_ALARM_WIFI, ALARM_WIFI, ALARM_OFF, true);
		//appuser_notify_wifi_connected_ok(datosApp);
		break;

	case FACTORY:
	case NORMAL_FIN_PROGRAMA_ACTIVO:
	case STARTING:
		registrar_alarma(datosApp, MNEMONIC_ALARM_WIFI, ALARM_WIFI, ALARM_OFF, false);
		break;
	}

	appuser_notify_wifi_connected_ok(datosApp);

}

void process_event_error_wifi(DATOS_APLICACION *datosApp) {

	switch(datosApp->datosGenerales->estadoApp) {

	case NORMAL_AUTO:
	case NORMAL_AUTOMAN:
	case NORMAL_MANUAL:
	case STARTING:
	case NORMAL_SIN_PROGRAMACION:
	case NORMAL_SINCRONIZANDO:
	case ESPERA_FIN_ARRANQUE:
	case UPGRADE_EN_PROGRESO:
	case NORMAL_FIN_PROGRAMA_ACTIVO:
	case ERROR_APP:
	case DEVICE_ALONE:
	case CHECK_PROGRAMS:
	case FACTORY:

		registrar_alarma(datosApp, MNEMONIC_ALARM_WIFI, ALARM_WIFI, ALARM_ON, false);
		registrar_alarma(datosApp, MNEMONIC_ALARM_NTP, ALARM_NTP, ALARM_ON, false);
		registrar_alarma(datosApp, MNEMONIC_ALARM_MQTT, ALARM_MQTT, ALARM_ON, false);

	default:
		break;

	}

	appuser_notify_error_wifi_connection(datosApp);


}





void receive_event(DATOS_APLICACION *datosApp, EVENT_TYPE event) {

	ESP_LOGE(TAG, ""TRAZAR"receive_event", INFOTRAZA);

	switch (event) {

	case EVENT_WARNING_DEVICE:
		break;
	case EVENT_ERROR_DEVICE:
		break;
	case EVENT_ERROR_APP:
		ESP_LOGE(TAG, ""TRAZAR"RECIBIDO ERROR APP", INFOTRAZA);
		break;
	case EVENT_ERROR_NVS:

		if (datosApp->datosGenerales->estadoApp == STARTING) {
			datosApp->alarmas[ALARM_NVS].estado_alarma = ALARM_ON;

		} else {
			registrar_alarma(datosApp, MNEMONIC_ALARM_NVS, ALARM_NVS, ALARM_ON, true);
		}

		break;
	case EVENT_ERROR_LCD:
		ESP_LOGE(TAG, ""TRAZAR"RECIBIDO ERROR LCD", INFOTRAZA);
		registrar_alarma(datosApp, MNEMONIC_ALARM_LCD, ALARM_LCD, ALARM_ON, true);
		break;
	case EVENT_ERROR_NTP:
		registrar_alarma(datosApp, MNEMONIC_ALARM_NTP, ALARM_NTP, ALARM_ON, true);
		break;
	case EVENT_ERROR_WIFI:
		process_event_error_wifi(datosApp);
		break;
	case EVENT_ERROR_MQTT:
		registrar_alarma(datosApp, MNEMONIC_ALARM_MQTT, ALARM_MQTT, ALARM_ON, false);
		break;
	case EVENT_DEVICE_OK:
		break;
	case EVENT_APP_OK:
		ESP_LOGE(TAG, ""TRAZAR"RECIBIDO APP OK", INFOTRAZA);
		break;
	case EVENT_NVS_OK:
		process_event_nvs_ok(datosApp);

		break;
	case EVENT_LCD_OK:
		ESP_LOGE(TAG, ""TRAZAR"RECIBIDO LCD OK", INFOTRAZA);
		break;
	case EVENT_NTP_OK:
		registrar_alarma(datosApp, MNEMONIC_ALARM_NTP, ALARM_NTP, ALARM_OFF, true);
		break;
	case EVENT_WIFI_OK:
		process_event_wifi_ok(datosApp);
		break;
	case EVENT_MQTT_OK:
		registrar_alarma(datosApp, MNEMONIC_ALARM_MQTT, ALARM_MQTT, ALARM_ON, true);
		break;
	case EVENT_INSERT_SCHEDULE:
		break;
	case EVENT_MODIFY_SCHEDULE:
		break;
	case EVENT_DELETE_SCEDULE:
		break;
	case EVENT_START_SCHEDULE:
		break;
	case EVENT_END_SCHEDULE:
		break;
	case EVENT_NONE_SCHEDULE:
		break;
	case EVENT_FACTORY:
		change_status_application(datosApp, FACTORY);
		break;
	case EVENT_UPGRADE_FIRMWARE:
		break;

	}


}


void init_alarms(DATOS_APLICACION *datosApp) {

	int i;
	NTP_CLOCK clock;
	for (i=ALARM_DEVICE; i< ALARM_MQTT+1;i++) {
		datosApp->alarmas[i].tipo_alarma = i;
		datosApp->alarmas[i].estado_alarma = ALARM_ON;
		actualizar_hora(&clock);
		datosApp->alarmas[i].fecha_alarma = clock.time;
		strncpy(datosApp->alarmas[ALARM_DEVICE].nemonico, MNEMONIC_ALARM_DEVICE, 50);
		strncpy(datosApp->alarmas[ALARM_APP].nemonico, MNEMONIC_ALARM_APP, 50);
		strncpy(datosApp->alarmas[ALARM_LCD].nemonico, MNEMONIC_ALARM_LCD, 50);
		strncpy(datosApp->alarmas[ALARM_NVS].nemonico, MNEMONIC_ALARM_NVS, 50);
		strncpy(datosApp->alarmas[ALARM_WIFI].nemonico, MNEMONIC_ALARM_WIFI, 50);
		strncpy(datosApp->alarmas[ALARM_NTP].nemonico, MNEMONIC_ALARM_NTP, 50);
		strncpy(datosApp->alarmas[ALARM_MQTT].nemonico, MNEMONIC_ALARM_MQTT, 50);
		ESP_LOGI(TAG, ""TRAZAR" INICIALIZADAS ALARMAS, TIPO_ALARMA: %s, ESTADO: %s, FECHA: %llu", INFOTRAZA,
				datosApp->alarmas[i].nemonico,
				status_alarm_to_mnemonic(datosApp->alarmas[i].estado_alarma),
				datosApp->alarmas[i].fecha_alarma);
	}

}



void event_task(void *arg) {

	EVENT_TYPE event;
	DATOS_APLICACION *datosApp = (DATOS_APLICACION* ) arg;
	event_queue = xQueueCreate(10, sizeof(EVENT_TYPE));

	for(;;) {
		 ESP_LOGI(TAG, ""TRAZAR"ESPERANDO EVENTO...Memoria libre: %ld\n", INFOTRAZA, esp_get_free_heap_size());
		if (xQueueReceive(event_queue, &event, portMAX_DELAY) == pdTRUE) {
			ESP_LOGE(TAG, ""TRAZAR"Recibido evento %s", INFOTRAZA, event2mnemonic(event));
			receive_event(datosApp, event);


		} else {
			ESP_LOGE(TAG, ""TRAZAR"NO SE HA PODIDO PROCESAR LA PETICION", INFOTRAZA);
		}

	}
	vTaskDelete(NULL);


}

void create_event_task(DATOS_APLICACION *datosApp) {


	init_alarms(datosApp);
	xTaskCreate(event_task, "mqtt_task", 4096, (void*) datosApp, 4, NULL);
	ESP_LOGW(TAG, ""TRAZAR"TAREA DE EVENTOS CREADA", INFOTRAZA);
	change_status_application(datosApp, STARTING);

}

void send_event(EVENT_TYPE event) {

	xQueueSend(event_queue, &event,0);

}


ESTADO_APP current_status_application(DATOS_APLICACION *datosApp) {

	return datosApp->datosGenerales->estadoApp;


}




