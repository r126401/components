/*
 * applib.c
 *
 *  Created on: 10 jun 2024
 *      Author: esp8266
 */


#include "common_data.h"
#include "esp_log.h"
#include "logging.h"

static const char *TAG = "applib.c";

enum CONFIGURE_DEVICE get_app_status_device(DATOS_APLICACION *datosApp) {

	return datosApp->datosGenerales->status;

}

void set_app_status_device(DATOS_APLICACION *datosApp, enum CONFIGURE_DEVICE status) {

	datosApp->datosGenerales->status = status;
	ESP_LOGI(TAG, ""TRAZAR", Estado del dispositivo cambiado a %d", INFOTRAZA, status);

}
