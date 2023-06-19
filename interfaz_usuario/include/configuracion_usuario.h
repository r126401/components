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
#include "cJSON.h"
#include "mqtt_client.h"
#include "stdint.h"
#include "stdio.h"
#include "alarm_data.h"
#include "espota.h"
#include "ntp.h"
#include "nvs.h"
#include "common_data.h"


typedef enum ESTADO_RELE {
    INDETERMINADO = -1,
    OFF = 0,
    ON = 1
}ESTADO_RELE;

typedef enum TIPO_ACTUACION_RELE {
    MANUAL,
    REMOTA,
    TEMPORIZADA,
    ARRANQUE_RELE,
    EXPIRADA
}TIPO_ACTUACION_RELE;

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

void tarea_upgrade_firmware(DATOS_APLICACION *datosApp);

#endif /* COMPONENTS_COMUNES_INCLUDE_CONFIGURACION_USUARIO_H_ */
