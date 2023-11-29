/*
 * thermostat.h
 *
 *  Created on: Jun 19, 2023
 *      Author: t126401
 */

#ifndef COMPONENTS_COMUNES_INCLUDE_THERMOSTAT_H_
#define COMPONENTS_COMUNES_INCLUDE_THERMOSTAT_H_

#include "relay.h"

#define NOTIFICACION_ALARMA_SENSOR_REMOTO "alarmaDhtRemoto"
#define NOTIFICACION_ALARMA_SENSOR_DHT "alarmaDht"


enum COMANDOS_APP {
    OP_RELAY = 50,
    STATUS_DEVICE,
	MODIFY_THRESHOLD_TEMPERATURE,
	SELECT_SENSOR_TEMPERATURE
};


typedef struct DATOS_TERMOSTATO {
    float tempActual;
    float tempUmbral;
    float tempUmbralDefecto;
    float humedad;
    double margenTemperatura;
    uint8_t reintentosLectura;
    uint8_t retry_interval;
    uint8_t read_interval;
    double calibrado;
    bool master;
    char sensor_remoto[13];
    float incdec;

}DATOS_TERMOSTATO;




#endif /* COMPONENTS_COMUNES_INCLUDE_THERMOSTAT_H_ */
