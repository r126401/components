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


static esp_err_t registrar_alarma(DATOS_APLICACION *datosApp, char* mnemonico_alarma, uint8_t tipo_alarma, enum ESTADO_ALARMA estado_alarma, bool flag_envio) {

	//Registramos la alarma.


	ESP_LOGE(TAG, ""TRAZAR"registrar_alarma", INFOTRAZA);
	datosApp->alarmas[tipo_alarma].estado_alarma = estado_alarma;
	datosApp->alarmas[tipo_alarma].fecha_alarma = datosApp->datosGenerales->clock.time;
	if (flag_envio == true) {
		notificar_evento_alarma(datosApp, tipo_alarma, mnemonico_alarma);
	}


	switch(estado_alarma) {

	case ALARMA_OFF:
		ESP_LOGI(TAG, ""TRAZAR" ALARMA OFF %d ESTADO %d REGISTRADA", INFOTRAZA, tipo_alarma, estado_alarma);
		break;
	case ALARMA_WARNING:
		ESP_LOGW(TAG, ""TRAZAR" ALARMA WARNING %d ESTADO %d REGISTRADA", INFOTRAZA, tipo_alarma, estado_alarma);
		break;
	case ALARMA_ON:
		ESP_LOGW(TAG, ""TRAZAR" ALARMA ON %d ESTADO %d REGISTRADA", INFOTRAZA, tipo_alarma, estado_alarma);
		break;
	default:
		ESP_LOGI(TAG, ""TRAZAR" ALARMA INDETERMINADA %d ESTADO %d REGISTRADA", INFOTRAZA, tipo_alarma, estado_alarma);
		break;

	}


	appuser_notify_local_alarm(datosApp, tipo_alarma);
	return ESP_OK;
}

/*
void send_event_application(DATOS_APLICACION *datosApp, char* mnemonico_alarma, EVENT_TYPE event) {

	switch (event) {

	case EVENT_WARNING_DEVICE:
		break;
	case EVENT_ERROR_DEVICE:
		break;
	case EVENT_ERROR_APP:
		break;
	case EVENT_ERROR_NVS:
		process_event_error_nvs(datosApp, event);
		break;
	case EVENT_ERROR_LCD:
		break;
	case EVENT_ERROR_NTP:
		registrar_alarma(datosApp, mnemonico_alarma, ALARMA_NTP, ALARMA_ON, true);
		break;
	case EVENT_ERROR_WIFI:
		registrar_alarma(datosApp, mnemonico_alarma, ALARMA_WIFI, ALARMA_ON, false);
		break;
	case EVENT_ERROR_MQTT:
		registrar_alarma(datosApp, mnemonico_alarma, ALARMA_MQTT, ALARMA_ON, false);
		break;
	case EVENT_DEVICE_OK:
		break;
	case EVENT_APP_OK:
		break;
	case EVENT_NVS_OK:
		process_event_nvs_ok(datosApp, event);
		break;
	case EVENT_LCD_OK:
		break;
	case EVENT_NTP_OK:
		registrar_alarma(datosApp, mnemonico_alarma, ALARMA_NTP, ALARMA_OFF, true);
		break;
	case EVENT_WIFI_OK:
		if (datosApp->alarmas[ALARMA_WIFI].estado_alarma == ALARMA_ON) {
			registrar_alarma(datosApp, mnemonico_alarma, ALARMA_WIFI, ALARMA_OFF, true);
		}
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case EVENT_MQTT_OK:
		registrar_alarma(datosApp, mnemonico_alarma, ALARMA_MQTT, ALARMA_ON, true);
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
		break;
	case EVENT_UPGRADE_FIRMWARE:
		break;

	}

}
*/
void process_event_warning_device(DATOS_APLICACION *datosApp, EVENT_TYPE event) {

}

void process_event_error_device(DATOS_APLICACION *datosApp, EVENT_TYPE event) {

}

void process_event_error_app(DATOS_APLICACION *datosApp, EVENT_TYPE event) {


}

void process_event_error_nvs(DATOS_APLICACION *datosApp, EVENT_TYPE event) {


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

void process_event_nvs_ok(DATOS_APLICACION *datosApp, EVENT_TYPE event) {


	switch(datosApp->datosGenerales->estadoApp) {


	case STARTING:
		datosApp->alarmas[ALARMA_NVS].estado_alarma = ALARMA_OFF;
		strcpy (datosApp->alarmas[ALARMA_NVS].nemonico, NOTIFICACION_ALARMA_NVS);
		datosApp->alarmas[ALARMA_NVS].tipo_alarma = ALARMA_NVS;
		ESP_LOGE(TAG, ""TRAZAR"nvs ok!!!!!!!!!!!!!!!!!!!!!", INFOTRAZA);
		break;

	default:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_NVS, ALARMA_NVS, ALARMA_OFF, true);
		break;
	}





}

