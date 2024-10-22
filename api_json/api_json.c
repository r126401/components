/*
 * api_json.c
 *
 *  Created on: 21 sept. 2020
 *      Author: t126401
 */



#include "dialogos_json.h"
#include "api_json.h"
#include "cJSON.h"
#include "esp_err.h"
#include "configuracion.h"
#include "conexiones.h"
#include "esp_wifi.h"
#include "programmer.h"
#include "conexiones_mqtt.h"
#include "esp_system.h"
#include "user_interface.h"
#include "esp_timer.h"
#include "espota.h"
#include "events_device.h"
#include "alarmas.h"
#include "nvslib.h"
#include "applib.h"




#define DELAY_TIME_RESET 3000 //ms o 3s

static const char *TAG = "api_json.c";
esp_timer_handle_t tiempo;


char* report_2_mnemonic(SPONTANEOUS_TYPE report) {

	static char mnemonic[50] = {0};


	switch (report) {

	case STARTED:
		strcpy(mnemonic, "START APPLICATION");
	break;
	case OP_LOCAL_RELAY:
		strcpy(mnemonic, "ACTION LOCAL RELAY");
	break;
	case OP_REMOTE_RELAY:
		strcpy(mnemonic, "ACTION REMOTE RELAY");
	break;
	case UPGRADE_FIRMWARE_FOTA:
		strcpy(mnemonic, "UPGRADE FIRMWARE");
	break;
	case START_SCHEDULE:
		strcpy(mnemonic, "START SCHEDULE");
	break;
	case APPLICATION_COMMAND:
		strcpy(mnemonic, "APPLICATION COMMAND");
	break;
	case CHANGE_TEMPERATURE:
		strcpy(mnemonic, "CHANGE TEMPERATURE");
	break;
	case STATUS_REPORT:
		strcpy(mnemonic, "STATUS REPORT");
	break;
	case END_SCHEDULE:
		strcpy(mnemonic, "END SCHEDULE");
	break;
	case ALARM_REPORT:
		strcpy(mnemonic, "ALARM REPORT");
	break;
	case CAMBIO_UMBRAL_TEMPERATURA:
		strcpy(mnemonic, "CHANGE THRESHOLD TEMPERATURE");
	break;
	case CAMBIO_ESTADO_APLICACION:
		strcpy(mnemonic, "CHANGE THRESHOLD CAMBIO ESTADO_APLICACION");
	break;

	case ERROR_REPORT:
		strcpy(mnemonic, "ERROR");
	break;
	case START_UPGRADE_OTA:
		strcpy(mnemonic, "START UPGRADE OTA");
		break;
	case END_UPGRADE_OTA:
		strcpy(mnemonic, "END UPGRADE OTA");
		break;
	case ERROR_UPGRADE_OTA:
		strcpy(mnemonic, "ERROR UPGRADE OTA");
		break;
	case DEVICE_RECOVERED:
		strcpy(mnemonic, "ERROR DEVICE_RECOVERED");
		break;



	}

	return mnemonic;


}


esp_err_t   comandoDesconocido(DATOS_APLICACION *datosApp, cJSON *respuesta) {


    codigoRespuesta(respuesta, RESP_NOK);


    return ESP_OK;
}



void   codigoRespuesta(cJSON *respuesta, enum ANSWER_CODE codigo) {

    cJSON_AddNumberToObject(respuesta, DLG_COD_RESPUESTA, codigo);
    return;
}



cJSON*  analizar_comando(DATOS_APLICACION *datosApp, char* info) {


	ESP_LOGI(TAG, ""TRAZAR"Entramos en analizar_comando", INFOTRAZA);
    cJSON *idComando = NULL;
    cJSON *peticion = NULL;
    cJSON *respuesta = NULL;
    enum COMMAND_TYPE com;
    //time_t t_siguiente_intervalo;
    //1.- Analizar la peticion y vemos que comando nos piden.


    peticion = cJSON_Parse(info);
    if (peticion == NULL) {
        com = NO_JSON;
        free(peticion);
    } else {
        idComando = cJSON_GetObjectItem(peticion, COMANDO);
        if (idComando == NULL) {
            com = UNKNOWN_COMMAND;
        } else {
            idComando = cJSON_GetObjectItem(idComando, DLG_COMANDO);
            if (idComando == NULL) {
                com = NO_IMPLEMENTED;
            } else {
                com = idComando->valueint;
            }
        }

    }
    ESP_LOGW(TAG, ""TRAZAR"Memoria libre(3) Se ha creado el objeto peticion: "CONFIG_UINT32_FORMAT"\n", INFOTRAZA, esp_get_free_heap_size());
    respuesta = cabeceraRespuestaComando(datosApp, peticion);
    if (respuesta == NULL) {
    	return NULL;
    }
    ESP_LOGW(TAG, ""TRAZAR"Memoria libre(4) Se ha creado la respuesta: "CONFIG_UINT32_FORMAT"\n", INFOTRAZA, esp_get_free_heap_size());
    if (com >= 50) {
        appuser_set_command_application(peticion, idComando->valueint, datosApp, respuesta);
        ESP_LOGW(TAG, ""TRAZAR"Memoria libre(5). Se aumentan los campos de la cabecera: "CONFIG_UINT32_FORMAT"\n", INFOTRAZA, esp_get_free_heap_size());
        printf("analizarComando-->comando de aplicacion ejecutado!!\n");
    } else {

        //4.- En funcion del comando ejecutamos una cosa u otra
        switch (com) {
            case DISPLAY_APP_CONF: //ok--- con cabecera
            	visualizar_datos_aplicacion(datosApp, respuesta);
                //AppJsonComun(datosApp, respuesta);
                break;
            case DISPLAY_MQTT_CONF: //probada ok--- con cabecera
                 //Visualizar la configuracion mqtt de aplicacion
            	visualizar_configuracion_mqtt(datosApp, respuesta);
            	break;

            case DISPLAY_WIFI_CONF: // probado ok--- con cabecera
                //Visualizar la configuracion wifi
                visualizar_datos_wifi(datosApp, respuesta);
                break;
            case DISPLAY_SCHEDULES: // probado ok--- con cabecera
                visualizar_programas(datosApp, respuesta);
                break;
            case INSERT_SCHEDULE: // probado ok-- con construirCabecera
            	insertar_nuevo_programa(peticion, datosApp, respuesta);
            	//calcular_programa_activo(datosApp, &t_siguiente_intervalo);
                break;
            case DELETE_SCHEDULE: // probado ok-- con construirCabecera
                 borrar_programa(peticion, datosApp, respuesta);
                 //calcular_programa_activo(datosApp, &t_siguiente_intervalo);
                 break;
            case MODIFY_SCHEDULE: // probado ok-- con construirCabecera
            	modificar_programa(peticion, datosApp, respuesta);
            	//calcular_programa_activo(datosApp, &t_siguiente_intervalo);
                break;
            case RESET: // probado ok-- con cabecera
                ejecutar_reset(datosApp, respuesta);
                break;
            case FACTORY_RESET: // probado ok-- con cabecera
                ejecutar_factory_reset(datosApp, respuesta);
                break;
            case UPGRADE_FIRMWARE_OTA:
                upgrade_ota(peticion, datosApp, respuesta);
                break;
            case DISPLAY_ACTIVE_ALARMS:
            	visualizar_alarmas_activas(datosApp, respuesta);
            	break;
            case MODIFY_APP_CONF:
            	modificar_configuracion_app(peticion, datosApp, respuesta);
                break;

            default:
            	visualizar_comando_desconocido(datosApp, respuesta);
            	break;
        }
    }




    cJSON_Delete(peticion);
    ESP_LOGW(TAG, ""TRAZAR"Memoria libre(6) Se libera el json de la peticion: "CONFIG_UINT32_FORMAT"\n", INFOTRAZA, esp_get_free_heap_size());
	return respuesta;
}


