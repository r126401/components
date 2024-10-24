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

/**
 * @fn char get_app_publish_topic*(DATOS_APLICACION*, uint8_t)
 * @brief get application topic
 *
 * @pre
 * @post
 * @param datosApp
 * @param index
 * @return
 */
char* get_app_publish_topic(DATOS_APLICACION *datosApp, uint8_t index);
char* get_app_subscribe_topic(DATOS_APLICACION *datosApp, uint8_t index);
void set_app_publish_topic(DATOS_APLICACION *datosApp, uint8_t index, char* topic);
void set_app_subscribe_topic(DATOS_APLICACION *datosApp, uint8_t index, char* topic);
esp_err_t init_device(DATOS_APLICACION *datosApp);
esp_err_t init_data_app(DATOS_APLICACION *datosApp);
esp_err_t init_services_device(DATOS_APLICACION *datosApp);
void config_default_services_device(DATOS_APLICACION *datosApp);
void set_app_config_wifi(DATOS_APLICACION *datosApp, bool flag);
void set_app_config_mqtt(DATOS_APLICACION *datosApp, bool flag);
void set_app_config_timing(DATOS_APLICACION *datosApp, bool flag);
void set_app_config_manage_schedules(DATOS_APLICACION *datosApp, bool flag);
bool get_app_config_wifi(DATOS_APLICACION *datosApp);
bool using_mqtt(DATOS_APLICACION *datosApp);
bool using_ntp(DATOS_APLICACION *datosApp);
bool using_schedules(DATOS_APLICACION *datosApp);
void set_new_certificate(DATOS_APLICACION *datosApp, char* text, int size);
void set_default_certificate(DATOS_APLICACION *datosApp);
char* get_certificate(DATOS_APLICACION *datosApp);
bool get_mqtt_tls(DATOS_APLICACION *datosApp);
void set_mqtt_tls(DATOS_APLICACION *datosApp, bool tls);
void change_status_application(DATOS_APLICACION *datosApp, ESTADO_APP new_status);
void set_status_application(DATOS_APLICACION *datosApp, EVENT_TYPE event);
