/*
 * events.c
 *
 *  Created on: Oct 25, 2023
 *      Author: t126401
 */


#include "events_device.h"
#include "esp_log.h"
#include "esp_err.h"
#include "logging.h"
#include "alarmas.h"
#include "conexiones.h"
#include "user_interface.h"
#include "dialogos_json.h"
#include "conexiones_mqtt.h"
#include "applib.h"
#include "configuracion.h"





static const char *TAG = "events_device.c";
xQueueHandle event_queue;


char* event2mnemonic(EVENT_TYPE event) {

	static char mnemonic[50] = {0};


	switch (event) {

	case EVENT_ERROR_REMOTE_DEVICE:
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
	case EVENT_DEVICE_READY:
		strcpy(mnemonic, "EVENT_DEVICE_READY");
		break;
	case EVENT_REMOTE_DEVICE_OK:
		strcpy(mnemonic, "EVENT_REMOTE_DEVICE_OK");
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
	case EVENT_GET_NTP:
		strcpy(mnemonic, "EVENT_GET_NTP");
		break;

	case EVENT_WIFI_OK:
		strcpy(mnemonic, "EVENT_WIFI_OK");
		break;
	case EVENT_MQTT_OK:
		strcpy(mnemonic, "EVENT_MQTT_OK");
		break;
	case EVENT_CONNECT_MQTT:
		strcpy(mnemonic, "EVENT_CONNECT_MQTT");
		break;

	case EVENT_CHECK_SCHEDULES:
		strcpy(mnemonic, "EVENT_CHECK_PROGRAMS");
		break;
	case EVENT_INSERT_SCHEDULE:
		strcpy(mnemonic, "EVENT_INSERT_SCHEDULE");
		break;
	case EVENT_MODIFY_SCHEDULE:
		strcpy(mnemonic, "EVENT_MODIFY_SCHEDULE");
		break;
	case EVENT_DELETE_SCHEDULE:
		strcpy(mnemonic, "EVENT_DELETE_SCEDULE");
		break;
	case EVENT_START_SCHEDULE:
		strcpy(mnemonic, "EVENT_START_SCHEDULE");
		break;
	case EVENT_END_SCHEDULE:
		strcpy(mnemonic, "EVENT_END_SCHEDULE");
		break;
	case EVENT_NO_ACTIVE_SCHEDULE:
		strcpy(mnemonic, "EVENT_NO_ACTIVE_SCHEDULE");
		break;
	case EVENT_FACTORY:
		strcpy(mnemonic, "EVENT_FACTORY");
		break;
	case EVENT_UPGRADE_FIRMWARE:
		strcpy(mnemonic, "EVENT_UPGRADE_FIRMWARE");
		break;
	case EVENT_LOCAL_DEVICE:
		strcpy(mnemonic, "EVENT_LOCAL_DEVICE");
		break;

	case EVENT_SMARTCONFIG_START:
		strcpy(mnemonic, "EVENT_SMARTCONFIG_START");
		break;
	case EVENT_SMARTCONFIG_END:
		strcpy(mnemonic, "EVENT_SMARTCONFIG_END");
		break;

	case EVENT_START_APP:
		strcpy(mnemonic, "EVENT_START_APP");
		break;

	case EVENT_ERROR_SMARTCONFIG:
		strcpy(mnemonic, "EVENT_ERROR_SMARTCONFIG");
		break;


	case EVENT_END_UPGRADE:
		strcpy(mnemonic, "EVENT_END_UPGRADE");
		break;


	case EVENT_ERROR_UPGRADE:
		strcpy(mnemonic, "EVENT_END_UPGRADE");
		break;
	case EVENT_SCHEDULES_OK:
		strcpy(mnemonic, "EVENT_SCHEDULES_OK");
		break;
	case EVENT_NO_SCHEDULE:
		strcpy(mnemonic, "EVENT_NO_SCHEDULES");
		break;
	case EVENT_AUTOMAN:
		strcpy(mnemonic, "EVENT_AUTOMAN");
		break;
	case EVENT_STARTING:
		strcpy(mnemonic, "EVENT_STARTING");
		break;
	case EVENT_RESTART:
		strcpy(mnemonic, "EVENT_RESTART");
		break;

	case EVENT_MQTT_SUBSCRIBED:
		strcpy(mnemonic, "EVENT_MQTT_SUBSCRIBED");
		break;


	}





	return mnemonic;

}




