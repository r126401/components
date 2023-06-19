/**
Proyecto realizado por Emilio Jimenez Prieto
*/

/*
 * iotOnOff.h
 *
 *  Created on: 10 sept. 2020
 *      Author: t126401
 */


#include "configuracion_usuario.h"

#ifndef MAIN_INCLUDE_IOTONOFF_H_
#define MAIN_INCLUDE_IOTONOFF_H_

typedef enum TIPO_LECTURA {

	LECTURA_ARRANQUE,
	LECTURA_MUESTREO,
	LECTURA_TEMPORIZADA
}TIPO_LECTURA;

typedef enum TIPO_ACCION_TERMOSTATO {
	NO_ACCIONAR_TERMOSTATO,
	ACCIONAR_TERMOSTATO
}TIPO_ACCION_TERMOSTATO;




#define MODIFICAR_APP "modificarApp"
#define APP_PARAMS "configApp"
#define MASTER		"master"
#define SENSOR_REMOTO	"idsensor"
#define MODIFICAR_SENSOR_TEMPERATURA	"sensorTemperatura"
#define INCDEC "incdecTemperatura"

enum ESTADO_RELE operacion_rele(DATOS_APLICACION *datosApp, enum TIPO_ACTUACION_RELE tipo, enum ESTADO_RELE operacion);


void programacion_task(void *parametro);
void ejecutar_lectura_termometro(DATOS_APLICACION *datosApp);
esp_err_t lectura_temperatura(DATOS_APLICACION *datosApp);
//esp_err_t tomar_lectura_dht(struct DATOS_APLICACION *datosApp);
esp_err_t leer_temperatura_local(DATOS_APLICACION *datosApp);
esp_err_t leer_temperatura(DATOS_APLICACION *datosApp);
void tarea_lectura_temperatura(void *parametros);
esp_err_t leer_temperatura_remota(DATOS_APLICACION *datosApp);
enum TIPO_ACCION_TERMOSTATO calcular_accion_termostato(DATOS_APLICACION *datosApp, enum ESTADO_RELE *accion);
esp_err_t seleccionarSensorTemperatura(cJSON *peticion, DATOS_APLICACION *datosApp, cJSON *respuesta);
esp_err_t notificar_fin_arranque(DATOS_APLICACION *datosApp);
void gpio_rele_in();
void gpio_rele_out();
void pulsacion_modo_app(DATOS_APLICACION *datosApp);









#endif /* MAIN_INCLUDE_IOTONOFF_H_ */
