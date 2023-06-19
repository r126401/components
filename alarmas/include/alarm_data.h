/*
 * alarm_data.h
 *
 *  Created on: Jun 19, 2023
 *      Author: t126401
 */

#ifndef COMPONENTS_ALARMAS_INCLUDE_ALARM_DATA_H_
#define COMPONENTS_ALARMAS_INCLUDE_ALARM_DATA_H_


#define ALARMA_WIFI 0
#define ALARMA_MQTT 1
#define ALARMA_NTP 2
#define ALARMA_NVS 3


#define NOTIFICACION_ALARMA_WIFI	"alarmaWifi"
#define NOTIFICACION_ALARMA_MQTT	"alarmaMqtt"
#define NOTIFICACION_ALARMA_NTP		"alarmaNtp"
#define NOTIFICACION_ALARMA_NVS		"alarmaNvs"



typedef enum TIPO_NOTIFICACION {
	NOTIFICACION_LOCAL,
	NOTIFICACION_REMOTA
}TIPO_NOTIFICACION;

typedef enum ESTADO_ALARMA {
	ALARMA_INDETERMINADA = -1,
	ALARMA_OFF,
	ALARMA_WARNING,
	ALARMA_ON,


}ESTADO_ALARMA;

typedef struct ALARMA {

	uint8_t tipo_alarma;
	enum ESTADO_ALARMA estado_alarma;
	time_t fecha_alarma;
	char nemonico[50];

} ALARMA;



#endif /* COMPONENTS_ALARMAS_INCLUDE_ALARM_DATA_H_ */