void process_event_warning_device(DATOS_APLICACION *datosApp) {

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
		/*
	case NO_PROGRAM:
		esp_restart();
		break;
		*/
	case UPGRADING:
		break;

	case WAITING_END_STARTING:
		break;
	case FACTORY:
		break;
	case ERROR_APP:
		break;
	case CHECK_SCHEDULES:
		break;
	case SCHEDULING:
		break;
	case RESTARTING:
		break;
	case DEVICE_READY:
		break;

	case UNKNOWN_STATUS:
		break;
	case RECOVERING:
		break;


	}


}

void process_event_nvs_ok(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento nvs", INFOTRAZA);
	switch(datosApp->datosGenerales->estadoApp) {


	case STARTING:
		//datosApp->alarmas[ALARM_NVS].estado_alarma = ALARM_OFF;
		send_alarm(datosApp, ALARM_NVS, ALARM_OFF, true);
		break;

	default:
		send_alarm(datosApp, ALARM_NVS, ALARM_OFF, true);
		break;
	}





}

void process_event_wifi_ok(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_WIFI_OK", INFOTRAZA);


	set_status_application(datosApp, EVENT_WIFI_OK);


	send_alarm(datosApp, ALARM_WIFI, ALARM_OFF, true);

	appuser_notify_wifi_connected_ok(datosApp);


}

void process_event_error_wifi(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_ERROR_WIFI", INFOTRAZA);

	set_status_application(datosApp, EVENT_ERROR_WIFI);

	switch(datosApp->datosGenerales->estadoApp) {

	case FACTORY:
		ESP_LOGE(TAG, ""TRAZAR" ERROR AL PONER LA CLAVE WIFI EN SMARTCONFIG", INFOTRAZA);
		appuser_notify_error_smartconfig(datosApp);
		reinicio_fabrica(datosApp);
		break;

	default:
		send_alarm(datosApp, ALARM_WIFI, ALARM_ON, false);
		send_event(__func__,EVENT_ERROR_NTP);
		send_event(__func__,EVENT_ERROR_MQTT);

		break;

	}

	appuser_notify_error_wifi_connection(datosApp);



}

void process_event_ntp_ok(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_NTP_OK", INFOTRAZA);

	if (get_status_alarm(datosApp, ALARM_NTP) == ALARM_ON) {
		send_alarm(datosApp, ALARM_NTP, ALARM_OFF, true);
		actualizar_hora(&datosApp->datosGenerales->clock);
		datosApp->datosGenerales->estadoProgramacion = VALID_PROG;
		appuser_notify_sntp_ok(datosApp);

	}

	if (get_current_status_application(datosApp) == STARTING) {

		if (using_schedules(datosApp)) {
			init_schedule_service(datosApp);
		} else {
			send_event(__func__, EVENT_START_APP);
		}


	}

}

void process_event_error_ntp(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_ERROR_NTP", INFOTRAZA);

	switch(datosApp->datosGenerales->estadoApp) {

	case NORMAL_AUTO:
	case NORMAL_AUTOMAN:
		//change_status_application(datosApp, DEVICE_ALONE);
		send_alarm(datosApp, ALARM_NTP, ALARM_ON, true);
		datosApp->datosGenerales->estadoProgramacion = INVALID_PROG;
		break;

	default:
		send_alarm(datosApp, ALARM_NTP, ALARM_ON, true);
		datosApp->datosGenerales->estadoProgramacion = INVALID_PROG;
		break;


	}

	appuser_notify_error_sntp(datosApp);

}


void process_event_no_active_schedule(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_NO_ACTIVE_SCHEDULE", INFOTRAZA);

	set_status_application(datosApp, EVENT_NO_ACTIVE_SCHEDULE);
	appuser_notify_event_no_active_schedule(datosApp);




}

