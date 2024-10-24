/*
 * datos_comunes.c
 *
 *  Created on: 2 nov. 2020
 *      Author: t126401
 */


#include "logging.h"
#include <stdio.h>
#include <stdarg.h>
#include "esp_log.h"

char* pintar_fecha() {

	static char fecha_actual[120] = {0};

	time_t now;
	struct tm fecha;
	//ESP_LOGI(TAG, ""TRAZAR"ACTUALIZAR_HORA", INFOTRAZA);
    time(&now);
    localtime_r(&now, &fecha);


	sprintf(fecha_actual, "%02d/%02d/%04d %02d:%02d:%02d",
			fecha.tm_mday,
    		fecha.tm_mon + 1,
			fecha.tm_year + 1900,
			fecha.tm_hour,
			fecha.tm_min,
			fecha.tm_sec);

	return fecha_actual;

}


char* status2mnemonic(ESTADO_APP status) {

	static char mnemonic[30] = {0};

	switch (status) {

	case ERROR_APP:
		strcpy(mnemonic, "ERROR_APP");
		break;

	case NORMAL_AUTO:
		strcpy(mnemonic, "NORMAL_AUTO");
		break;
	case NORMAL_AUTOMAN:
		strcpy(mnemonic, "NORMAL_AUTOMAN");
		break;
	case NORMAL_MANUAL:
		strcpy(mnemonic, "NORMAL_MANUAL");
		break;
	case STARTING:
		strcpy(mnemonic, "STARTING");
		break;
		/*
	case NO_PROGRAM:
		strcpy(mnemonic, "NORMAL_SIN_PROGRAMACION");
		break;
		*/
	case UPGRADING:
		strcpy(mnemonic, "UPGRADE_EN_PROGRESO");
		break;

	case WAITING_END_STARTING:
		strcpy(mnemonic, "ESPERA_FIN_ARRANQUE");
		break;
	case FACTORY:
		strcpy(mnemonic, "FACTORY");
		break;
		/*
	case NORMAL_FIN_PROGRAMA_ACTIVO:
		strcpy(mnemonic, "NORMAL_FIN_PROGRAMA_ACTIVO");
		break;
		*/
	case CHECK_SCHEDULES:
		strcpy(mnemonic, "CHECK_SCHEDULES");
		break;
	case SCHEDULING:
		strcpy(mnemonic, "SCHEDULING");
		break;
	case RESTARTING:
		strcpy(mnemonic, "RESTARTING");
		break;
	case DEVICE_READY:
		strcpy(mnemonic, "DEVICE_READY");
		break;
	case UNKNOWN_STATUS:
		strcpy(mnemonic, "UNKNOWN_STATUS");
		break;
	case RECOVERING:
		strcpy(mnemonic, "RECOVERING");




	}
	return mnemonic;
}
