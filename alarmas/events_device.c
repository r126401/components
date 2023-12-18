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
#include "interfaz_usuario.h"
#include "dialogos_json.h"
#include "conexiones_mqtt.h"





static const char *TAG = "EVENTS";
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
	case EVENT_DEVICE_OK:
		strcpy(mnemonic, "EVENT_DEVICE_OK");
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

	case EVENT_CHECK_PROGRAMS:
		strcpy(mnemonic, "EVENT_CHECK_PROGRAMS");
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

	}



	return mnemonic;

}




void process_event_warning_device(DATOS_APLICACION *datosApp) {

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
	case NO_PROGRAM:
		esp_restart();
		break;
	case UPGRADING:
		break;
	case SYNCRONIZING:
		break;
	case WAITING_END_STARTING:
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
	case SCHEDULING:
		break;
	case RESTARTING:
		break;


	}


}

void process_event_nvs_ok(DATOS_APLICACION *datosApp) {


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

	switch(datosApp->datosGenerales->estadoApp) {

	case FACTORY:
		change_status_application(datosApp, STARTING);
		break;

	default:

		break;


	}

	send_alarm(datosApp, ALARM_WIFI, ALARM_OFF, true);
	sync_app_by_ntp(datosApp);
	appuser_notify_wifi_connected_ok(datosApp);
	if (datosApp->handle_mqtt == NULL) {
		ESP_LOGI(TAG, ""TRAZAR"INICIAMOS LA TAREA MQTT PORQUE NO EXISTE", INFOTRAZA);
		iniciar_gestion_programacion(datosApp);
		crear_tarea_mqtt(datosApp);

	} else {
		ESP_LOGW(TAG, ""TRAZAR"NO INICIAMOS LA TAREA MQTT PORQUE YA EXISTE", INFOTRAZA);
	}

}

void process_event_error_wifi(DATOS_APLICACION *datosApp) {

	ESP_LOGE(TAG, ""TRAZAR" process_event_error_wifi: eeeeeeeeeeeeeeeeeeeeeeeeeeeeeee", INFOTRAZA);

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

	switch(datosApp->datosGenerales->estadoApp) {

	case STARTING:
		send_alarm(datosApp, ALARM_NTP, ALARM_OFF, true);
		actualizar_hora(&datosApp->datosGenerales->clock);
		datosApp->datosGenerales->estadoProgramacion = VALID_PROG;
		break;

	default:
		send_alarm(datosApp, ALARM_NTP, ALARM_OFF, true);
		actualizar_hora(&datosApp->datosGenerales->clock);
		datosApp->datosGenerales->estadoProgramacion = VALID_PROG;
		change_status_application(datosApp, CHECK_PROGRAMS);
		break;


	}
	appuser_notify_sntp_ok(datosApp);
}

