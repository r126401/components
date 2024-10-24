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
#include "local_events_device.h"




typedef enum EVENT_TYPE {

	EVENT_ERROR_REMOTE_DEVICE,
	EVENT_ERROR_DEVICE,
	EVENT_ERROR_APP,
	EVENT_ERROR_NVS,
	EVENT_ERROR_LCD,
	EVENT_ERROR_NTP,
	EVENT_ERROR_WIFI,
	EVENT_ERROR_MQTT,
	EVENT_MQTT_SUBSCRIBED,
	EVENT_DEVICE_READY,
	EVENT_REMOTE_DEVICE_OK,
	EVENT_APP_OK,
	EVENT_NVS_OK,
	EVENT_LCD_OK,
	EVENT_NTP_OK,
	EVENT_GET_NTP,
	EVENT_WIFI_OK,
	EVENT_MQTT_OK,
	EVENT_CONNECT_MQTT,
	EVENT_CHECK_SCHEDULES,
	EVENT_INSERT_SCHEDULE,
	EVENT_MODIFY_SCHEDULE,
	EVENT_DELETE_SCHEDULE,
	EVENT_START_SCHEDULE,
	EVENT_END_SCHEDULE,
	EVENT_NO_ACTIVE_SCHEDULE,
	EVENT_SCHEDULES_OK,
	EVENT_NO_SCHEDULE,
	EVENT_FACTORY,
	EVENT_UPGRADE_FIRMWARE,
	EVENT_END_UPGRADE,
	EVENT_ERROR_UPGRADE,
	EVENT_LOCAL_DEVICE,
	EVENT_SMARTCONFIG_START,
	EVENT_SMARTCONFIG_END,
	EVENT_START_APP,
	EVENT_ERROR_SMARTCONFIG,
	EVENT_AUTOMAN,
	EVENT_STARTING,
	EVENT_RESTART,
} EVENT_TYPE;



typedef struct EVENT_APP {

	EVENT_TYPE event_app;
	EVENT_DEVICE event_device;

}EVENT_APP;


typedef enum ESTADO_APP {

	UNKNOWN_STATUS = -2,
	ERROR_APP = -1,
    NORMAL_AUTO,
    NORMAL_AUTOMAN,
    NORMAL_MANUAL,
    STARTING,
    UPGRADING,
	WAITING_END_STARTING,
	FACTORY,
	CHECK_SCHEDULES,
	SCHEDULING,
	RESTARTING,
	DEVICE_READY,
	RECOVERING



} ESTADO_APP;


typedef enum TIPO_DISPOSITIVO {
	DESCONOCIDO = -1,
	INTERRUPTOR = 0,
	CRONOTERMOSTATO = 1,
	TERMOMETRO = 2
}TIPO_DISPOSITIVO;

typedef enum STATUS_DEBUG {
	TOPIC_NO_ACTIVE,
	TOPIC_ACTIVE
}STATUS_DEBUG;


typedef struct TOPICS_LIST {

	char publish[50];
	char subscribe[50];
	STATUS_DEBUG status;


}TOPICS_LIST;

typedef struct MQTT_PARAM {
    char broker[100];
    int port;
    char user[25];
    char password[25];
    char publish[50];
    char subscribe[50];
    int qos;
    bool tls;
    int size_cert;
    TOPICS_LIST topics[CONFIG_NUM_TOPICS];


} MQTT_PARAM;

typedef enum CONFIGURE_DEVICE {

	DEVICE_NOT_CONFIGURED,
	WIFI_NOT_CONFIGURED,
	WIFI_CONFIGURED,
	MQTT_NOT_CONFIGURED,
	MQTT_CONFIGURED,
	DEVICE_CONFIGURED

} CONFIGURE_DEVICE;


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
    //Determina el estado del dispositivo a nivel de configuracion
    enum CONFIGURE_DEVICE status;


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
    ALARMA alarmas[ALARM_END];
    DATOS_TERMOSTATO termostato;
    bool wifi;
    bool mqtt;
    bool timing;
    bool schedules;





} DATOS_APLICACION;

#endif /* COMPONENTS_COMUNES_INCLUDE_COMMON_DATA_H_ */