void process_event_end_schedule(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_END_SCHEDULE", INFOTRAZA);

	set_status_application(datosApp, EVENT_END_SCHEDULE);
	appuser_end_schedule(datosApp);


}

void process_event_start_schedule(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_START_SCHEDULE", INFOTRAZA);

	set_status_application(datosApp, EVENT_START_SCHEDULE);
	start_schedule(datosApp);
	appuser_start_schedule(datosApp);


}

void process_event_check_schedules(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_CHECK_SCHEDULES", INFOTRAZA);

	set_status_application(datosApp, EVENT_CHECK_SCHEDULES);
}








void process_event_app_ok(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_APP_OK", INFOTRAZA);





}

void process_event_mqtt_ok(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_MQTT_OK", INFOTRAZA);
	set_status_application(datosApp, EVENT_MQTT_OK);

	send_alarm(datosApp, ALARM_MQTT, ALARM_OFF, true);

	switch (get_current_status_application(datosApp)) {

	case STARTING:
		if (using_ntp(datosApp)) {
			init_ntp_service(datosApp);
		} else {
			send_event(__func__, EVENT_START_APP);
		}
		break;


	default:
		send_event(__func__, EVENT_DEVICE_READY);
		break;
	}



	appuser_notify_broker_connected_ok(datosApp);




}


void process_event_modify_schedule(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_MODIFY_SCHEDULE", INFOTRAZA);

	set_status_application(datosApp, EVENT_MODIFY_SCHEDULE);


}

void process_event_delete_schedule(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_DELETE_SCHEDULE", INFOTRAZA);

	set_status_application(datosApp, EVENT_DELETE_SCHEDULE);
}


void process_event_insert_schedule(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_INSERT_SCHEDULE", INFOTRAZA);

	set_status_application(datosApp, EVENT_INSERT_SCHEDULE);
}


void process_event_upgrade_firmware(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_UPGRADE_FIRMWARE", INFOTRAZA);

	set_status_application(datosApp, EVENT_UPGRADE_FIRMWARE);
	appuser_notify_start_ota(datosApp);

}


void process_event_factory(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_FACTORY", INFOTRAZA);
	set_status_application(datosApp, EVENT_FACTORY);

	appuser_notify_no_config(datosApp);


}

void process_event_device_ready(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_DEVICE_OK", INFOTRAZA);

	if (datosApp->alarmas[ALARM_DEVICE].estado_alarma == ALARM_ON) {
		send_alarm(datosApp, ALARM_DEVICE, ALARM_OFF, true);
	}
	set_status_application(datosApp, EVENT_DEVICE_READY);
	appuser_notify_device_ready(datosApp);
	if (using_schedules(datosApp)) {
		send_event(__func__, EVENT_CHECK_SCHEDULES);
	}

}


void process_event_smartconfig_start(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_SMARTCONFIG_START", INFOTRAZA);

	if (get_current_status_application(datosApp) == FACTORY) {
		ESP_LOGI(TAG, ""TRAZAR"SMARTCONFIG START EN MODO FACTORY", INFOTRAZA);
		init_wifi_device(datosApp);

	} else {
		ESP_LOGI(TAG, ""TRAZAR" RECIBIDO EVENTO SMARTCONFIG EN ESTADO %s", INFOTRAZA, status2mnemonic(get_current_status_application(datosApp)));
		set_status_application(datosApp, EVENT_SMARTCONFIG_START);
		reinicio_fabrica(datosApp);
		task_smartconfig(datosApp);


	}

}

void process_event_smartconfig_end(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_SMARTCONFIG_END", INFOTRAZA);

	//Ponemos el dispositivo como configurado y salvamos la configuracion.
	set_app_status_device(datosApp, DEVICE_CONFIGURED);
	salvar_configuracion_general(datosApp);
	set_status_application(datosApp, EVENT_SMARTCONFIG_END);
	send_event(__func__, EVENT_STARTING);
	appuser_notify_smartconfig_end(datosApp);

}

