/**
Proyecto realizado por Emilio Jimenez Prieto
*/

/*
 * configuracion_usuario.h
 *
 *  Created on: 10 sept. 2020
 *      Author: t126401
 */





#ifndef INCLUDE_CONFIGURACION_USUARIO_H_
#define INCLUDE_CONFIGURACION_USUARIO_H_
#include "datos_comunes.h"
#include "cJSON.h"
#include "mqtt_client.h"
#include "stdint.h"
#include "stdio.h"


/**
 * @def NUM_TIPOS_ALARMAS
 * @brief Define el numero de alarmas del dispositivo
 *
 */

enum COMANDOS_APP {
    OPERAR_RELE = 50,
    STATUS_DISPOSITIVO,
	MODIFICAR_UMBRAL,
	SELECCIONAR_SENSOR
};

#define NUM_TIPOS_ALARMAS 6
#define ALARMA_WIFI 0
#define ALARMA_MQTT 1
#define ALARMA_NTP 2
#define ALARMA_NVS 3
#define ALARMA_SENSOR_DHT 4
#define ALARMA_SENSOR_REMOTO 5
#define NOTIFICACION_ALARMA_WIFI	"alarmaWifi"
#define NOTIFICACION_ALARMA_MQTT	"alarmaMqtt"
#define NOTIFICACION_ALARMA_NTP		"alarmaNtp"
#define NOTIFICACION_ALARMA_NVS		"alarmaNvs"
#define NOTIFICACION_ALARMA_SENSOR_REMOTO "alarmaDhtRemoto"
#define NOTIFICACION_ALARMA_SENSOR_DHT "alarmaDht"


typedef struct DATOS_TERMOSTATO {
    float tempActual;
    float tempUmbral;
    float tempUmbralDefecto;
    float humedad;
    double margenTemperatura;
    uint8_t reintentosLectura;
    uint8_t intervaloReintentos;
    uint8_t intervaloLectura;
    double calibrado;
    bool master;
    char sensor_remoto[13];
    float incdec;

}DATOS_TERMOSTATO;

/**
 * @struct DATOS_APLICACION
 * @brief Estructura general de la aplicacion.
 *
 */
typedef struct DATOS_APLICACION {

	DATOS_GENERALES *datosGenerales;
    nvs_handle handle;
    esp_mqtt_event_handle_t handle_mqtt;
    ALARMA alarmas[CONFIG_NUMERO_ALARMAS_PROVISIONADAS];
    DATOS_TERMOSTATO termostato;





} DATOS_APLICACION;



#endif /* COMPONENTS_COMUNES_INCLUDE_CONFIGURACION_USUARIO_H_ */