void process_event_error_ntp(DATOS_APLICACION *datosApp) {


	switch(datosApp->datosGenerales->estadoApp) {

	case NORMAL_AUTO:
	case NORMAL_AUTOMAN:
		change_status_application(datosApp, DEVICE_ALONE);
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


void process_event_none_schedule(DATOS_APLICACION *datosApp) {


	if (datosApp->datosGenerales->nProgramacion == 0) {
		change_status_application(datosApp, NORMAL_AUTO);
	} else {
		change_status_application(datosApp, NORMAL_AUTO);
	}



	appuser_notify_event_none_schedule(datosApp);




}

void process_event_end_schedule(DATOS_APLICACION *datosApp) {

	change_status_application(datosApp, CHECK_PROGRAMS);
	appuser_end_schedule(datosApp);


}

void process_event_start_schedule(DATOS_APLICACION *datosApp) {


	switch(datosApp->datosGenerales->estadoApp) {

	case NORMAL_AUTO:
	case NORMAL_AUTOMAN:
	case SCHEDULING:
		change_status_application(datosApp, SCHEDULING);
		appuser_start_schedule(datosApp);

	break;

	case CHECK_PROGRAMS:
		change_status_application(datosApp, SCHEDULING);
		start_schedule(datosApp);
		break;


	default:
		break;

	}


}

void process_event_check_programs(DATOS_APLICACION *datosApp) {
	change_status_application(datosApp, CHECK_PROGRAMS);
}








void process_event_app_ok(DATOS_APLICACION *datosApp) {





}

void process_event_mqtt_ok(DATOS_APLICACION *datosApp) {


	send_alarm(datosApp, ALARM_MQTT, ALARM_OFF, true);
	appuser_notify_broker_connected_ok(datosApp);



	switch(datosApp->datosGenerales->estadoApp) {

	case STARTING:
		change_status_application(datosApp, CHECK_PROGRAMS);
		appuser_notify_application_started(datosApp);
		break;

	default:
		break;

	}



}


void process_event_modify_schedule(DATOS_APLICACION *datosApp) {


	change_status_application(datosApp, CHECK_PROGRAMS);

}

void process_event_delete_schedule(DATOS_APLICACION *datosApp) {

	change_status_application(datosApp, CHECK_PROGRAMS);
}


void process_event_insert_schedule(DATOS_APLICACION *datosApp) {

	change_status_application(datosApp, CHECK_PROGRAMS);
}


void process_event_upgrade_firmware(DATOS_APLICACION *datosApp) {

	change_status_application(datosApp, UPGRADING);
	appuser_notify_start_ota(datosApp);
	send_spontaneous_report(datosApp, START_UPGRADE_OTA);
}


void process_event_factory(DATOS_APLICACION *datosApp) {

	change_status_application(datosApp, FACTORY);
	//send_event(__func__, EVENT_SMARTCONFIG);
	appuser_notify_no_config(datosApp);


}

void process_event_device_ok(DATOS_APLICACION *datosApp) {

	switch (get_current_status_application(datosApp)) {

	case FACTORY:
		ESP_LOGW(TAG, ""TRAZAR", NO SE CAMBIA EL ESTADO PORQUE ESTAMOS EN FACTORY", INFOTRAZA);
		break;


	default:
		if (datosApp->alarmas[ALARM_DEVICE].estado_alarma == ALARM_ON) {
			send_alarm(datosApp, ALARM_DEVICE, ALARM_OFF, true);
			if (datosApp->alarmas[ALARM_NTP].estado_alarma == ALARM_OFF) {
				change_status_application(datosApp, CHECK_PROGRAMS);
			} else {
				change_status_application(datosApp,NORMAL_MANUAL);
			}
			appuser_notify_device_ok(datosApp);


		} else {
			ESP_LOGI(TAG, ""TRAZAR"EL DISPOSITIVO YA ESTABA OK", INFOTRAZA);
		}
		break;


	}
}


void process_event_smartconfig_start(DATOS_APLICACION *datosApp) {


	switch (get_current_status_application(datosApp)) {

	case FACTORY:
		ESP_LOGI(TAG, ""TRAZAR"SMARTCONFIG START EN MODO FACTORY", INFOTRAZA);
		conectar_dispositivo_wifi();
		break;
	default:
		ESP_LOGI(TAG, ""TRAZAR" RECIBIDO EVENTO SMARTCONFIG EN ESTADO %s", INFOTRAZA, status2mnemonic(get_current_status_application(datosApp)));
		change_status_application(datosApp, FACTORY);
		reinicio_fabrica(datosApp);
		task_smartconfig();
		break;


	}


}

void process_event_smartconfig_end(DATOS_APLICACION *datosApp) {

	change_status_application(datosApp, STARTING);
	appuser_notify_smartconfig_end(datosApp);


}

void process_event_error_smartconfig(DATOS_APLICACION *datosApp) {

	appuser_notify_error_smartconfig(datosApp);
}

void process_event_end_upgrade(DATOS_APLICACION *datosApp) {

	//notificar_evento_ota(datosApp, OTA_UPGRADE_FINALIZADO);
	send_spontaneous_report(datosApp, END_UPGRADE_OTA);
	esp_restart();

}


void process_event_error_upgrade(DATOS_APLICACION *datosApp) {

	send_spontaneous_report(datosApp, ERROR_UPGRADE_OTA);
	esp_restart();

}

void process_event_error_mqtt(DATOS_APLICACION *datosApp) {

	send_alarm(datosApp, ALARM_MQTT, ALARM_ON, false);
	appuser_notify_broker_disconnected(datosApp);

}

void process_event_error_device(DATOS_APLICACION *datosApp) {

	if (datosApp->alarmas[ALARM_DEVICE].estado_alarma == ALARM_OFF) {
		send_alarm(datosApp, ALARM_DEVICE, ALARM_ON, true);
	}
	appuser_notify_error_device(datosApp);
	change_status_application(datosApp, ERROR_APP);


}



void receive_event(DATOS_APLICACION *datosApp, EVENT_APP event) {

	ESP_LOGE(TAG, ""TRAZAR"receive_event", INFOTRAZA);

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
			ESP_LOGE(TAG, ""TRAZAR"RECIBIDO ERROR APP", INFOTRAZA);
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
		case EVENT_DEVICE_OK:
			process_event_device_ok(datosApp);
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
			appuser_get_date_sntp(datosApp);
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

		case EVENT_CHECK_PROGRAMS:

			break;
		case EVENT_INSERT_SCHEDULE:
			process_event_insert_schedule(datosApp);
			break;
		case EVENT_MODIFY_SCHEDULE:
			process_event_modify_schedule(datosApp);
			break;
		case EVENT_DELETE_SCEDULE:
			process_event_delete_schedule(datosApp);
			break;
		case EVENT_START_SCHEDULE:
			process_event_start_schedule(datosApp);
			break;
		case EVENT_END_SCHEDULE:
			process_event_end_schedule(datosApp);
			break;
		case EVENT_NONE_SCHEDULE:
			process_event_none_schedule(datosApp);
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
				send_event(__func__,EVENT_DEVICE_OK);
			}

			break;

		case EVENT_SMARTCONFIG_START:
			process_event_smartconfig_start(datosApp);
			break;

		case EVENT_SMARTCONFIG_END:
			process_event_smartconfig_end(datosApp);
			break;
		case EVENT_START_APP:
			break;

		case EVENT_ERROR_SMARTCONFIG:
			process_event_error_smartconfig(datosApp);
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
		 ESP_LOGI(TAG, ""TRAZAR"ESPERANDO EVENTO...Memoria libre: "CONFIG_UINT32_ESPLOG_FORMAT"\n", INFOTRAZA, esp_get_free_heap_size());
		if (xQueueReceive(event_queue, &event, portMAX_DELAY) == pdTRUE) {
			ESP_LOGE(TAG, ""TRAZAR"event_task:Recibido evento app %s, evento device:%s. Estado App: %s", INFOTRAZA,
					event2mnemonic(event.event_app), local_event_2_mnemonic(event.event_device),
							status2mnemonic(datosApp->datosGenerales->estadoApp));
			receive_event(datosApp, event);


		} else {
			ESP_LOGE(TAG, ""TRAZAR"NO SE HA PODIDO PROCESAR LA PETICION", INFOTRAZA);
		}

	}
	vTaskDelete(NULL);


}