void process_event_error_smartconfig(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_ERROR_SMARTCONFIG", INFOTRAZA);

	appuser_notify_error_smartconfig(datosApp);
}

void process_event_end_upgrade(DATOS_APLICACION *datosApp) {

	//notificar_evento_ota(datosApp, OTA_UPGRADE_FINALIZADO);
	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_END_UPGRADE", INFOTRAZA);

	send_spontaneous_report(datosApp, END_UPGRADE_OTA);
	esp_restart();

}


void process_event_error_upgrade(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_ERROR_UPGRADE", INFOTRAZA);

	send_spontaneous_report(datosApp, ERROR_UPGRADE_OTA);
	esp_restart();

}

void process_event_error_mqtt(DATOS_APLICACION *datosApp) {


	static uint8_t retry = 0;
	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_ERROR_MQTT", INFOTRAZA);

	if (get_current_status_application(datosApp) == RECOVERING) {
		retry ++;
		ESP_LOGW(TAG,""TRAZAR" Contador de reintentos: %d", INFOTRAZA, retry);
		if (retry == 10) {
			esp_restart();
		}

	} else {
		retry = 0;
	}



	if (get_status_alarm(datosApp, ALARM_MQTT) == ALARM_ON) {
		ESP_LOGW(TAG, ""TRAZAR" Alarma ya activa, no se hace nada", INFOTRAZA);
		return;
	}


	send_alarm(datosApp, ALARM_MQTT, ALARM_ON, false);
	appuser_notify_broker_disconnected(datosApp);
	set_status_application(datosApp, EVENT_ERROR_MQTT);

}

void process_event_error_device(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_ERROR_DEVICE", INFOTRAZA);

	if (datosApp->alarmas[ALARM_DEVICE].estado_alarma == ALARM_OFF) {
		send_alarm(datosApp, ALARM_DEVICE, ALARM_ON, true);
	}
	appuser_notify_error_device(datosApp);
	change_status_application(datosApp, ERROR_APP);


}

void process_event_error_app(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_ERROR_APP", INFOTRAZA);

	ESP_LOGE(TAG, ""TRAZAR"RECIBIDO ERROR APP", INFOTRAZA);
	change_status_application(datosApp, ERROR_APP);


}

void process_event_start_app(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_START_APP", INFOTRAZA);
	set_status_application(datosApp, EVENT_START_APP);
	appuser_notify_application_started(datosApp);



}

void process_event_schedules_ok(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_SCHEDULES_OK", INFOTRAZA);

	send_event(__func__, EVENT_START_APP);



}


void process_event_no_schedule(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_NO_SCHEDULE", INFOTRAZA);

	set_status_application(datosApp, EVENT_NO_SCHEDULE);

	//change_status_application(datosApp, NORMAL_MANUAL);

}

void process_event_automan(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_AUTOMAN", INFOTRAZA);

	ESP_LOGI(TAG, ""TRAZAR" process_event_automan", INFOTRAZA);
}

void process_event_restart(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_RESTART", INFOTRAZA);

	set_status_application(datosApp, EVENT_RESTART);


}

void process_event_starting(DATOS_APLICACION *datosApp) {


	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_STARTING", INFOTRAZA);

	set_status_application(datosApp, EVENT_STARTING);

}

void process_event_mqtt_subscribed(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR"Procesando evento EVENT_MQTT_SUBSCRIBED", INFOTRAZA);




}