MESSAGE_TYPE select_topic_client(DATOS_APLICACION *datosApp, char* topic, int *index) {


	int i;

	ESP_LOGI(TAG, ""TRAZAR"topic: %s, topic: %s", INFOTRAZA, datosApp->datosGenerales->parametrosMqtt.topics[0].subscribe, topic);

	if (strcmp(datosApp->datosGenerales->parametrosMqtt.topics[0].subscribe, topic) == 0) {

		return APPLICATION_MESSAGE;

	}


	if (strcmp(datosApp->datosGenerales->parametrosMqtt.topics[1].subscribe, topic) == 0) {

		return DEBUG_MESSAGE;

	}


	for (i=0;i < CONFIG_NUM_TOPICS; i++) {

		if (strcmp(datosApp->datosGenerales->parametrosMqtt.topics[i].subscribe, topic) == 0) {
			return APPLICATION_DEVICE_MESSAGE;
		}
	}


	return UNKNOWN_MESSAGE;

}


void process_application_message(DATOS_APLICACION *datosApp, char* peticion) {


	 cJSON *root = NULL;


	 ESP_LOGE(TAG, ""TRAZAR"process_application_message: Memoria libre(1): "CONFIG_UINT32_FORMAT"\n", INFOTRAZA, esp_get_free_heap_size());
	 root = analizar_comando(datosApp, peticion);
	 if (root != NULL) {
		 free(peticion);
		 publicar_mensaje_json(datosApp, root, get_app_publish_topic(datosApp, 0) );
		 //cJSON_Delete(root);
	 } else {
		 ESP_LOGE(TAG, ""TRAZAR"process_application_message: Error al procesar la peticion", INFOTRAZA);
	 }
	 ESP_LOGE(TAG, ""TRAZAR"Memoria despues: "CONFIG_UINT32_FORMAT"\n", INFOTRAZA, esp_get_free_heap_size());


}

void process_debug_message(DATOS_APLICACION *datosApp, char *message) {


	free(message);
}

void process_application_device_message(DATOS_APLICACION *datosApp, char *message) {

	appuser_received_application_device_message(datosApp, message);
	free(message);

}


