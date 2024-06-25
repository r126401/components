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
#include "configuracion.h"
#include "conexiones_mqtt.h"
#include "conexiones.h"
#include "applib.h"
#include "user_interface.h"
#include "alarmas.h"

static const char *TAG = "applib.c";


esp_err_t init_data_app(DATOS_APLICACION *datosApp) {

	DATOS_GENERALES *datosGenerales;
	datosGenerales = (DATOS_GENERALES*) calloc(1, sizeof(DATOS_GENERALES));
	datosApp->datosGenerales = datosGenerales;
	change_status_application(datosApp, UNKNOWN_STATUS);

	return ESP_OK;
}


esp_err_t init_device(DATOS_APLICACION *datosApp) {

	esp_err_t error;


	create_event_task(datosApp);
	send_event(__func__, EVENT_STARTING);
	error = init_global_parameters(datosApp);
	if (error == ESP_OK) {
		ESP_LOGI(TAG, ""TRAZAR"INICIALIZACION CORRECTA", INFOTRAZA);
	} else {

	}

	return error;


}


void config_default_services_device(DATOS_APLICACION *datosApp) {

#ifdef CONFIG_WIFI_ACTIVE

	set_app_config_wifi(datosApp, true);
#else
	set_app_config_wifi(datosApp, false);
#endif

#ifdef CONFIG_MQTT_ACTIVE

	set_app_config_mqtt(datosApp, true);
#else
	set_app_config_mqtt(datosApp, false);

#endif

#ifdef CONFIG_CLOCK_ACTIVE

	set_app_config_timing(datosApp, true);
#else
	set_app_config_timing(datosApp, false);
#endif

#ifdef CONFIG_SCHEDULE_ACTIVE

	set_app_config_manage_schedules(datosApp, true);
#else
	set_app_config_manage_schedules(datosApp, false);
#endif


}

