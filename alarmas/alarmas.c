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
#include "conexiones.h"
#include "esp_timer.h"
#include "local_events_device.h"
#include "dialogos_json.h"



static const char *TAG = "ALARMAS";

#define TIMEOUT_REQUEST_REMOTE_TEMPERATURE 5


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

esp_err_t send_alarm(DATOS_APLICACION *datosApp, ALARM_TYPE tipo_alarma, enum ALARM_STATUS estado_alarma, bool flag_envio) {

	//Registramos la alarma.


	ESP_LOGE(TAG, ""TRAZAR"registrar_alarma", INFOTRAZA);
	datosApp->alarmas[tipo_alarma].estado_alarma = estado_alarma;
	datosApp->alarmas[tipo_alarma].fecha_alarma = datosApp->datosGenerales->clock.time;
	if (flag_envio == true) {
		notificar_evento_alarma(datosApp, tipo_alarma, datosApp->alarmas[tipo_alarma].nemonico);
	}


	ESP_LOGE(TAG, ""TRAZAR" ALARMA  %s ESTADO %s REGISTRADA", INFOTRAZA, datosApp->alarmas[tipo_alarma].nemonico, status_alarm_to_mnemonic(estado_alarma));


	if (datosApp->alarmas[tipo_alarma].estado_alarma == ALARM_ON) {

		appuser_notify_alarm_on_device(datosApp, datosApp->alarmas[tipo_alarma].estado_alarma);
	} else {
		appuser_notify_alarm_off_device(datosApp, datosApp->alarmas[tipo_alarma].estado_alarma);
	}


	//appuser_notify_error_device(datosApp, tipo_alarma);
	return ESP_OK;
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
		strncpy(datosApp->alarmas[ALARM_REMOTE_DEVICE].nemonico, MNEMONIC_ALARM_REMOTE_DEVICE, 50);
		ESP_LOGI(TAG, ""TRAZAR" INICIALIZADAS ALARMAS, TIPO_ALARMA: %s, ESTADO: %s, FECHA: %llu", INFOTRAZA,
				datosApp->alarmas[i].nemonico,
				status_alarm_to_mnemonic(datosApp->alarmas[i].estado_alarma),
				datosApp->alarmas[i].fecha_alarma);
	}

}

ALARM_STATUS get_status_alarm(DATOS_APLICACION *datosApp, ALARM_TYPE alarm) {



	return datosApp->alarmas[alarm].estado_alarma;

}





