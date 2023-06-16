/*
 * interfaz_usuario.h
 *
 *  Created on: 21 sept. 2020
 *      Author: t126401
 */

#ifndef COMPONENTS_INTERFAZ_USUARIO_INCLUDE_INTERFAZ_USUARIO_H_
#define COMPONENTS_INTERFAZ_USUARIO_INCLUDE_INTERFAZ_USUARIO_H_

#include "configuracion_usuario.h"
#include "programmer.h"




/**
 * @fn esp_err_t appUser_analizarComandoAplicacion(cJSON*, int, DATOS_APLICACION*, cJSON*)
 * @brief Funcion llamada desde analizar_comando cuando el idComando > 50. Son comandos especificos del dispositivo concreto.
 *
 *
 * @pre
 * @post
 * @param peticion Es el texto de la peticion recibida
 * @param nComando Es la identidad el comando
 * @param datosApp Es la estructura de la aplicacion
 * @param respuesta Es parametro de vuelta con la informacion del comando despues de procesar por el dispositivo
 * @return ESP_OK si no ha habido ningun error sintactico o semantico
 */
esp_err_t appuser_set_command_application(cJSON *peticion, int nComando, DATOS_APLICACION *datosApp, cJSON *respuesta);
/**
 * @fn esp_err_t appuser_temporizador_cumplido(DATOS_APLICACION*)
 * @brief Esta funcion es llamada desde el modulo programmer y comunica el inicio de un nuevo programa para que la aplicacion
 * lo pueda notificar
 *
 * @pre
 * @post
 * @param datosApp Es la estructura de la aplicacion
 * @return ESP_OK cuando el resultado es ok
 */
esp_err_t appuser_start_schedule(DATOS_APLICACION *datosApp);
/**
 * @fn esp_err_t appuser_init_application(DATOS_APLICACION*)
 * @brief Esta funcion es llamada desde la aplicacion de usuario y determina las acciones de inicializacion del despositivo especifico.
 *
 * @pre
 * @post
 * @param datosApp es la estructura de aplicacion
 * @return ESP_OK cuando la inicializacion del dispositivo especifico es correcta
 */
esp_err_t appuser_init_application(DATOS_APLICACION *datosApp);
/**
 * @fn esp_err_t appuser_wifi_conectando()
 * @brief Llamado desde el modulo conexiones de manera que el usuario pueda notificar en la aplicacion que el dispositivo
 * esta intentando conectar a una red wifi
 *
 * @pre
 * @post
 * @return
 */
esp_err_t appuser_notify_connecting_wifi();
/**
 * @fn esp_err_t appuser_wifi_conectado()
 * @brief Llamado desde el modulo conexiones de manera que el usuario pueda notificar en la aplicacion que el dispositivo
 * se ha conectado correcamente a una red wifi
 *
 * @pre
 * @post
 * @return
 */
esp_err_t appuser_notify_wifi_connected_ok();
/**
 * @fn esp_err_t appuser_broker_conectando()
 * @brief Llamado desde el modulo de conexiones, el usuario puede indicar que la aplicacion esta conectandose al broker mqtt
 *
 * @pre
 * @post
 * @return
 */
esp_err_t appuser_notify_connecting_broker_mqtt();
/**
 * @fn esp_err_t appuser_broker_conectado()
 * @brief Llamado desde el modulo de conexion indica al usuario que la aplicacion se ha conec
 *
 * @pre
 * @post
 * @return
 */
esp_err_t appuser_notify_broker_connected_ok();
/**
 * @fn esp_err_t appuser_broker_desconectado()
 * @brief El usuario puede indicar acciones en el dispositivo para notificar el evento.
 *
 * @pre
 * @post
 * @return
 */
esp_err_t appuser_notify_broker_disconnected();
/**
 * @fn esp_err_t appuser_obteniendo_sntp()
 * @brief llamada desde el modulo de ntp tiene la finalidad de comunicar al usuario que se esta intentando sincronizar la hora
 * con el servidor ntp
 *
 * @pre
 * @post
 * @return
 */
esp_err_t appuser_get_date_sntp();
/**
 * @fn esp_err_t appuser_error_get_date_sntp()
 * @brief La funcion es llamada desde el modulo ntp para indicar a la aplicacion que no se ha podido sincronizar la hora
 *
 * @pre
 * @post
 * @return
 */
