/*
 * applib.c
 *
 *  Created on: 10 jun 2024
 *      Author: esp8266
 */


#include "common_data.h"
#include "esp_log.h"
#include "logging.h"
#include "events_device.h"

static const char *TAG = "applib.c";


void init_device(DATOS_APLICACION *datosApp) {


	DATOS_GENERALES *datosGenerales;
	datosGenerales = (DATOS_GENERALES*) calloc(1, sizeof(DATOS_GENERALES));
	datosApp->datosGenerales = datosGenerales;
	create_event_task(datosApp);


}

enum CONFIGURE_DEVICE get_app_status_device(DATOS_APLICACION *datosApp) {

	return datosApp->datosGenerales->status;

}

void set_app_status_device(DATOS_APLICACION *datosApp, enum CONFIGURE_DEVICE status) {

	datosApp->datosGenerales->status = status;
	ESP_LOGI(TAG, ""TRAZAR", Estado del dispositivo cambiado a %d", INFOTRAZA, status);

}

char* get_app_publish_topic(DATOS_APLICACION *datosApp, uint8_t index) {

	ESP_LOGI(TAG, ""TRAZAR" El topic solicitado es :%s", INFOTRAZA, datosApp->datosGenerales->parametrosMqtt.topics[index].publish);
	return datosApp->datosGenerales->parametrosMqtt.topics[index].publish;

}


char* get_app_subscribe_topic(DATOS_APLICACION *datosApp, uint8_t index) {

	return datosApp->datosGenerales->parametrosMqtt.topics[index].subscribe;

}

void set_app_publish_topic(DATOS_APLICACION *datosApp, uint8_t index, char* topic) {


}

void set_app_subscribe_topic(DATOS_APLICACION *datosApp, uint8_t index, char* topic) {


}

void set_app_config_wifi(DATOS_APLICACION *datosApp, bool flag) {

	datosApp->wifi = flag;


}

void set_app_config_mqtt(DATOS_APLICACION *datosApp, bool flag) {

	datosApp->mqtt = flag;

}

void set_app_config_timing(DATOS_APLICACION *datosApp, bool flag) {

	datosApp->timing = flag;

}

void set_app_config_manage_schedules(DATOS_APLICACION *datosApp, bool flag) {

	datosApp->schedules = flag;

}

bool get_app_config_wifi(DATOS_APLICACION *datosApp) {

	return datosApp->wifi;
}
bool get_app_config_mqtt(DATOS_APLICACION *datosApp) {

	return datosApp->mqtt;
}

bool get_app_config_timing(DATOS_APLICACION *datosApp) {

	return datosApp->timing;
}

bool get_app_config_manage_schedules(DATOS_APLICACION *datosApp) {

	return datosApp->schedules;
}


