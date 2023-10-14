/*
 * alarm_data.h
 *
 *  Created on: Jun 19, 2023
 *      Author: t126401
 */

#ifndef COMPONENTS_ALARMAS_INCLUDE_ALARM_DATA_H_
#define COMPONENTS_ALARMAS_INCLUDE_ALARM_DATA_H_









#define MNEMONIC_ALARM_DEVICE	"alarmDevice"
#define MNEMONIC_ALARM_APP		"alarmApp"
#define MNEMONIC_ALARM_LCD		"alarmLcd"
#define MNEMONIC_ALARM_WIFI		"alarmaWifi"
#define MNEMONIC_ALARM_NVS		"alarmaNvs"
#define MNEMONIC_ALARM_NTP		"alarmaNtp"
#define MNEMONIC_ALARM_MQTT		"alarmaMqtt"


typedef enum TIPO_NOTIFICACION {
	NOTIFICACION_LOCAL,
	NOTIFICACION_REMOTA
}TIPO_NOTIFICACION;

typedef enum ALARM_STATUS {
	ALARM_UNDEFINED = -1,
	ALARM_OFF,
	ALARM_WARNING,
	ALARM_ON,


}ALARM_STATUS;


typedef enum ALARM_TYPE {
	ALARM_DEVICE,
	ALARM_APP,
	ALARM_LCD,
	ALARM_NVS,
	ALARM_WIFI,
	ALARM_NTP,
	ALARM_MQTT,
	ALARM_END

}ALARM_TYPE;


typedef struct ALARMA {

	ALARM_TYPE tipo_alarma;
	enum ALARM_STATUS estado_alarma;
	time_t fecha_alarma;
	char nemonico[50];

} ALARMA;



#endif /* COMPONENTS_ALARMAS_INCLUDE_ALARM_DATA_H_ */