void process_unknown_message(DATOS_APLICACION *datosApp, char *message) {


	free(message);
}


 void  message_application_received(DATOS_APLICACION *datosApp, char *topic) {


	 char* message = NULL;

	 MESSAGE_TYPE message_type = UNKNOWN_MESSAGE;
	 int index;

	 ESP_LOGE(TAG, ""TRAZAR"Memoria libre(1): "CONFIG_UINT32_FORMAT"\n", INFOTRAZA, esp_get_free_heap_size());

	 if(datosApp->handle_mqtt->data_len == 0) {
		 ESP_LOGW(TAG, ""TRAZAR"MENSAJE VACIO", INFOTRAZA);
		 return;
	 }
	 message = (char*) calloc((datosApp->handle_mqtt->data_len + 1), sizeof(char));
	 strncpy(message,datosApp->handle_mqtt->data, datosApp->handle_mqtt->data_len);
	 ESP_LOGI(TAG, ""TRAZAR"Recibido %s, longitud %d", INFOTRAZA, message, datosApp->handle_mqtt->data_len);

	 message_type = select_topic_client(datosApp, topic, &index);
	 ESP_LOGI(TAG, ""TRAZAR"TIPO DE MENSAJE :%d", INFOTRAZA, message_type);

	 switch (message_type) {

	 case APPLICATION_MESSAGE:
		 process_application_message(datosApp, message);
		 break;
	 case DEBUG_MESSAGE:
		 process_debug_message(datosApp, message);
		 break;
	 case APPLICATION_DEVICE_MESSAGE:
		 process_application_device_message(datosApp, message);
		 break;
	 case UNKNOWN_MESSAGE:
		 process_unknown_message(datosApp, message);
		 break;

	 }


 }


 cJSON   *cabeceraRespuestaComando(DATOS_APLICACION *datosApp, cJSON *peticion) {

     cJSON* clave = NULL;
     cJSON *parteComando = NULL;
     cJSON *respuesta = NULL;
     cJSON *com = NULL;

     respuesta = cabeceraGeneral(datosApp);
     if (respuesta == NULL) {
    	 return NULL;
     }
     if (peticion != NULL) {
         parteComando = cJSON_GetObjectItem(peticion, COMANDO);
         if (parteComando != NULL) {
             clave = cJSON_GetObjectItem(parteComando, DLG_KEY);
             //cJSON_AddItemToObject(respuesta, DLG_KEY, clave);
             if (clave != NULL) cJSON_AddStringToObject(respuesta, DLG_KEY, clave->valuestring);
         } else {
             return respuesta;
         }
     }else {
         ESP_LOGE(TAG, ""TRAZAR"json invalido\n", INFOTRAZA);
         //printf("cabeceraRespuestaComando--> cabecera defecto_ %s\n", cJSON_Print(respuesta));
         return respuesta;
     }
     com = cJSON_GetObjectItem(parteComando, DLG_COMANDO);
     if (com != NULL) {
         cJSON_AddNumberToObject(respuesta, DLG_COMANDO, com->valueint);

     }
     return respuesta;
 }

 cJSON*  cabeceraGeneral(DATOS_APLICACION *datosApp) {

     char fecha[100];
     cJSON *respuesta = NULL;
     char *mac = NULL;

     if (!using_schedules(datosApp)) {
    	 ESP_LOGW(TAG, ""TRAZAR" Se actualiza la hora ya que no esta activado el schedule", INFOTRAZA);
    	 actualizar_hora(&datosApp->datosGenerales->clock);
     }

     sprintf(fecha, "%02d/%02d/%d %02d:%02d:%02d",
        datosApp->datosGenerales->clock.date.tm_mday,datosApp->datosGenerales->clock.date.tm_mon+1, datosApp->datosGenerales->clock.date.tm_year+1900,
        datosApp->datosGenerales->clock.date.tm_hour, datosApp->datosGenerales->clock.date.tm_min, datosApp->datosGenerales->clock.date.tm_sec);
     if ((mac = get_my_id()) == NULL) {
    	 return NULL;
     }
     respuesta = cJSON_CreateObject();
     cJSON_AddStringToObject(respuesta, ID_DEVICE, mac);
     cJSON_AddNumberToObject(respuesta, DEVICE, datosApp->datosGenerales->tipoDispositivo);
     cJSON_AddStringToObject(respuesta, OTA_SW_VERSION, datosApp->datosGenerales->ota.swVersion->version);
     cJSON_AddStringToObject(respuesta, DATE, fecha);
     return respuesta;


 }


 int   extraer_dato_tm(char *id_programa, int desplazamiento, uint8_t tamano) {

 	char dato_temporal[5] = {0};
 	int dato;
 	memcpy(dato_temporal, id_programa+desplazamiento, tamano);
 	dato = atoi(dato_temporal);
 	ESP_LOGI(TAG, ""TRAZAR"El dato con desplazamiento %d es %d", INFOTRAZA, desplazamiento, dato);
 	return dato;
 }


 esp_err_t extraer_dato_bool(cJSON *nodo, char *nombre_campo, bool *dato) {

     cJSON *campo = NULL;
     campo =cJSON_GetObjectItem(nodo, nombre_campo);
     if (campo == NULL) {
    	 ESP_LOGI(TAG, ""TRAZAR"extraer_dato_bool--> campo %s invalido", INFOTRAZA, nombre_campo);
    	 return ESP_FAIL;
     }

     if (cJSON_IsBool(campo)) {
    	 if (cJSON_IsFalse(campo)) {
        	 *dato = false;
        	 ESP_LOGI(TAG, ""TRAZAR"extraer_dato_bool--> campo %s es false %d", INFOTRAZA, nombre_campo, campo->valueint);
    	 } else {
        	 *dato = true;
        	 ESP_LOGI(TAG, ""TRAZAR"extraer_dato_bool--> campo %s es true %d", INFOTRAZA, nombre_campo, campo->valueint);
         }

     }  else {
    	 ESP_LOGI(TAG, ""TRAZAR"extraer_dato_bool--> El campo %s no es booleaneo %d", INFOTRAZA, nombre_campo, campo->valueint);
     }

     return ESP_OK;






 }


 esp_err_t   extraer_dato_string(cJSON *nodo, char *nombre_campo, char* dato) {

     cJSON *campo = NULL;
     campo =cJSON_GetObjectItem(nodo, nombre_campo);
     if((campo != NULL) && (campo->type == cJSON_String)) {
     	strcpy(dato, campo->valuestring);
         ESP_LOGI(TAG, ""TRAZAR"extraerDatoString--> campo %s = %s\n", INFOTRAZA, nombre_campo, dato);
         return ESP_OK;
     } else {
     	ESP_LOGW(TAG, ""TRAZAR"No se ha podido extraer el dato %s que aparece de tipo %d", INFOTRAZA, nombre_campo, campo->type);
     	strcpy(dato, "NULL");
     	return ESP_FAIL;
     }
 }

 esp_err_t   extraer_dato_int(cJSON *nodo, char* nombre_campo, int *dato) {


     cJSON *campo = NULL;
     campo = cJSON_GetObjectItem(nodo, nombre_campo);
     if((campo != NULL) && (campo->type == cJSON_Number)) {
        *dato = campo->valueint;
        ESP_LOGI(TAG, ""TRAZAR"extraer_dato_int--> campo %s = %d\n", INFOTRAZA, nombre_campo, *dato);
        return ESP_OK;
     } else {
         ESP_LOGW(TAG, ""TRAZAR"extraer_dato_int-->campo %s no aparece\n", INFOTRAZA, nombre_campo);
         return ESP_FAIL;
     }

 }

 esp_err_t   extraer_dato_uint8(cJSON *nodo, char* nombre_campo, uint8_t *dato) {


     cJSON *campo = NULL;
     campo = cJSON_GetObjectItem(nodo, nombre_campo);
     if((campo != NULL) && ((campo->type == cJSON_Number) || (campo->type == cJSON_False) || (campo->type == cJSON_True))) {
        *dato = (uint8_t) campo->valueint;
        ESP_LOGI(TAG, ""TRAZAR"extraer_dato_int--> campo %s = %d\n", INFOTRAZA, nombre_campo, *dato);
        return ESP_OK;
     } else {
         ESP_LOGW(TAG, ""TRAZAR"extraer_dato_int-->campo %s no aparece\n", INFOTRAZA, nombre_campo);
         return ESP_FAIL;
     }

 }
 esp_err_t   extraer_dato_uint32(cJSON *nodo, char* nombre_campo, uint32_t *dato) {


     cJSON *campo = NULL;
     campo = cJSON_GetObjectItem(nodo, nombre_campo);
     if((campo != NULL) && (campo->type == cJSON_Number)) {
        *dato = (uint32_t) campo->valueint;
        ESP_LOGI(TAG, ""TRAZAR"extraer_dato_int--> campo %s = "CONFIG_UINT32_FORMAT"\n", INFOTRAZA, nombre_campo, *dato);
        return ESP_OK;
     } else {
         ESP_LOGW(TAG, ""TRAZAR"extraer_dato_int-->campo %s no aparece\n", INFOTRAZA, nombre_campo);
         return ESP_FAIL;
     }

 }


 esp_err_t 	extraer_dato_float(cJSON *nodo, char *nombre_campo, float *dato) {

	 double dato_double;

	 char cadena[10] = {0};

	 if ((extraer_dato_double(nodo, nombre_campo, &dato_double) == ESP_OK)) {
		 sprintf(cadena, "%02lf", dato_double);
		 *dato = atoff(cadena);
		 ESP_LOGI(TAG, ""TRAZAR"extraer_dato_float--> campo %s = %.02f\n", INFOTRAZA, nombre_campo, *dato);
		 return ESP_OK;

	 } else {
		 ESP_LOGW(TAG, ""TRAZAR"extraer_dato_float-->campo %s no aparece\n", INFOTRAZA, nombre_campo);
		 return ESP_FAIL;
	 }
 }

 esp_err_t   extraer_dato_double(cJSON *nodo, char *nombre_campo, double *dato) {


     cJSON *campo = NULL;
     campo =cJSON_GetObjectItem(nodo, nombre_campo);
     if((campo != NULL) && (campo->type == cJSON_Number)) {
         *dato = campo->valuedouble;
         ESP_LOGI(TAG, ""TRAZAR"extraer_dato_double--> campo %s = %f\n", INFOTRAZA, nombre_campo, *dato);
         return ESP_OK;
     } else {
     	ESP_LOGW(TAG, ""TRAZAR"extraer_dato_double-->campo %s no aparece\n", INFOTRAZA, nombre_campo);
     	return ESP_FAIL;
     }
 }




 uint32_t   hex2int(char *hex) {
     uint32_t val = 0;
     while (*hex) {
         // get current character then increment
         uint8_t byte = *hex++;
         // transform hex character to the 4bit equivalent number, using the ascii table indexes
         if (byte >= '0' && byte <= '9') byte = byte - '0';
         else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
         else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
         // shift 4 to make space for new digit, and add the 4 bits of the new digit
         val = (val << 4) | (byte & 0xF);
     }
     return val;
 }

 int   extraer_dato_hex(char* texto, int desplazamiento, uint8_t tamano) {

 	char dato_temporal[5] = {0};
 	int dato = 0;
 	ESP_LOGI(TAG, ""TRAZAR"dato temporal es %s", INFOTRAZA, dato_temporal);
 	memcpy(dato_temporal, texto+desplazamiento, tamano);
 	dato = hex2int(dato_temporal);
 	ESP_LOGI(TAG, ""TRAZAR"El dato con desplazamiento %d es %d", INFOTRAZA, desplazamiento, dato);

 	return dato;
 }
 esp_err_t   visualizar_datos_aplicacion(struct DATOS_APLICACION *datosApp, cJSON *respuesta) {



     cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
     cJSON_AddNumberToObject(respuesta, NUMBER_PROGRAMS, datosApp->datosGenerales->nProgramacion);
     cJSON_AddStringToObject(respuesta, MQTT_BROKER, datosApp->datosGenerales->parametrosMqtt.broker);
     cJSON_AddNumberToObject(respuesta, MQTT_PORT, datosApp->datosGenerales->parametrosMqtt.port);
     cJSON_AddStringToObject(respuesta, MQTT_USER, datosApp->datosGenerales->parametrosMqtt.user);
     cJSON_AddStringToObject(respuesta, MQTT_PASS, datosApp->datosGenerales->parametrosMqtt.password);
     cJSON_AddStringToObject(respuesta, MQTT_PUBLISH, datosApp->datosGenerales->parametrosMqtt.publish);
     cJSON_AddStringToObject(respuesta, MQTT_SUBSCRIBE, datosApp->datosGenerales->parametrosMqtt.subscribe);
     cJSON_AddNumberToObject(respuesta, FREE_MEMORY, esp_get_free_heap_size());
     cJSON_AddNumberToObject(respuesta, "uptime", esp_timer_get_time());
     //cJSON_AddNumberToObject(respuesta, UPTIME, uptime);
     codigoRespuesta(respuesta, RESP_OK);
     printf("AppJsonComun--> vamos a imprimir\n");

     return ESP_OK;
 }

