/*
 * mqtt.h
 *
 *  Created on: 22 sept. 2020
 *      Author: t126401
 */

//#include "configuracion_usuario.h"
#include "common_data.h"

#ifndef COMPONENTS_CONEXIONES_INCLUDE_MQTT_H_
#define COMPONENTS_CONEXIONES_INCLUDE_MQTT_H_

typedef struct COLA_MQTT {
	char topic[50];
	char buffer[1024];
}COLA_MQTT;


void init_device_mqtt(void *arg);
esp_err_t unsubscribe_topic(DATOS_APLICACION *datosApp, int index_topic);
esp_err_t subscribe_topic(DATOS_APLICACION *datosApp, char* topic);
esp_err_t publicar_mensaje(DATOS_APLICACION *datosApp, char* topic, char* message);
esp_err_t publicar_mensaje_json(DATOS_APLICACION *datosAppp, cJSON *mensaje, char *topic);
esp_err_t publicar_mensaje_json_generico(DATOS_APLICACION *datosApp, cJSON *mensaje, char* topic);
void mqtt_task(void *datosApp);
void init_mqtt_service(DATOS_APLICACION *datosApp);
esp_err_t obtener_certificado(DATOS_APLICACION *datosApp);

#endif /* COMPONENTS_CONEXIONES_INCLUDE_MQTT_H_ */
