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



static const char *TAG = "ALARMAS";




esp_err_t registrar_alarma(DATOS_APLICACION *datosApp, char* mnemonico_alarma, uint8_t tipo_alarma, enum ESTADO_ALARMA estado_alarma, bool flag_envio) {

	//Registramos la alarma.
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


