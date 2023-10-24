/*
 * local_events.c
 *
 *  Created on: Oct 23, 2023
 *      Author: t126401
 */


#include "local_events_device.h"
#include "common_data.h"
#include "esp_timer.h"
#include "alarmas.h"
#include "interfaz_usuario.h"
#include "esp_err.h"
#include "logging.h"



static const char *TAG = "LOCAL_EVENTS_DEVICE";

#define TIMEOUT_REQUEST_REMOTE_TEMPERATURE 5
#define NUM_FAILS 5
static esp_timer_handle_t timer_request_remote_temperature;
const esp_timer_create_args_t timer_remote_read_args;


void event_handler_request_remote_temperature(void *arg) {


	send_event_device(EVENT_TIMEOUT_REMOTE_TEMPERATURE);


}


void process_local_event_request_temperature(DATOS_APLICACION *datosApp) {


    const esp_timer_create_args_t timer_remote_read_args = {
    		.callback = &event_handler_request_remote_temperature,
			.name = "timer remote read",
			.arg = (void*) datosApp
    };


	switch (get_current_status_application(datosApp)) {

	default:
	    ESP_ERROR_CHECK(esp_timer_create(&timer_remote_read_args, &timer_request_remote_temperature));
	    ESP_ERROR_CHECK(esp_timer_start_once(timer_request_remote_temperature, TIMEOUT_REQUEST_REMOTE_TEMPERATURE * 1000000));
		break;

	}


}



void process_local_event_answer_temperature(DATOS_APLICACION *datosApp) {


	ESP_LOGI(TAG, ""TRAZAR"process_local_event_answer_temperature", INFOTRAZA);


	if (esp_timer_is_active(timer_request_remote_temperature)) {
		ESP_LOGI(TAG, ""TRAZAR"process_local_event_answer_temperature: se cancela temporizador", INFOTRAZA);
		//esp_timer_stop(timer_request_remote_temperature);
		esp_timer_delete(timer_request_remote_temperature);
	} else {
		ESP_LOGW(TAG, ""TRAZAR"process_local_event_answer_temperature:  No se cancela temporizador de lectura remota", INFOTRAZA);
	}





}


void process_local_event_timeout_remote_temperature(DATOS_APLICACION *datosApp) {

	static int error_counter = 0;

	if (error_counter == NUM_FAILS) {

	} else if (error_counter > NUM_FAILS) {

	} else {

	}




}





void received_local_event(DATOS_APLICACION *datosApp, EVENT_DEVICE event) {

	switch (event) {

	case EVENT_REQUEST_TEMPERATURE:
		process_local_event_request_temperature(datosApp);
		break;
	case EVENT_ANSWER_TEMPERATURE:
		process_local_event_answer_temperature(datosApp);
		break;
	case EVENT_TIMEOUT_REMOTE_TEMPERATURE:
		process_local_event_timeout_remote_temperature(datosApp);
		break;

	case EVENT_DEVICE_REMOTE_ERROR:
		break;
	case EVENT_UP_THRESHOLD:
		appuser_received_local_event(datosApp, event);
		break;
	case EVENT_DOWN_THRESHOLD:
		appuser_received_local_event(datosApp, event);
		break;

	default:
		break;

	}




}