esp_err_t visualizar_datos_wifi(DATOS_APLICACION *datosApp, cJSON *respuesta) {

     wifi_config_t config;

     esp_wifi_get_config(WIFI_IF_STA, &config);
     cJSON_AddStringToObject(respuesta, CONF_WIFI_ESSID, (char*) config.sta.ssid);
     cJSON_AddStringToObject(respuesta, CONF_WIFI_PASS, (char*) config.sta.password);
     codigoRespuesta(respuesta, RESP_OK);
     return ESP_OK;


 }

esp_err_t   visualizar_programas(struct DATOS_APLICACION *datosApp, cJSON *respuesta) {

	cJSON *array = NULL;

    escribir_programa_actual(datosApp, respuesta);
    cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
    cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
	array = cJSON_CreateArray();

    crear_programas_json(datosApp, array);
    cJSON_AddItemToObject(respuesta, PROGRAM, array);
    codigoRespuesta(respuesta, RESP_OK);

    return ESP_OK;



}

esp_err_t   escribir_programa_actual(DATOS_APLICACION *datosApp, cJSON *respuesta) {

	NTP_CLOCK hora;
	actualizar_hora(&hora);



	if (get_current_status_application(datosApp) == SCHEDULING) {
		cJSON_AddStringToObject(respuesta, CURRENT_PROGRAM_ID, datosApp->datosGenerales->programacion[datosApp->datosGenerales->nProgramaCandidato].idPrograma);
		ESP_LOGW(TAG, ""TRAZAR" programa candidato vale ok %d", INFOTRAZA, datosApp->datosGenerales->nProgramaCandidato);

	} else {
		ESP_LOGW(TAG, ""TRAZAR"NO HAY PROGRAMA ACTIVO", INFOTRAZA);
	}

	return ESP_OK;
	/*
	schedule = datosApp->datosGenerales->programacion;
	indice = datosApp->datosGenerales->nProgramaCandidato;
	if (datosApp->datosGenerales->nProgramacion > 0){
		if (datosApp->datosGenerales->nProgramaCandidato >= 0) {
			if ((schedule[indice].programa + schedule[indice].duracion) <= hora_actual) {
				ESP_LOGE(TAG, ""TRAZAR" no se pinta el programa actual ya que excedio la duracion del programa", INFOTRAZA);
			} else {
				cJSON_AddStringToObject(respuesta, CURRENT_PROGRAM_ID, datosApp->datosGenerales->programacion[datosApp->datosGenerales->nProgramaCandidato].idPrograma);
				ESP_LOGW(TAG, ""TRAZAR" programa candidato vale ok %d", INFOTRAZA, datosApp->datosGenerales->nProgramaCandidato);
			}
		} else {
			ESP_LOGW(TAG, ""TRAZAR" programa candidato vale %d", INFOTRAZA, datosApp->datosGenerales->nProgramaCandidato);
		}


	} else {
		ESP_LOGW(TAG, ""TRAZAR"NO SE PUEDE ESCRIBIR EL PROGRAMA ACTUAL PORQUE NO HAY PROGRAMAS REGISTRADOS O EL PROGRAMA VALE %d", INFOTRAZA, datosApp->datosGenerales->nProgramaCandidato );
	}

    return ESP_OK;
    */
}

