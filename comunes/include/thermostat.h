/*
 * thermostat.h
 *
 *  Created on: Jun 19, 2023
 *      Author: t126401
 */

#ifndef COMPONENTS_COMUNES_INCLUDE_THERMOSTAT_H_
#define COMPONENTS_COMUNES_INCLUDE_THERMOSTAT_H_

#include "relay.h"
#define NUM_TIPOS_ALARMAS 6
#define ALARMA_SENSOR_DHT 4
#define ALARMA_SENSOR_REMOTO 5

#define NOTIFICACION_ALARMA_SENSOR_REMOTO "alarmaDhtRemoto"
#define NOTIFICACION_ALARMA_SENSOR_DHT "alarmaDht"


enum COMANDOS_APP {
    OPERAR_RELE = 50,
    STATUS_DISPOSITIVO,
	MODIFICAR_UMBRAL,
	SELECCIONAR_SENSOR
};


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




#endif /* COMPONENTS_COMUNES_INCLUDE_THERMOSTAT_H_ */