esp_err_t appuser_error_get_date_sntp();
/**
 * @fn esp_err_t appuser_sntp_ok()
 * @brief Llamada desde el modulo ntp para indicar que la aplicacion ha sincronizado la hora correctamente
 *
 * @pre
 * @post
 * @return
 */
esp_err_t appuser_sntp_ok();
/**
 * @fn esp_err_t appuser_arranque_aplicacion(DATOS_APLICACION*)
 * @brief Esta funcion es llamada despues de la inicializacion del dispositivo y una vez suscrito al topic para que se pueda notificar
 * a la aplicacion remota que el dispositivo esta arrancado y preparado. El usuario puede definir que acciones quiere realizar
 * para notificarlo.
 *
 * @pre
 * @post
 * @param datosApp
 * @return
 */
esp_err_t appuser_notify_application_started(DATOS_APLICACION *datosApp);
/**
 * @fn void appuser_ejecucion_accion_temporizada(void*)
 * @brief Esta funcion es invocada una vez expira el temporizacion de duracion de un evento de programacion. En esta funcion,
 * el usuario puede definir las acciones a realizar en el dispitivo.
 *
 * @pre
 * @post
 * @param datosApp
 */
void appuser_end_schedule(void *arg);
/**
 * @fn esp_err_t appuser_acciones_ota(DATOS_APLICACION*)
 * @brief Esta funcion es invocada desde apijson despues de responder al comando de iniciar upgrade ota y tiene la finalidad de
 * que la aplicacion pueda informar segun necesidad del usuario
 *
 * @pre
 * @post
 * @param datosApp
 * @return
 */
esp_err_t appuser_start_ota(DATOS_APLICACION *datosApp);

/**
 * @fn esp_err_t appuser_notify_smartconfig()
 * @brief Es llamada desde la libreria conexiones cuando se comienza la tarea de smartconfig para que el usuario
 * pueda señalizar o establecer las acciones oportunas en el dispositivo cuando comienza el proceso de smartconfig.
 *
 * @return ESP_OK por defecto.
 */
esp_err_t appuser_notify_smartconfig();

/**
 * @fn esp_err_t appuser_notify_local_alarm(DATOS_APLICACION*, uint8_t)
 * @brief Esta funcion se utiliza para notificar localmente que se ha producido una alarma en el dispositivo.
 *
 * @param datosApp
 * @param indice es el id de la alarma que se ha producido
 * @return
 */

esp_err_t appuser_notify_local_alarm(DATOS_APLICACION *datosApp, uint8_t indice);


/**
 * @fn esp_err_t appuser_set_default_config(DATOS_APLICACION*)
 * @brief es llamada desde la llibreria de configuracion para establecer los parametros por defecto de la aplicacion cuando arranca desde factory reset
 * o es la primera vez que se ejecuta.
 *
 * @param datosApp Son los datos de la aplicacion.
 * @return ESP_OK por defecto.
 */
esp_err_t appuser_set_default_config(DATOS_APLICACION *datosApp);


/**
 * @fn esp_err_t appuser_set_configuration_to_json(DATOS_APLICACION*, cJSON*)
 * @brief el objetivo de esta funcion es convertir la estructura DATOS_APLICACION del dispositivo en una
 * estructura json que posteriormente se guardara en la particion nvs
 *
 * @param datosApp
 * @param conf Es la estructura en formato json que se podra grabar en la particion nvs
 * @return
 */
esp_err_t appuser_set_configuration_to_json(DATOS_APLICACION *datosApp, cJSON *conf);

/**
 * @fn esp_err_t appuser_json_to_configuration(DATOS_APLICACION*, cJSON*)
 * @brief Esta funcion vuelca sobre la estructura DATOS_APLICACION la configuracion json leida.
 *
 * @param datosApp
 * @param datos
 * @return
 */
esp_err_t appuser_json_to_configuration(DATOS_APLICACION *datosApp, cJSON *datos);


/**
 * @fn esp_err_t appUser_set_action_without_schedule_active(DATOS_APLICACION*)
 * @brief Es llamada desde el programador para que el usuario pueda gestionar el comportamiento de la aplicacion cuando no hay ningun programa activo
 * en ese momento.
 *
 * @param datosApp Son los datos de la aplicacion.
 * @return ESP_OK por defecto.
 */
esp_err_t appuser_set_action_without_schedule_active(DATOS_APLICACION *datosApp);


/**
 * @fn esp_err_t appuser_load_default_schedules(DATOS_APLICACION*, cJSON*)
 * @brief
 *
 * @param datosApp
 * @param array
 * @return
 */