esp_err_t   visualizar_configuracion_mqtt(DATOS_APLICACION *datosApp, cJSON *respuesta) {

    cJSON_AddStringToObject(respuesta, MQTT_BROKER, datosApp->datosGenerales->parametrosMqtt.broker);
    cJSON_AddNumberToObject(respuesta, MQTT_PORT, datosApp->datosGenerales->parametrosMqtt.port);
    cJSON_AddStringToObject(respuesta, MQTT_USER, datosApp->datosGenerales->parametrosMqtt.user);
    cJSON_AddStringToObject(respuesta, MQTT_PASS, datosApp->datosGenerales->parametrosMqtt.password);
    cJSON_AddStringToObject(respuesta, MQTT_PUBLISH, datosApp->datosGenerales->parametrosMqtt.publish);
    cJSON_AddStringToObject(respuesta, MQTT_SUBSCRIBE, datosApp->datosGenerales->parametrosMqtt.subscribe);
    codigoRespuesta(respuesta, RESP_OK);

    return ESP_OK;


}
esp_err_t   visualizar_comando_desconocido(DATOS_APLICACION *datosApp, cJSON *respuesta) {


    codigoRespuesta(respuesta, RESP_NOK);


    return true;
}

esp_err_t   crear_programas_json(DATOS_APLICACION *datosApp, cJSON *respuesta) {

	cJSON *array;
	cJSON *item;
	int i;
    char dato[30] = {0};

    array = respuesta;


    for (i=0;i<datosApp->datosGenerales->nProgramacion;i++) {
        cJSON_AddItemToArray(array, item = cJSON_CreateObject());
        sprintf(dato, "%s%01d%01d",datosApp->datosGenerales->programacion[i].idPrograma,
                datosApp->datosGenerales->programacion[i].estadoPrograma,
                datosApp->datosGenerales->programacion[i].accion);

        ESP_LOGW(TAG, ""TRAZAR"PROGRAMID: %s estado: %d, accion: %01d)", INFOTRAZA, datosApp->datosGenerales->programacion[i].idPrograma, datosApp->datosGenerales->programacion[i].estadoPrograma,
                datosApp->datosGenerales->programacion[i].accion);

        cJSON_AddStringToObject(item, PROGRAM_ID, dato);
        appuser_reporting_schedule_extra_data(&datosApp->datosGenerales->programacion[i], item);
        /*
        if (datosApp->datosGenerales->programacion[i].duracion > 0) {
            cJSON_AddNumberToObject(item,DURATION_PROGRAM, datosApp->datosGenerales->programacion[i].duracion );
        }
        switch(datosApp->datosGenerales->tipoDispositivo) {

        case INTERRUPTOR:
        	break;
        case CRONOTERMOSTATO:
            cJSON_AddNumberToObject(item,UMBRAL_TEMPERATURA, datosApp->datosGenerales->programacion[i].temperatura );
        	break;
        case TERMOMETRO:
        	break;
        default:
        	break;
        }
        */

    }


        return ESP_OK;

}

esp_err_t   insertar_nuevo_programa(cJSON *peticion,DATOS_APLICACION *datosApp, cJSON *respuesta) {

   cJSON *nodo;
   TIME_PROGRAM *programaActual;
   uint8_t elements;



   elements= datosApp->datosGenerales->nProgramacion;

   printf("funcion insertarProgramacion\n");
   nodo = cJSON_GetObjectItem(peticion, PROGRAM);
   if(nodo == NULL) {
       printf("program no encontrado\n");
       codigoRespuesta(respuesta, RESP_NOK);
       return false;

   }

   //Revision de campos para crear la nueva programacion
   programaActual = (TIME_PROGRAM*) calloc(1, sizeof(TIME_PROGRAM));
   extraer_dato_int(nodo, PROGRAM_TYPE, (int*) &programaActual->tipo);
   extraer_dato_int(nodo, HOUR, &programaActual->programacion.tm_hour);
   extraer_dato_int(nodo, MINUTE, &programaActual->programacion.tm_min);
   extraer_dato_int(nodo, SECOND, &programaActual->programacion.tm_sec);
   extraer_dato_int(nodo, YEAR, &programaActual->programacion.tm_yday);
   extraer_dato_int(nodo, MONTH, &programaActual->programacion.tm_mon);
   extraer_dato_int(nodo, DAY, &programaActual->programacion.tm_mday);
   if(extraer_dato_int(nodo, WEEKDAY, &programaActual->programacion.tm_wday) != ESP_OK) {
	   programaActual->programacion.tm_wday = 0;
   }
   /*
   if (programaActual->programacion.tm_wday = extraer_dato_int(nodo, WEEKDAY, &) == -1000) {
       programaActual->programacion.tm_wday = 0;
   }*/
   extraer_dato_int(nodo, PROGRAM_STATE, (int*) &programaActual->estadoPrograma);
   extraer_dato_int(nodo, PROGRAM_STATE, (int*) &programaActual->activo);
   //datosApp->datosGenerales->estadoApp = NORMAL_AUTO;
   extraer_dato_uint8(nodo, PROGRAM_ACTION, &programaActual->accion);
   extraer_dato_uint8(nodo, PROGRAM_MASK, &programaActual->mascara);
   appuser_get_schedule_extra_data(programaActual, nodo);
   generarIdPrograma(programaActual);
   datosApp->datosGenerales->programacion = crearPrograma(programaActual, datosApp->datosGenerales->programacion, &datosApp->datosGenerales->nProgramacion);


   printf("borrarProgramacion--> (1)\n");
   if (datosApp->datosGenerales->nProgramacion == elements) {
       codigoRespuesta(respuesta, RESP_NOK);
         printf("borrarProgramacion--> (1')\n");
       return false;


   } else {
       cJSON_AddStringToObject(respuesta, PROGRAM_ID, programaActual->idPrograma);
       cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
       cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
       //appuser_notify_app_status(datosApp, NORMAL_SINCRONIZANDO);
       //datosApp->datosGenerales->estadoApp = NORMAL_SINCRONIZANDO;
       //gestion_programas(datosApp);
       escribir_programa_actual(datosApp, respuesta);
       codigoRespuesta(respuesta, RESP_OK);
       send_event(__func__,EVENT_INSERT_SCHEDULE);

   }
   guardar_programas(datosApp, CONFIG_CLAVE_PROGRAMACION);

   return ESP_OK;

}

