/*
 * common_data.h
 *
 *  Created on: Jun 19, 2023
 *      Author: t126401
 */

#ifndef COMPONENTS_COMUNES_INCLUDE_COMMON_DATA_H_
#define COMPONENTS_COMUNES_INCLUDE_COMMON_DATA_H_

#include "cJSON.h"
#include "mqtt_client.h"
#include "stdint.h"
#include "stdio.h"
#include "alarm_data.h"
#include "espota.h"
#include "ntp.h"
#include "nvs.h"
#include "thermostat.h"





typedef enum ESTADO_APP {
    NORMAL_AUTO,
    NORMAL_AUTOMAN,
    NORMAL_MANUAL,
    NORMAL_ARRANCANDO,
    NORMAL_SIN_PROGRAMACION,
    UPGRADE_EN_PROGRESO,
	NORMAL_SINCRONIZANDO,
	ESPERA_FIN_ARRANQUE,
	ARRANQUE_FABRICA,
	NORMAL_FIN_PROGRAMA_ACTIVO



} ESTADO_APP;


typedef enum TIPO_DISPOSITIVO {
	DESCONOCIDO = -1,
	INTERRUPTOR = 0,
	CRONOTERMOSTATO = 1,
	TERMOMETRO = 2
}TIPO_DISPOSITIVO;



typedef struct MQTT_PARAM {
    char broker[100];
    int port;
    char user[25];
    char password[25];
    char publish[50];
    char subscribe[50];
    int qos;
    bool tls;
    char* cert;

} MQTT_PARAM;


typedef struct DATOS_GENERALES {
	enum TIPO_DISPOSITIVO tipoDispositivo;
    MQTT_PARAM parametrosMqtt;
    struct NTP_CLOCK clock;
    struct TIME_PROGRAM *programacion;
    uint8_t nProgramacion;
    uint8_t programa_estatico_real;
    int nProgramaCandidato;
    PROG_STATE estadoProgramacion;
    OTADATA ota;
    bool botonPulsado;
    //Estado de la aplicacion
    enum ESTADO_APP estadoApp;


}DATOS_GENERALES;


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

#endif /* COMPONENTS_COMUNES_INCLUDE_COMMON_DATA_H_ */
