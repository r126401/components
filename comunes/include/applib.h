/*
 * applib.h
 *
 *  Created on: 10 jun 2024
 *      Author: esp8266
 */

#ifndef COMPONENTS_COMUNES_INCLUDE_APPLIB_H_
#define COMPONENTS_COMUNES_INCLUDE_APPLIB_H_

#include "common_data.h"

/**
 * @fn enum CONFIGURE_DEVICE get_app_status_device(DATOS_APLICACION*)
 * @brief Obtiene el estado de configuracion del dispositivo
 *
 * @pre
 * @post
 * @param datosApp
 * @return
 */
enum CONFIGURE_DEVICE get_app_status_device(DATOS_APLICACION *datosApp);


/**
 * @fn void set_app_status_device(DATOS_APLICACION*, enum CONFIGURE_DEVICE)
 * @brief Pone el estado de configuracion del dispositivo
 *
 * @pre
 * @post
 * @param datosApp
 * @param status
 */
void set_app_status_device(DATOS_APLICACION *datosApp, enum CONFIGURE_DEVICE status);


#endif /* COMPONENTS_COMUNES_INCLUDE_APPLIB_H_ */
