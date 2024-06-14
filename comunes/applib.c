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

static const char *TAG = "applib.c";


esp_err_t init_global_parameters_device(DATOS_APLICACION *datosApp) {

	esp_err_t error;

	DATOS_GENERALES *datosGenerales;
	datosGenerales = (DATOS_GENERALES*) calloc(1, sizeof(DATOS_GENERALES));
	datosApp->datosGenerales = datosGenerales;
	create_event_task(datosApp);
	error = init_device(datosApp);
	if (error == ESP_OK) {
		ESP_LOGI(TAG, ""TRAZAR"INICIALIZACION CORRECTA", INFOTRAZA);
	} else {

	}

	return error;


}

void init_service_device(DATOS_APLICACION *datosApp) {

	//En primer lugar se inicia la conexion wifi

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




	if (!get_app_config_wifi(datosApp)) {

		ESP_LOGW(TAG, ""TRAZAR"init_service_device: No se configura wifi por lo que no se configuran el resto de servicios de red", INFOTRAZA);
		return;
	}

		init_wifi_device();
		ESP_LOGW(TAG, ""TRAZAR"init_service_device: Wifi configurado y activo", INFOTRAZA);

		//2.- Se inicia la tarea mqtt si estuviera configurado.
	if (get_app_config_mqtt(datosApp)) {
		crear_tarea_mqtt(datosApp);
		ESP_LOGW(TAG, ""TRAZAR" init_service_device: servicio mqtt configurado y activo", INFOTRAZA);
	} else {
		// Si no estuviera configurado, iniciamos la secuencia de activacion del ntp y la programcion si procede
		if (get_app_config_timing(datosApp)) {
			sync_app_by_ntp(datosApp);
			if (get_app_config_manage_schedules(datosApp)) {
				iniciar_gestion_programacion(datosApp);
			}
		}



	}




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

	ESP_LOGW(TAG, ""TRAZAR" El valor de mqtt es %d", INFOTRAZA, datosApp->mqtt);

	return datosApp->mqtt;
}

bool get_app_config_timing(DATOS_APLICACION *datosApp) {

	return datosApp->timing;
}

bool get_app_config_manage_schedules(DATOS_APLICACION *datosApp) {

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



	ESP_LOGW(TAG, ""TRAZAR"ESTADO ANTERIOR %s", INFOTRAZA, status2mnemonic(datosApp->datosGenerales->estadoApp));
	datosApp->datosGenerales->estadoApp = new_status;
	ESP_LOGW(TAG, ""TRAZAR"ESTADO POSTERIOR %s", INFOTRAZA, status2mnemonic(new_status));
	appuser_notify_app_status(datosApp, new_status);


}