void process_event_wifi_ok(DATOS_APLICACION *datosApp) {

	switch(datosApp->datosGenerales->estadoApp) {

	case NORMAL_AUTO:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_OFF, true);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case NORMAL_AUTOMAN:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_OFF, true);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case NORMAL_MANUAL:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_OFF, true);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case STARTING:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_OFF, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case NORMAL_SIN_PROGRAMACION:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_OFF, true);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case UPGRADE_EN_PROGRESO:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_OFF, true);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case NORMAL_SINCRONIZANDO:
		break;
	case ESPERA_FIN_ARRANQUE:
		break;
	case FACTORY:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_OFF, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case NORMAL_FIN_PROGRAMA_ACTIVO:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_OFF, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case ERROR_APP:
		break;
	case DEVICE_ALONE:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_OFF, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case CHECK_PROGRAMS:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_OFF, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;


	}


}

void process_event_error_wifi(DATOS_APLICACION *datosApp) {

	switch(datosApp->datosGenerales->estadoApp) {

	case NORMAL_AUTO:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case NORMAL_AUTOMAN:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case NORMAL_MANUAL:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case STARTING:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case NORMAL_SIN_PROGRAMACION:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case UPGRADE_EN_PROGRESO:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case NORMAL_SINCRONIZANDO:
		break;
	case ESPERA_FIN_ARRANQUE:
		break;
	case FACTORY:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case NORMAL_FIN_PROGRAMA_ACTIVO:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case ERROR_APP:
		break;
	case DEVICE_ALONE:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;
	case CHECK_PROGRAMS:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
		appuser_notify_wifi_connected_ok(datosApp);
		break;


	}


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
			datosApp->alarmas[ALARMA_NVS].estado_alarma = ALARMA_ON;
			strcpy (datosApp->alarmas[ALARMA_NVS].nemonico, NOTIFICACION_ALARMA_NVS);
			datosApp->alarmas[ALARMA_NVS].tipo_alarma = ALARMA_NVS;

		} else {
			registrar_alarma(datosApp, NOTIFICACION_ALARMA_NVS, ALARMA_NVS, ALARMA_ON, true);
		}

		break;
	case EVENT_ERROR_LCD:
		ESP_LOGE(TAG, ""TRAZAR"RECIBIDO ERROR LCD", INFOTRAZA);
		break;
	case EVENT_ERROR_NTP:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_NTP, ALARMA_NTP, ALARMA_ON, true);
		break;
	case EVENT_ERROR_WIFI:
		process_event_error_wifi(datosApp);
		break;
	case EVENT_ERROR_MQTT:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_MQTT, ALARMA_MQTT, ALARMA_ON, false);
		break;
	case EVENT_DEVICE_OK:
		break;
	case EVENT_APP_OK:
		ESP_LOGE(TAG, ""TRAZAR"RECIBIDO APP OK", INFOTRAZA);
		break;
	case EVENT_NVS_OK:


		break;
	case EVENT_LCD_OK:
		ESP_LOGE(TAG, ""TRAZAR"RECIBIDO LCD OK", INFOTRAZA);
		break;
	case EVENT_NTP_OK:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_NTP, ALARMA_NTP, ALARMA_OFF, true);
		break;
	case EVENT_WIFI_OK:
		process_event_wifi_ok(datosApp);
		break;
	case EVENT_MQTT_OK:
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_MQTT, ALARMA_MQTT, ALARMA_ON, true);
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
		break;
	case EVENT_UPGRADE_FIRMWARE:
		break;

	}


}





esp_err_t inicializacion_registros_alarmas(DATOS_APLICACION *datosApp) {

	int i;
	NTP_CLOCK clock;
	for (i=0;i<NUM_TIPOS_ALARMAS;i++) {

		datosApp->alarmas[i].tipo_alarma = i;
		datosApp->alarmas[i].estado_alarma = ALARMA_ON;
		actualizar_hora(&clock);
		datosApp->alarmas[i].fecha_alarma = clock.time;
		ESP_LOGI(TAG, ""TRAZAR" INICIALIZADAS ALARMAS, TIPO_ALARMA: %d, ESTADO: %d, FECHA: %llu", INFOTRAZA,
				datosApp->alarmas[i].tipo_alarma,
				datosApp->alarmas[i].estado_alarma,
				datosApp->alarmas[i].fecha_alarma);
		nemonicos_alarmas(datosApp, i);
	}

	ESP_LOGI(TAG, ""TRAZAR"REGISTROS DE ALARMAS INICIALIZADOS", INFOTRAZA);

	return ESP_OK;

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


	inicializacion_registros_alarmas(datosApp);
	xTaskCreate(event_task, "mqtt_task", 4096, (void*) datosApp, 4, NULL);
	ESP_LOGW(TAG, ""TRAZAR"TAREA DE EVENTOS CREADA", INFOTRAZA);

}

void send_event(EVENT_TYPE event) {

	xQueueSend(event_queue, &event,0);

}