void receive_event(DATOS_APLICACION *datosApp, EVENT_APP event) {


	ESP_LOGI(TAG, ""TRAZAR" task event : EVENTO RECIBIDO:%s, estado de la aplicacion: %s", INFOTRAZA, event2mnemonic(event.event_app),
			status2mnemonic(get_current_status_application(datosApp)));

	if (event.event_app == EVENT_LOCAL_DEVICE) {
		received_local_event(datosApp, event.event_device);

	} else {
		switch (event.event_app) {

		case EVENT_ERROR_REMOTE_DEVICE:
			send_alarm(datosApp, ALARM_REMOTE_DEVICE, ALARM_ON, true);
			appuser_notify_error_remote_device(datosApp);
			break;
		case EVENT_ERROR_DEVICE:
			process_event_error_device(datosApp);
			break;
		case EVENT_ERROR_APP:
			process_event_error_app(datosApp);

			break;
		case EVENT_ERROR_NVS:

			if (datosApp->datosGenerales->estadoApp == STARTING) {
				datosApp->alarmas[ALARM_NVS].estado_alarma = ALARM_ON;

			} else {
				send_alarm(datosApp, ALARM_NVS, ALARM_ON, true);
			}

			break;
		case EVENT_ERROR_LCD:
			ESP_LOGE(TAG, ""TRAZAR"RECIBIDO ERROR LCD", INFOTRAZA);
			send_alarm(datosApp, ALARM_LCD, ALARM_ON, true);
			break;
		case EVENT_ERROR_NTP:
			process_event_error_ntp(datosApp);
			break;
		case EVENT_ERROR_WIFI:
			process_event_error_wifi(datosApp);
			break;
		case EVENT_ERROR_MQTT:
			process_event_error_mqtt(datosApp);

			break;
		case EVENT_DEVICE_READY:
			process_event_device_ready(datosApp);
			break;
		case EVENT_APP_OK:
			ESP_LOGE(TAG, ""TRAZAR"RECIBIDO APP OK", INFOTRAZA);
			process_event_app_ok(datosApp);
			break;
		case EVENT_NVS_OK:
			process_event_nvs_ok(datosApp);

			break;
		case EVENT_LCD_OK:
			ESP_LOGE(TAG, ""TRAZAR"RECIBIDO LCD OK", INFOTRAZA);
			break;
		case EVENT_GET_NTP:
			appuser_notify_get_date_sntp(datosApp);
			break;

		case EVENT_NTP_OK:
			process_event_ntp_ok(datosApp);
			break;
		case EVENT_WIFI_OK:
			process_event_wifi_ok(datosApp);
			break;
		case EVENT_MQTT_OK:
			process_event_mqtt_ok(datosApp);
			break;
		case EVENT_CONNECT_MQTT:
			appuser_notify_connecting_broker_mqtt(datosApp);
			break;

		case EVENT_CHECK_SCHEDULES:

			break;
		case EVENT_INSERT_SCHEDULE:
			process_event_insert_schedule(datosApp);
			break;
		case EVENT_MODIFY_SCHEDULE:
			process_event_modify_schedule(datosApp);
			break;
		case EVENT_DELETE_SCHEDULE:
			process_event_delete_schedule(datosApp);
			break;
		case EVENT_START_SCHEDULE:
			process_event_start_schedule(datosApp);
			break;
		case EVENT_END_SCHEDULE:
			process_event_end_schedule(datosApp);
			break;
		case EVENT_NO_ACTIVE_SCHEDULE:
			ESP_LOGW(TAG, ""TRAZAR" VAMOS A PROCESAR EL EVENT_NO_ACTIVE_SCHEDULE", INFOTRAZA);
			process_event_no_active_schedule(datosApp);
			break;
		case EVENT_FACTORY:
			process_event_factory(datosApp);
			break;
		case EVENT_UPGRADE_FIRMWARE:
			process_event_upgrade_firmware(datosApp);
			break;
		case EVENT_END_UPGRADE:
			process_event_end_upgrade(datosApp);
			break;
		case EVENT_ERROR_UPGRADE:
			process_event_error_upgrade(datosApp);
			break;

		case EVENT_REMOTE_DEVICE_OK:
			if (get_status_alarm(datosApp, ALARM_REMOTE_DEVICE)) {
				send_alarm(datosApp, ALARM_REMOTE_DEVICE, ALARM_OFF, true);
				send_event(__func__,EVENT_DEVICE_READY);
			}

			break;

		case EVENT_SMARTCONFIG_START:
			process_event_smartconfig_start(datosApp);
			break;

		case EVENT_SMARTCONFIG_END:
			process_event_smartconfig_end(datosApp);
			break;
		case EVENT_START_APP:
			process_event_start_app(datosApp);
			break;

		case EVENT_ERROR_SMARTCONFIG:
			process_event_error_smartconfig(datosApp);
			break;

		case EVENT_SCHEDULES_OK:
			process_event_schedules_ok(datosApp);
			break;

		case EVENT_NO_SCHEDULE:
			process_event_no_schedule(datosApp);
			break;

		case EVENT_AUTOMAN:
			process_event_automan(datosApp);
			break;

		case EVENT_RESTART:
			process_event_restart(datosApp);
			break;

		case EVENT_STARTING:
			process_event_starting(datosApp);
			break;

		case EVENT_MQTT_SUBSCRIBED:
			process_event_mqtt_subscribed(datosApp);
			break;
		default:
			ESP_LOGE(TAG, ""TRAZAR"ERROR EN LA RECEPCION DEL EVENTO", INFOTRAZA);
			break;

		}
	}


}

