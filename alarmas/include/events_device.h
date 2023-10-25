/*
 * events_device.h
 *
 *  Created on: Oct 25, 2023
 *      Author: t126401
 */

#ifndef COMPONENTS_ALARMAS_INCLUDE_EVENTS_DEVICE_H_
#define COMPONENTS_ALARMAS_INCLUDE_EVENTS_DEVICE_H_

#include "common_data.h"

ESTADO_APP get_current_status_application(DATOS_APLICACION *datosApp);

void received_local_event(DATOS_APLICACION *datosApp, EVENT_DEVICE event);


void process_event_warning_device(DATOS_APLICACION *datosApp);
void process_event_error_device(DATOS_APLICACION *datosApp);
void process_event_error_app(DATOS_APLICACION *datosApp);
void process_event_error_nvs(DATOS_APLICACION *datosApp);
void process_event_nvs_ok(DATOS_APLICACION *datosApp);
void send_event(EVENT_TYPE event);
void send_event_device(EVENT_DEVICE event);
void create_event_task(DATOS_APLICACION *datosApp);
char* event2mnemonic(EVENT_TYPE event);


#endif /* COMPONENTS_ALARMAS_INCLUDE_EVENTS_DEVICE_H_ */