esp_err_t init_services_device(DATOS_APLICACION *datosApp) {

	//En primer lugar se inicia la conexion wifi

	config_default_services_device(datosApp);


	if (!get_app_config_wifi(datosApp)) {

		ESP_LOGW(TAG, ""TRAZAR"init_service_device: No se configura wifi por lo que no se configuran el resto de servicios de red", INFOTRAZA);
		return ESP_FAIL;
	}

		init_wifi_device();
		ESP_LOGW(TAG, ""TRAZAR"init_service_device: Wifi configurado y activo", INFOTRAZA);

		//2.- Se inicia la tarea mqtt si estuviera configurado.
	if (using_mqtt(datosApp)) {
		init_mqtt_service(datosApp);
		ESP_LOGW(TAG, ""TRAZAR" init_service_device: servicio mqtt configurado y activo", INFOTRAZA);
	} else {
		// Si no estuviera configurado, iniciamos la secuencia de activacion del ntp y la programcion si procede
		if (using_ntp(datosApp)) {
			init_ntp_service(datosApp);
			if (using_schedules(datosApp)) {
				init_schedule_service(datosApp);
			}
			ESP_LOGW(TAG, ""TRAZAR" VAMOS A ENVIAR EL EVENTO EVENT_START_APP", INFOTRAZA);
			send_event(__func__, EVENT_START_APP);
		}



	}


	return ESP_OK;


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
bool using_mqtt(DATOS_APLICACION *datosApp) {

	ESP_LOGW(TAG, ""TRAZAR" El valor de mqtt es %d", INFOTRAZA, datosApp->mqtt);

	return datosApp->mqtt;
}

bool using_ntp(DATOS_APLICACION *datosApp) {

	return datosApp->timing;
}

bool using_schedules(DATOS_APLICACION *datosApp) {

	return datosApp->schedules;
}


void set_new_certificate(DATOS_APLICACION *datosApp, char* text, int size) {





}

char* get_certificate(DATOS_APLICACION *datosApp) {

	static char datos[CONFIG_TAMANO_BUFFER_LECTURA];
	leer_configuracion(datosApp, CONFIG_CLAVE_CERTIFICADO_TLS, datos);
	return datos;



}


void set_default_certificate(DATOS_APLICACION *datosApp) {



	extern const uint8_t mqtt_jajica_pem_start[]   asm("_binary_mqtt_cert_crt_start");
	//extern const uint8_t mqtt_jajica_pem_end[]   asm("_binary_mqtt_cert_crt_end");
	guardar_configuracion(datosApp, CONFIG_CLAVE_CERTIFICADO_TLS, (char*) mqtt_jajica_pem_start);


}

bool get_mqtt_tls(DATOS_APLICACION *datosApp) {

	return datosApp->datosGenerales->parametrosMqtt.tls;

}

void set_mqtt_tls(DATOS_APLICACION *datosApp, bool tls) {

	datosApp->datosGenerales->parametrosMqtt.tls = tls;
}


void change_status_application(DATOS_APLICACION *datosApp, ESTADO_APP new_status) {

	ESTADO_APP current_status = get_current_status_application(datosApp);
	char *text;
	text = calloc(25, sizeof(char));
	strcpy(text, status2mnemonic(current_status));
	//ESP_LOGW(TAG, ""TRAZAR"ESTADO ANTERIOR %s", INFOTRAZA, status2mnemonic(datosApp->datosGenerales->estadoApp));
	datosApp->datosGenerales->estadoApp = new_status;
	//ESP_LOGW(TAG, ""TRAZAR"ESTADO POSTERIOR %s", INFOTRAZA, status2mnemonic(new_status));
	ESP_LOGE(TAG, ""TRAZAR" Cambiado estado: %s ------------> %s", INFOTRAZA, text, status2mnemonic(new_status));
	free(text);
	appuser_notify_app_status(datosApp, new_status);


}


void set_status_application(DATOS_APLICACION *datosApp, EVENT_TYPE event) {


	ESTADO_APP new_status = UNKNOWN_STATUS;

	if (event == EVENT_RESTART) {
		new_status = RESTARTING;
		change_status_application(datosApp, new_status);
		return;
	}

	if (event == EVENT_FACTORY) {
		new_status = FACTORY;
		change_status_application(datosApp, new_status);
		return;

	}



	switch (get_current_status_application(datosApp)) {


	case ERROR_APP:
		if (event == EVENT_DEVICE_OK) {
			//change_status_application(datosApp, APP_STARTED);
			new_status = APP_STARTED;
			break;
		}

		if (event == EVENT_SMARTCONFIG_START) {
			new_status = FACTORY;
			break;
		}

		break;


	case NORMAL_AUTO:
		if ((event == EVENT_ERROR_APP) || (event == EVENT_ERROR_DEVICE) || (event == EVENT_ERROR_LCD) || (event == EVENT_ERROR_NVS)) {
			//change_status_application(datosApp, ERROR_APP);
			new_status = ERROR_APP;
			break;
		}
		if (event == EVENT_UPGRADE_FIRMWARE) {
			//change_status_application(datosApp, UPGRADING);
			new_status = UPGRADING;

			break;
		}
		if (event == EVENT_CHECK_SCHEDULES) {
			//change_status_application(datosApp, CHECK_SCHEDULES);
			new_status = CHECK_SCHEDULES;
			break;
		}

		if (event == EVENT_NONE_SCHEDULE) {
			//change_status_application(datosApp, NORMAL_MANUAL);
			new_status = NORMAL_AUTO;
			break;
		}
		if (event == EVENT_NO_SCHEDULE) {
			//change_status_application(datosApp, NORMAL_MANUAL);
			new_status = NORMAL_MANUAL;
			break;
		}



		if (event == EVENT_AUTOMAN) {
			//change_status_application(datosApp, NORMAL_AUTOMAN);
			new_status = NORMAL_AUTOMAN;

			break;
		}
		if (event == EVENT_START_SCHEDULE) {
			//change_status_application(datosApp, CHECK_SCHEDULES);
			new_status = SCHEDULING;

			break;
		}
		if ((event == EVENT_END_SCHEDULE) || (event == EVENT_DELETE_SCHEDULE) || (event == EVENT_MODIFY_SCHEDULE) || (event == EVENT_INSERT_SCHEDULE)){
			new_status = CHECK_SCHEDULES;

			break;
		}

		if (event == EVENT_SMARTCONFIG_START) {
			new_status = FACTORY;
		}
		break;



	case NORMAL_AUTOMAN:
		if ((event == EVENT_ERROR_APP) || (event == EVENT_ERROR_DEVICE) || (event == EVENT_ERROR_LCD) || (event == EVENT_ERROR_NVS)) {
			//change_status_application(datosApp, ERROR_APP);
			new_status = ERROR_APP;

			break;
		}
		if (event == EVENT_UPGRADE_FIRMWARE) {
			//change_status_application(datosApp, UPGRADING);
			new_status = UPGRADING;

			break;
		}
		if (event == EVENT_CHECK_SCHEDULES) {
			//change_status_application(datosApp, CHECK_SCHEDULES);
			new_status = CHECK_SCHEDULES;

			break;
		}
		if (event == EVENT_NONE_SCHEDULE) {
			//change_status_application(datosApp, NORMAL_MANUAL);
			new_status = NORMAL_MANUAL;

			break;
		}
		if (event == EVENT_START_SCHEDULE) {
			//change_status_application(datosApp, CHECK_SCHEDULES);
			new_status = SCHEDULING;

			break;
		}
		if ((event == EVENT_END_SCHEDULE) || (event == EVENT_DELETE_SCHEDULE) || (event == EVENT_MODIFY_SCHEDULE) || (event == EVENT_INSERT_SCHEDULE)){
			new_status = CHECK_SCHEDULES;

			break;
		}
		if (event == EVENT_SMARTCONFIG_START) {
			new_status = FACTORY;
		}

		break;

	case NORMAL_MANUAL:
		if ((event == EVENT_ERROR_APP) || (event == EVENT_ERROR_DEVICE) || (event == EVENT_ERROR_LCD) || (event == EVENT_ERROR_NVS)) {
			//change_status_application(datosApp, ERROR_APP);
			new_status = ERROR_APP;

			break;
		}
		if (event == EVENT_UPGRADE_FIRMWARE) {
			//change_status_application(datosApp, UPGRADING);
			new_status = UPGRADING;

			break;
		}
		if (event == EVENT_CHECK_SCHEDULES) {
			//change_status_application(datosApp, CHECK_SCHEDULES);
			new_status = CHECK_SCHEDULES;

			break;
		}
		if (event == EVENT_START_SCHEDULE) {
			//change_status_application(datosApp, CHECK_SCHEDULES);
			new_status = SCHEDULING;

			break;
		}

		if ((event == EVENT_END_SCHEDULE) || (event == EVENT_DELETE_SCHEDULE) || (event == EVENT_MODIFY_SCHEDULE) || (event == EVENT_INSERT_SCHEDULE)){
			new_status = CHECK_SCHEDULES;

			break;
		}
		if (event == EVENT_SMARTCONFIG_START) {
			new_status = FACTORY;
		}

		break;

	case STARTING:
		if ((event == EVENT_ERROR_APP) || (event == EVENT_ERROR_DEVICE) || (event == EVENT_ERROR_LCD) || (event == EVENT_ERROR_NVS)) {
			//change_status_application(datosApp, ERROR_APP);
			new_status = ERROR_APP;

			break;
		}


		if (event == EVENT_START_APP) {
			if (using_schedules(datosApp)) {
				new_status = CHECK_SCHEDULES;
			} else {
				new_status = NORMAL_MANUAL;

			}

			break;
		}

		if (event == EVENT_UPGRADE_FIRMWARE) {
			//change_status_application(datosApp, UPGRADING);
			new_status = UPGRADING;

			break;
		}

		if ((event == EVENT_DEVICE_OK) || (event == EVENT_WIFI_OK) || (event == EVENT_SMARTCONFIG_END)){
			ESP_LOGW(TAG, ""TRAZAR" cambiando de estado por EVENT_DEVICE_OK", INFOTRAZA);
			new_status = STARTING;
			break;

		}
		if (event == EVENT_SMARTCONFIG_START) {
			new_status = FACTORY;
		}
		break;

	case UPGRADING:
		if (event == EVENT_END_UPGRADE) {
			//change_status_application(datosApp, RESTARTING);
			new_status = RESTARTING;
			break;
		}
		break;

	case WAITING_END_STARTING:
		break;

	case FACTORY:
		if (event == EVENT_SMARTCONFIG_END) {
			//change_status_application(datosApp, STARTING);
			new_status = STARTING;

			break;
		}

		if (event == EVENT_DEVICE_OK){
			new_status = FACTORY;
			break;
		}

		if (event == EVENT_WIFI_OK) {
			new_status = STARTING;
		}



		break;

	case CHECK_SCHEDULES:
		if ((event == EVENT_ERROR_APP) || (event == EVENT_ERROR_DEVICE) || (event == EVENT_ERROR_LCD) || (event == EVENT_ERROR_NVS)) {
			//change_status_application(datosApp, ERROR_APP);
			new_status = ERROR_APP;

			break;
		}
		if (event == EVENT_UPGRADE_FIRMWARE) {
			//change_status_application(datosApp, UPGRADING);
			new_status = UPGRADING;

			break;
		}
		if (event == EVENT_NONE_SCHEDULE) {
			//change_status_application(datosApp, NORMAL_AUTO);
			new_status = NORMAL_AUTO;

			break;
		}
		if (event == EVENT_START_SCHEDULE) {
			//change_status_application(datosApp, SCHEDULING);
			new_status = SCHEDULING;

			break;
		}

		if ((event == EVENT_END_SCHEDULE) || (event == EVENT_DELETE_SCHEDULE) || (event == EVENT_MODIFY_SCHEDULE) || (event == EVENT_INSERT_SCHEDULE)){
			new_status = CHECK_SCHEDULES;

			break;
		}

		break;

	case SCHEDULING:
		if ((event == EVENT_ERROR_APP) || (event == EVENT_ERROR_DEVICE) || (event == EVENT_ERROR_LCD) || (event == EVENT_ERROR_NVS)) {
			//change_status_application(datosApp, ERROR_APP);
			new_status = ERROR_APP;

			break;
		}
		if (event == EVENT_UPGRADE_FIRMWARE) {
			//change_status_application(datosApp, UPGRADING);
			new_status = UPGRADING;

			break;
		}

		if ((event == EVENT_END_SCHEDULE) || (event == EVENT_DELETE_SCHEDULE) || (event == EVENT_MODIFY_SCHEDULE) || (event == EVENT_INSERT_SCHEDULE)){
			new_status = CHECK_SCHEDULES;

			break;
		}
		if (event == EVENT_SMARTCONFIG_START) {
			new_status = FACTORY;
		}

		break;

	case RESTARTING:
		break;

	case APP_STARTED:
		if (event == EVENT_CHECK_SCHEDULES) {
			//change_status_application(datosApp, CHECK_SCHEDULES);
			new_status = CHECK_SCHEDULES;

			break;
		}
		if (event == EVENT_NO_SCHEDULE) {
			//change_status_application(datosApp, NORMAL_MANUAL);
			new_status = NORMAL_MANUAL;

			break;
		}
		if (event == EVENT_ERROR_DEVICE) {
			//change_status_application(datosApp, ERROR_APP);
			new_status = ERROR_APP;

			break;
		}

		if (event == EVENT_UPGRADE_FIRMWARE) {
			//change_status_application(datosApp, UPGRADING);
			new_status = UPGRADING;

			break;
		}
		if (event == EVENT_SMARTCONFIG_START) {
			new_status = FACTORY;
		}

		break;

	case UNKNOWN_STATUS:
		if (event == EVENT_STARTING) {
			new_status = STARTING;
			break;
		}
		break;

	default:
		ESP_LOGW(TAG, ""TRAZAR" ERROR DE CONSISTENCIA AL CAMBIAR EL ESTADO DE LA APLICACION DESDE %s --> %s",
				INFOTRAZA, status2mnemonic(get_current_status_application(datosApp)), event2mnemonic(event));
		break;


	}

	change_status_application(datosApp, new_status);





	return;
}