void event_task(void *arg) {

	EVENT_APP event;

	DATOS_APLICACION *datosApp = (DATOS_APLICACION* ) arg;
	event_queue = xQueueCreate(10, sizeof(EVENT_APP));

	for(;;) {
		 ESP_LOGI(TAG, ""TRAZAR"ESPERANDO EVENTO...Memoria libre: "CONFIG_UINT32_FORMAT"\n", INFOTRAZA, esp_get_free_heap_size());
		if (xQueueReceive(event_queue, &event, portMAX_DELAY) == pdTRUE) {
			//ESP_LOGE(TAG, ""TRAZAR"event_task:Recibido evento app %s, evento device:%s. Estado App: %s", INFOTRAZA,
					//event2mnemonic(event.event_app), local_event_2_mnemonic(event.event_device),
							//status2mnemonic(datosApp->datosGenerales->estadoApp));
			receive_event(datosApp, event);


		} else {
			ESP_LOGE(TAG, ""TRAZAR"NO SE HA PODIDO PROCESAR LA PETICION", INFOTRAZA);

		}

	}
	vTaskDelete(NULL);


}

void create_event_task(DATOS_APLICACION *datosApp) {


	init_alarms(datosApp);
	xTaskCreate(event_task, "event_task", CONFIG_RESOURCE_EVENT_TASK, (void*) datosApp, 4, NULL);
	ESP_LOGW(TAG, ""TRAZAR"TAREA DE EVENTOS CREADA", INFOTRAZA);


}

void send_event(const char *func, EVENT_TYPE event) {

	EVENT_APP event_received;

	event_received.event_app = event;
	event_received.event_device = EVENT_NONE;

	//ESP_LOGW(TAG, ""TRAZAR" envio de evento: funcion: %s: evento: %s", INFOTRAZA, func, event2mnemonic(event));
	if ( xQueueSend(event_queue, &event_received,( TickType_t ) 100) != pdPASS) {
		ESP_LOGE(TAG, ""TRAZAR"no se ha podido enviar el evento %s", INFOTRAZA, event2mnemonic(event));
		esp_restart();
	}

}


ESTADO_APP get_current_status_application(DATOS_APLICACION *datosApp) {

	if (datosApp != NULL) {
		//ESP_LOGI(TAG, ""TRAZAR" Se devuelve el estado: %s", INFOTRAZA, status2mnemonic(datosApp->datosGenerales->estadoApp));
		return datosApp->datosGenerales->estadoApp;
	} else {
		ESP_LOGE(TAG, ""TRAZAR" Se devuelve UNKNOWN_STATUS porque datosApp es nulo", INFOTRAZA);
		return UNKNOWN_STATUS;
	}


}




void send_event_device(const char *func, EVENT_DEVICE event) {



#ifndef CONFIG_IDF_TARGET_ESP8266
	ESP_LOGW(TAG, ""TRAZAR"RECIBIDO EVENTO LOCAL %s: %s", INFOTRAZA, func, local_event_2_mnemonic(event));
#endif

	EVENT_APP event_received;
	event_received.event_app = EVENT_LOCAL_DEVICE;
	event_received.event_device = event;
	xQueueSend(event_queue, &event_received,0);


}








