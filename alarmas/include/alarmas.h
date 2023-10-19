/*
 * alarmas.h
 *
 *  Created on: 10 oct. 2020
 *      Author: t126401
 */

#include "stdint.h"
#include "sys/types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "configuracion_usuario.h"





#ifndef COMPONENTS_ALARMAS_INCLUDE_ALARMAS_H_
#define COMPONENTS_ALARMAS_INCLUDE_ALARMAS_H_






//esp_err_t registrar_alarma(DATOS_APLICACION *datosApp, char* nemonico_alarma, ALARM_TYPE tipo_alarma, enum ESTADO_ALARMA estado_alarma, bool flag_envio);
//esp_err_t inicializacion_registros_alarmas(DATOS_APLICACION *datosApp);
//static esp_err_t registrar_alarma(DATOS_APLICACION *datosApp, char* mnemonico_alarma, ALARM_TYPE tipo_alarma, enum ESTADO_ALARMA estado_alarma, bool flag_envio);
//void send_event_application(DATOS_APLICACION *datosApp, char* mnemonico_alarma, EVENT_TYPE event);
void send_event(EVENT_TYPE event);
void send_event_device(EVENT_DEVICE event);
void create_event_task(DATOS_APLICACION *datosApp);
ESTADO_APP current_status_application(DATOS_APLICACION *datosApp);



void process_event_warning_device(DATOS_APLICACION *datosApp);
void process_event_error_device(DATOS_APLICACION *datosApp);
void process_event_error_app(DATOS_APLICACION *datosApp);
void process_event_error_nvs(DATOS_APLICACION *datosApp);
void process_event_nvs_ok(DATOS_APLICACION *datosApp);


#endif /* COMPONENTS_ALARMAS_INCLUDE_ALARMAS_H_ */