esp_err_t appuser_load_default_schedules(DATOS_APLICACION *datosApp, cJSON *array);
/**
 * @fn esp_err_t appuser_get_schedule_extra_data(TIME_PROGRAM*, cJSON*)
 * @brief Esta funcion llamada desde api_json extrae para la aplicacion los datos extras del schedule dependiendo del tipo de dispositivo específico.
 *
 * @param programa_actual
 * @param nodo
 * @return
 */
esp_err_t appuser_get_schedule_extra_data(TIME_PROGRAM *programa_actual, cJSON *nodo);

/**
 * @fn esp_err_t appuser_modify_schedule_extra_data(TIME_PROGRAM*, cJSON*)
 * @brief Esta funcion llamada dedse api_json modifica los datos del schedule específicos del dispositivo
 *
 * @param programa_actual
 * @param nodo
 * @return
 */
esp_err_t appuser_modify_schedule_extra_data(TIME_PROGRAM *programa_actual, cJSON *nodo);

/**
 * @fn esp_err_t appuser_reporting_schedule_extra_data(TIME_PROGRAM*, cJSON*)
 * @brief Esta funcion añade al api_json los datos extras de los informes relativos a los schedules especificos segun el dispositivo
 *
 * @param programa_actual
 * @param nodo
 * @return
 */
esp_err_t appuser_reporting_schedule_extra_data(TIME_PROGRAM *programa_actual, cJSON *nodo);

/**
 * @fn esp_err_t appuser_modify_local_configuration_application(cJSON*, DATOS_APLICACION*, cJSON*)
 * @brief Esta funcion realiza las modificaciones especificas de la estructura de aplicacion en funcion del tipo de dispositivo
 *
 * @param root
 * @param datosApp
 * @param respuesta
 * @return
 */
esp_err_t appuser_modify_local_configuration_application(cJSON *root, DATOS_APLICACION *datosApp, cJSON *respuesta);
/**
 * @fn esp_err_t appuser_received_message_extra_subscription(DATOS_APLICACION*)
 * @brief Esta funcion es utilizada para recibir mensajes de otras suscripciones diferentes al topic de suscripcion ya suscripto dentro de la
 * estructura de DATOS_APLICACION.
 *
 * @param datosApp
 * @return
 */
esp_err_t appuser_received_message_extra_subscription(DATOS_APLICACION *datosApp);

/**
 * @fn esp_err_t appuser_notify_app_status(DATOS_APLICACION*, enum ESTADO_APP)
 * @brief Se usa para que el usuario pueda redefinir el comportamiento de la aplicacion cuando recibe un cambio de estado.
 *
 * @param datosApp
 * @param estado
 * @return
 */
esp_err_t appuser_notify_app_status(DATOS_APLICACION *datosApp, enum ESTADO_APP estado);

/**
 * @fn void appuser_notify_schedule_events(DATOS_APLICACION*)
 * @brief Esta funcion permite al usuario realizar una notificacion debido a un evento temporal. Es llamada desde el programmer cada segundo.
 *
 * @param datosApp
 */
void appuser_notify_schedule_events(DATOS_APLICACION *datosApp);


void nemonicos_alarmas(DATOS_APLICACION *datosApp, int i);
/**
 * @fn cJSON appuser_send_spontaneous_report*(DATOS_APLICACION*, enum TIPO_INFORME, cJSON*)
 * @brief Esta funcion envia de manera normalizada los reportes espontaneos de la aplicacion.
 *
 * @param datosApp
 * @param tipoInforme
 * @param comandoOriginal
 * @return
 */
cJSON* appuser_send_spontaneous_report(DATOS_APLICACION *datosApp, enum TIPO_INFORME tipoInforme, cJSON *comandoOriginal);

/**
 * @fn esp_err_t appuser_load_schedule_extra_data(DATOS_APLICACION*, TIME_PROGRAM*, cJSON*)
 * @brief Carga en la estructura de schedules los parametros del programa específicos al tipo de dispositivo.
 *
 * @param datosApp
 * @param programa_actual
 * @param nodo
 * @return
 */
esp_err_t appuser_load_schedule_extra_data(DATOS_APLICACION *datosApp, TIME_PROGRAM *programa_actual, cJSON *nodo);
#endif /* COMPONENTS_INTERFAZ_USUARIO_INCLUDE_INTERFAZ_USUARIO_H_ */