esp_err_t   borrar_programa(cJSON *peticion,struct DATOS_APLICACION *datosApp, cJSON *respuesta) {

   cJSON *nodo, *campo;
   char idPrograma[20] = {0};
   int nPrograma;




   //Buscamos el nodo programa
   printf("borrarProgramacion-->borrar programa\n");
   nodo = cJSON_GetObjectItem(peticion, PROGRAM);
   if(nodo == NULL) {
       codigoRespuesta(respuesta, RESP_NOK);
       return false;
   }


   campo = cJSON_GetObjectItem(nodo, PROGRAM_ID);
   if((campo != NULL) && (campo->type == cJSON_String)) {
       printf("idPrograma vale %s\n", campo->valuestring);
       strcpy(idPrograma, campo->valuestring);
   }
   //Localizamos el programa
   nPrograma = localizarProgramaPorId(idPrograma, datosApp->datosGenerales->programacion, datosApp->datosGenerales->nProgramacion);


   if (nPrograma == -1) {

       codigoRespuesta(respuesta, RESP_NOK);
       return false;


   } else {

       //borramos el programa
       datosApp->datosGenerales->programacion = borrarPrograma(datosApp->datosGenerales->programacion, &datosApp->datosGenerales->nProgramacion, nPrograma);
       if (datosApp->datosGenerales->nProgramacion == 0) {
           //datosApp->datosGenerales->estadoApp = NORMAL_SIN_PROGRAMACION;
           //appuser_notify_app_status(datosApp, NO_PROGRAM);
           send_event(__func__, EVENT_NO_ACTIVE_SCHEDULE);
       }
       cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
       cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
       printf("idprograma borrado:%s\n", idPrograma);
       cJSON_AddStringToObject(respuesta, PROGRAM_ID, idPrograma);
       send_event(__func__,EVENT_DELETE_SCHEDULE);
       //datosApp->datosGenerales->estadoApp = NORMAL_SINCRONIZANDO;
       //appuser_notify_app_status(datosApp, NORMAL_SINCRONIZANDO);
       //gestion_programas(datosApp);
       escribir_programa_actual(datosApp, respuesta);
       codigoRespuesta(respuesta, RESP_OK);

   }

   guardar_programas(datosApp, CONFIG_CLAVE_PROGRAMACION);
   return ESP_OK;
}

esp_err_t   modificar_programa(cJSON *peticion,struct DATOS_APLICACION *datosApp, cJSON *respuesta) {


	cJSON *campo;
    cJSON *nodo;
    //TIME_PROGRAM programaActual;
    char idPrograma[19];
    int nPrograma;




    //Buscamos el nodo programa
    nodo = cJSON_GetObjectItem(peticion, PROGRAM);
    if(nodo == NULL) {
        printf("modificarProgramacion-->program no encontrado\n");
        codigoRespuesta(respuesta, RESP_NOK);
        return ESP_FAIL;
    }


    //extraer_dato_string(nodo, PROGRAM_ID, idPrograma);

    campo = cJSON_GetObjectItem(nodo, PROGRAM_ID);
    if((campo != NULL) && (campo->type == cJSON_String)) {
        printf("modificarProgramacion-->idPrograma vale %s\n", campo->valuestring);
        strcpy(idPrograma, campo->valuestring);
    }

    //Localizamos el programa
    nPrograma = localizarProgramaPorId(idPrograma, datosApp->datosGenerales->programacion, datosApp->datosGenerales->nProgramacion);


    if (nPrograma == -1) {
        codigoRespuesta(respuesta, RESP_NOK);
        return false;

    } else {
    	extraer_dato_int(nodo, YEAR, &datosApp->datosGenerales->programacion[nPrograma].programacion.tm_year);
    	extraer_dato_int(nodo, MONTH, &datosApp->datosGenerales->programacion[nPrograma].programacion.tm_mon);
    	extraer_dato_int(nodo, DAY, &datosApp->datosGenerales->programacion[nPrograma].programacion.tm_mday);
    	extraer_dato_int(nodo, HOUR, &datosApp->datosGenerales->programacion[nPrograma].programacion.tm_hour);
    	extraer_dato_int(nodo, MINUTE, &datosApp->datosGenerales->programacion[nPrograma].programacion.tm_min);
    	extraer_dato_int(nodo, SECOND, &datosApp->datosGenerales->programacion[nPrograma].programacion.tm_sec);
    	extraer_dato_int(nodo, WEEKDAY, &datosApp->datosGenerales->programacion[nPrograma].programacion.tm_wday);
    	extraer_dato_int(nodo, PROGRAM_TYPE, (int*) &datosApp->datosGenerales->programacion[nPrograma].tipo);
    	extraer_dato_int(nodo, PROGRAM_STATE, (int*) &datosApp->datosGenerales->programacion[nPrograma].estadoPrograma);
    	extraer_dato_uint8(nodo, PROGRAM_ACTION, &datosApp->datosGenerales->programacion[nPrograma].accion);
    	extraer_dato_uint8(nodo, PROGRAM_MASK, &datosApp->datosGenerales->programacion[nPrograma].mascara);
        appuser_modify_schedule_extra_data(&datosApp->datosGenerales->programacion[nPrograma], nodo);

        generarIdPrograma(&datosApp->datosGenerales->programacion[nPrograma]);
        cJSON_AddStringToObject(respuesta, PROGRAM_ID, idPrograma);
        cJSON_AddStringToObject(respuesta, NEW_PROGRAM_ID, datosApp->datosGenerales->programacion[nPrograma].idPrograma);
        cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
        cJSON_AddNumberToObject(respuesta, PROGRAM_STATE, datosApp->datosGenerales->programacion[nPrograma].estadoPrograma);
        cJSON_AddNumberToObject(respuesta, PROGRAM_ACTION, datosApp->datosGenerales->programacion[nPrograma].accion);
        appuser_reporting_schedule_extra_data(&datosApp->datosGenerales->programacion[nPrograma], respuesta);

        send_event(__func__,EVENT_MODIFY_SCHEDULE);
        //datosApp->datosGenerales->estadoApp = NORMAL_SINCRONIZANDO;
        //appuser_notify_app_status(datosApp, NORMAL_SINCRONIZANDO);
        //gestion_programas(datosApp);
        escribir_programa_actual(datosApp, respuesta);
        codigoRespuesta(respuesta, RESP_OK);
    }

    guardar_programas(datosApp, CONFIG_CLAVE_PROGRAMACION);


    return ESP_OK;
}