void create_event_task(DATOS_APLICACION *datosApp) {


	init_alarms(datosApp);
	xTaskCreate(event_task, "event_task", 4096, (void*) datosApp, 4, NULL);
	ESP_LOGW(TAG, ""TRAZAR"TAREA DE EVENTOS CREADA", INFOTRAZA);
	change_status_application(datosApp, STARTING);

}

void send_event(const char *func, EVENT_TYPE event) {

	EVENT_APP event_received;

	event_received.event_app = event;
	event_received.event_device = EVENT_NONE;

	ESP_LOGW(TAG, ""TRAZAR" %s: %s", INFOTRAZA, func, event2mnemonic(event));
	xQueueSend(event_queue, &event_received,0);

}


ESTADO_APP get_current_status_application(DATOS_APLICACION *datosApp) {

	return datosApp->datosGenerales->estadoApp;
}




void send_event_device(const char *func, EVENT_DEVICE event) {



	ESP_LOGW(TAG, ""TRAZAR"RECIBIDO EVENTO LOCAL %s: %s", INFOTRAZA, func, local_event_2_mnemonic(event));
	EVENT_APP event_received;
	event_received.event_app = EVENT_LOCAL_DEVICE;
	event_received.event_device = event;
	xQueueSend(event_queue, &event_received,0);


}








