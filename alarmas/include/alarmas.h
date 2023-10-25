/*
 * alarmas.h
 *
 *  Created on: 10 oct. 2020
 *      Author: t126401
 */







#ifndef COMPONENTS_ALARMAS_INCLUDE_ALARMAS_H_
#define COMPONENTS_ALARMAS_INCLUDE_ALARMAS_H_


#include "stdint.h"
#include "sys/types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "configuracion_usuario.h"
#include "local_events_device.h"



//esp_err_t registrar_alarma(DATOS_APLICACION *datosApp, char* nemonico_alarma, ALARM_TYPE tipo_alarma, enum ESTADO_ALARMA estado_alarma, bool flag_envio);
//esp_err_t inicializacion_registros_alarmas(DATOS_APLICACION *datosApp);
//static esp_err_t registrar_alarma(DATOS_APLICACION *datosApp, char* mnemonico_alarma, ALARM_TYPE tipo_alarma, enum ESTADO_ALARMA estado_alarma, bool flag_envio);
//void send_event_application(DATOS_APLICACION *datosApp, char* mnemonico_alarma, EVENT_TYPE event);


void init_alarms(DATOS_APLICACION *datosApp);
esp_err_t send_alarm(DATOS_APLICACION *datosApp, ALARM_TYPE tipo_alarma, enum ALARM_STATUS estado_alarma, bool flag_envio);
ALARM_STATUS get_status_alarm(DATOS_APLICACION *datosApp, ALARM_TYPE alarm);


#endif /* COMPONENTS_ALARMAS_INCLUDE_ALARMAS_H_ */