cJSON*   cabecera_espontaneo(DATOS_APLICACION *datosApp, enum SPONTANEOUS_TYPE tipo_report) {

    cJSON *respuesta;
    respuesta = cabeceraGeneral(datosApp);
    if (respuesta == NULL) {
    	ESP_LOGE(TAG, ""TRAZAR"CABECERA NULA", INFOTRAZA);
    	return NULL;
    } else {
    	cJSON_AddNumberToObject(respuesta, TIPO_REPORT, tipo_report);
    }

    return respuesta;

}

void restart_normal(void* parametros) {
    esp_restart();
}

esp_err_t   ejecutar_reset(DATOS_APLICACION *datosApp, cJSON *respuesta) {


    const esp_timer_create_args_t timer_reset_args = {
    		.callback = &restart_normal,
			.name = "timer reset",
			.arg = (void*) datosApp
    };


    esp_timer_create(&timer_reset_args, &tiempo);
    esp_timer_start_once(tiempo, 3000000);
    if (respuesta != NULL) {
    	codigoRespuesta(respuesta, RESP_OK);
    }

    set_status_application(datosApp, EVENT_RESTART);



    return ESP_OK;
}

esp_err_t   ejecutar_factory_reset(DATOS_APLICACION *datosApp, cJSON *respuesta) {

    //Habria que eliminar la configuracion wifi y las etiquetas nvs
	reinicio_fabrica(datosApp);
    ejecutar_reset(datosApp, respuesta);
    return ESP_OK;
}

bool is_esp8266() {

#ifdef CONFIG_IDF_TARGET_ESP8266
	return true;
#else
	return false;
#endif


}



esp_err_t   upgrade_ota(cJSON *peticion, struct DATOS_APLICACION *datosApp, cJSON *respuesta) {


    cJSON *nodo;


   ESP_LOGI(TAG, ""TRAZAR"UPGRADE OTA...", INFOTRAZA);
   nodo = cJSON_GetObjectItem(peticion, UPGRADE_FIRMWARE);
   if(nodo == NULL) {
       codigoRespuesta(respuesta, RESP_NOK);
       return ESP_FAIL;
   }

   extraer_dato_string(nodo, OTA_SERVER, datosApp->datosGenerales->ota.server);
   extraer_dato_string(nodo, OTA_URL, datosApp->datosGenerales->ota.url);
   extraer_dato_string(nodo, OTA_FILE, datosApp->datosGenerales->ota.file);
   extraer_dato_string(nodo, OTA_SW_VERSION, datosApp->datosGenerales->ota.newVersion);
   extraer_dato_int(nodo, OTA_PORT, &datosApp->datosGenerales->ota.puerto);
   ESP_LOGI(TAG, ""TRAZAR"PUERTO: %d", INFOTRAZA, datosApp->datosGenerales->ota.puerto);
   salvar_configuracion_general(datosApp);


   if (datosApp->datosGenerales->estadoApp != UPGRADING) {
	   codigoRespuesta(respuesta, RESP_OK);
	   if (is_esp8266()) {
		   set_upgrade_data(datosApp);
		   ejecutar_reset(datosApp, NULL);
	   } else {
		   tarea_upgrade_firmware(datosApp);
	   }
   } else {
	   codigoRespuesta(respuesta, RESP_NOK);
   }

   return ESP_OK;

}


esp_err_t reinicio_fabrica(DATOS_APLICACION *datosApp) {

	wifi_config_t conf_wifi;
	ESP_LOGW(TAG, ""TRAZAR"SE REINICIA LA CLAVE WIFI Y SE BORRAN LAS CONFIGURACIONES", INFOTRAZA);
	memset(&conf_wifi, 0, sizeof(wifi_config_t));
	//restaurar_wifi_fabrica();
	// Restear claves wifi
	esp_wifi_set_config(WIFI_IF_STA, &conf_wifi);
	//resetar nvs
	nvs_erase_all(datosApp->handle);
	set_app_status_device(datosApp, DEVICE_NOT_CONFIGURED);
	salvar_configuracion_general(datosApp);
	set_status_application(datosApp, EVENT_FACTORY);



	return ESP_OK;
}

esp_err_t notificar_evento_ota(DATOS_APLICACION *datosApp, ESTADO_OTA codeOta) {

    cJSON *respuesta = NULL;

    respuesta = cabecera_espontaneo(datosApp, UPGRADE_FIRMWARE_FOTA);
    cJSON_AddNumberToObject(respuesta, CODIGO_OTA, codeOta);
    publicar_mensaje_json(datosApp, respuesta, NULL);
    //notificarAccion(respuesta, datosApp);
    printf("notificarOta-->enviado\n");
    return ESP_OK;

}

esp_err_t notificar_evento_alarma(DATOS_APLICACION *datosApp, int tipo_alarma, char* mnemonico_alarma) {

	cJSON *respuesta;

	respuesta = cabecera_espontaneo(datosApp, ALARM_REPORT);
	if (respuesta == NULL) {
		return ESP_FAIL;
	}
	cJSON_AddStringToObject(respuesta, MNEMONIC_REPORT, report_2_mnemonic(ALARM_REPORT));
	cJSON_AddNumberToObject(respuesta, mnemonico_alarma, datosApp->alarmas[tipo_alarma].estado_alarma);
	cJSON_AddNumberToObject(respuesta, FECHA_ALARMA, datosApp->alarmas[tipo_alarma].fecha_alarma);
	if (datosApp->alarmas[ALARM_MQTT].estado_alarma == ALARM_OFF) {
		publicar_mensaje_json(datosApp, respuesta, NULL);
	} else {
		ESP_LOGW(TAG, ""TRAZAR"No se publica nada porque no esta activa la conexion MQTT", INFOTRAZA);
	}

	return ESP_OK;

}

esp_err_t    visualizar_alarmas_activas(DATOS_APLICACION *datosApp, cJSON *respuesta) {


	cJSON_AddNumberToObject(respuesta, MNEMONIC_ALARM_WIFI, datosApp->alarmas[0].estado_alarma);
	cJSON_AddNumberToObject(respuesta, MNEMONIC_ALARM_MQTT, datosApp->alarmas[1].estado_alarma);
	cJSON_AddNumberToObject(respuesta, MNEMONIC_ALARM_NTP, datosApp->alarmas[2].estado_alarma);
	cJSON_AddNumberToObject(respuesta, MNEMONIC_ALARM_NVS, datosApp->alarmas[3].estado_alarma);
	codigoRespuesta(respuesta, RESP_OK);

	return ESP_OK;
}

esp_err_t   modificar_configuracion_app(cJSON *peticion,struct DATOS_APLICACION *datosApp, cJSON *respuesta) {

	return appuser_modify_local_configuration_application(peticion, datosApp, respuesta);
}




esp_err_t create_header_report(DATOS_APLICACION *datosApp, cJSON *message, SPONTANEOUS_TYPE spontaneous_type) {

	ESP_LOGI(TAG, ""TRAZAR"create_header_report", INFOTRAZA);


    message = cabeceraGeneral(datosApp);
    if (message == NULL) {
    	ESP_LOGE(TAG, ""TRAZAR"CABECERA NULA", INFOTRAZA);
    	return ESP_FAIL;
    } else {
    	cJSON_AddNumberToObject(message, TIPO_REPORT, spontaneous_type);
    }

    return ESP_OK;

}




esp_err_t send_spontaneous_report(DATOS_APLICACION *datosApp, enum SPONTANEOUS_TYPE tipoInforme) {


    cJSON *respuesta = NULL;
    char valor[20];

    ESP_LOGI(TAG, ""TRAZAR"send_spontaneous_report", INFOTRAZA);
    respuesta = cabecera_espontaneo(datosApp, tipoInforme);
    if (respuesta == NULL) {
    	ESP_LOGE(TAG, ""TRAZAR"send_spontaneous_report:Cabecera nula", INFOTRAZA);
    	return ESP_FAIL;
    }
    cJSON_AddStringToObject(respuesta, MNEMONIC_REPORT, report_2_mnemonic(tipoInforme));
    switch(tipoInforme) {
        case STARTED:

            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
            if (leer_configuracion(datosApp, FIN_UPGRADE, valor) == ESP_OK) {
            	cJSON *upgrade;
            	int dato;
            	upgrade = cJSON_Parse(valor);
            	extraer_dato_int(upgrade, FIN_UPGRADE, &dato);
            	ESP_LOGI(TAG, ""TRAZAR" ESCRIBIMOS EL FIN DE UPGRADE", INFOTRAZA);
            	cJSON_AddNumberToObject(respuesta, FIN_UPGRADE, dato);
            	delete_key_upgrade(&datosApp->handle, FIN_UPGRADE);
            }
                escribir_programa_actual(datosApp, respuesta);
                codigoRespuesta(respuesta,RESP_OK);
                break;
        case OP_LOCAL_RELAY:
        case START_SCHEDULE:
        case END_SCHEDULE:
        case CHANGE_TEMPERATURE:
        case CAMBIO_UMBRAL_TEMPERATURA:
        	//cJSON_AddStringToObject(respuesta, MNEMONIC_REPORT, report_2_mnemonic(tipoInforme));
            cJSON_AddNumberToObject(respuesta, PROGRAMMER_STATE, datosApp->datosGenerales->estadoProgramacion);
            cJSON_AddNumberToObject(respuesta, DEVICE_STATE, datosApp->datosGenerales->estadoApp);
            escribir_programa_actual(datosApp, respuesta);
            codigoRespuesta(respuesta,RESP_OK);
            break;
        case START_UPGRADE_OTA:
        	//cJSON_AddStringToObject(respuesta, MNEMONIC_REPORT, report_2_mnemonic(tipoInforme));
        	codigoRespuesta(respuesta,RESP_OK);
        	break;
        case END_UPGRADE_OTA:
        	codigoRespuesta(respuesta,RESP_OK);
        	break;
        case ERROR_UPGRADE_OTA:
        	codigoRespuesta(respuesta,RESP_NOK);
        	break;

        case DEVICE_RECOVERED:
        	codigoRespuesta(respuesta,RESP_OK);
        	break;


        default:
        	cJSON_AddStringToObject(respuesta, MNEMONIC_REPORT, report_2_mnemonic(tipoInforme));
            codigoRespuesta(respuesta, RESP_NOK);
            break;
    }

    appuser_send_spontaneous_report(datosApp, tipoInforme, respuesta);

	if (respuesta != NULL) {
		publicar_mensaje_json(datosApp, respuesta, NULL);
		ESP_LOGI(TAG, ""TRAZAR" PUBLICADO DESDE API_JSON", INFOTRAZA);
	}


	return ESP_OK;

}

esp_err_t send_start_upgrade_event(DATOS_APLICACION *datosApp, cJSON *message) {


	if (create_header_report(datosApp, message, START_UPGRADE_OTA) == ESP_FAIL) {
		ESP_LOGE(TAG, ""TRAZAR"ERROR AL ENVIAR EL EVENTO SEND_START_OTA", INFOTRAZA);
		return ESP_FAIL;
	}
	cJSON_AddStringToObject(message, MNEMONIC_REPORT, report_2_mnemonic(START_UPGRADE_OTA));
	codigoRespuesta(message, RESP_OK);




	return ESP_OK;

}



