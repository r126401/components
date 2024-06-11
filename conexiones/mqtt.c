/*
 * mqtt.c
 *
 *  Created on: 22 sept. 2020
 *      Author: t126401
 */


#include "esp_err.h"
#include "esp_log.h"
#include "conexiones_mqtt.h"
#include "mqtt_client.h"
#include "api_json.h"
#include "events_device.h"
#include "user_interface.h"
#include "dialogos_json.h"
#include "esp_tls.h"
#include "conexiones_mqtt.h"
#include "event_groups.h"
#include "applib.h"



static const char *TAG = "MQTT";



extern const uint8_t mqtt_jajica_pem_start[]   asm("_binary_mqtt_cert_crt_start");
extern const uint8_t mqtt_jajica_pem_end[]   asm("_binary_mqtt_cert_crt_end");

extern DATOS_APLICACION datosApp;

xQueueHandle cola_mqtt = NULL;
esp_mqtt_client_handle_t client;
extern TaskHandle_t handle;


esp_err_t subscribe_topic(DATOS_APLICACION *datosApp, char* topic) {


	int id;





	id = esp_mqtt_client_subscribe(client, topic, datosApp->datosGenerales->parametrosMqtt.qos);

	if (id > 0) {
		ESP_LOGI(TAG, ""TRAZAR"subscribe_topic: suscrito. id = %d", INFOTRAZA, id);
		return ESP_OK;
	} else {
		ESP_LOGI(TAG, ""TRAZAR"subscribe_topic: no suscrito. id = %d", INFOTRAZA, id);
		return ESP_FAIL;
	}

}



esp_err_t unsubscribe_topic(DATOS_APLICACION *datosApp, int index_topic) {

	int id = esp_mqtt_client_unsubscribe(client, datosApp->datosGenerales->parametrosMqtt.topics[index_topic].subscribe);

	if (id > 0) {
		ESP_LOGI(TAG, ""TRAZAR"unsubscribe_topic: no suscrito. id = %d", INFOTRAZA, id);
		datosApp->datosGenerales->parametrosMqtt.topics[index_topic].status = false;
		memset(datosApp->termostato.sensor_remoto, 0,sizeof(datosApp->datosGenerales->parametrosMqtt.topics[index_topic].publish));
		memset(datosApp->termostato.sensor_remoto, 0,sizeof(datosApp->datosGenerales->parametrosMqtt.topics[index_topic].subscribe));
		return ESP_OK;
	} else {
		ESP_LOGE(TAG, ""TRAZAR"unsubscribe_topic: no estaba suscrito. id = %d", INFOTRAZA, id);
		return ESP_FAIL;
	}





}



void mqtt_task(void *arg) {

	//DATOS_APLICACION *datosApp = (DATOS_APLICACION*) arg;
	COLA_MQTT cola;


	ESP_LOGI(TAG, ""TRAZAR"COMIENZO MQTT_TASK", INFOTRAZA);
	//establecer_conexion_mqtt(&datosApp);

	cola_mqtt = xQueueCreate(10, sizeof(COLA_MQTT));

	for(;;) {
		 ESP_LOGI(TAG, ""TRAZAR"ESPERANDO MENSAJE...Memoria libre: "CONFIG_UINT32_FORMAT"\n", INFOTRAZA, esp_get_free_heap_size());
		if (xQueueReceive(cola_mqtt, &cola, portMAX_DELAY) == pdTRUE) {
			ESP_LOGE(TAG, ""TRAZAR"se va a procesar la peticion", INFOTRAZA);
			//publicar_mensaje(&datosApp, &cola);

		} else {
			ESP_LOGE(TAG, ""TRAZAR"NO SE HA PODIDO PROCESAR LA PETICION", INFOTRAZA);
		}

	}

	vTaskDelete(NULL);


}



#ifndef CONFIG_IDF_TARGET_ESP8266


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    static bool arranque = false;
    ESP_LOGI(TAG, "Event dispatched from event loop base=%s, event_id="CONFIG_UINT32_FORMAT"", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    datosApp.handle_mqtt = event;
    int msg_id = 0;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_CONNECTED: CONECTADO AL BROKER", INFOTRAZA);
        msg_id = esp_mqtt_client_subscribe(client, datosApp.datosGenerales->parametrosMqtt.topics[0].subscribe,datosApp.datosGenerales->parametrosMqtt.qos);
        ESP_LOGI(TAG, ""TRAZAR"ACCION PARA SUBSCRIBIR AL TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);
        if (datosApp.alarmas[ALARM_MQTT].estado_alarma == ALARM_ON) {
        	//registrar_alarma(&datosApp, NOTIFICACION_ALARMA_MQTT, ALARMA_MQTT, ALARMA_OFF, true);
        	send_event(__func__,EVENT_MQTT_OK);
        }

        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, ""TRAZAR"MQTT_EVENT_DISCONNECTED: Desconectado del broker :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.broker, msg_id);
        //registrar_alarma(&datosApp, NOTIFICACION_ALARMA_MQTT, ALARMA_MQTT, ALARMA_ON, false);
        send_event(__func__,EVENT_ERROR_MQTT);

        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_SUBSCRIBED: SUBSCRITOS CON EXITO AL TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);
        if (arranque == false ){
        	send_event(__func__,EVENT_MQTT_OK);
        	//appuser_notify_application_started(&datosApp);
        	//datosApp.datosGenerales->estadoApp = ESPERA_FIN_ARRANQUE;
        	arranque = true;
        }
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGW(TAG, ""TRAZAR"MQTT_EVENT_UNSUBSCRIBED: YA NO ESTAS SUBSCRITO AL TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_PUBLISHED: CONFIRMACION DE EVENTO PUBLICADO. TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_DATA: RECIBIDO MENSAJE", INFOTRAZA);

        char topic[55] = {0};
        strncpy(topic, event->topic, event->topic_len);

        message_application_received(&datosApp, topic);
/*
        if (strcmp(datosApp.datosGenerales->parametrosMqtt.subscribe, topic) == 0) {
        	message_application_received(&datosApp, topic);
        } else {
        	appuser_reding_remote_temperature(&datosApp);
        }
*/
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)",  event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        } else {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        ESP_LOGE(TAG, ""TRAZAR"MQTT_EVENT_ERROR", INFOTRAZA);
        //registrar_alarma(&datosApp, NOTIFICACION_ALARMA_MQTT, ALARMA_MQTT, ALARMA_ON, false);
        send_event(__func__,EVENT_ERROR_MQTT);

        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}







esp_err_t init_device_mqtt(DATOS_APLICACION *datosApp) {
	esp_err_t error;

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address.uri = datosApp->datosGenerales->parametrosMqtt.broker,
			.address.port = datosApp->datosGenerales->parametrosMqtt.port,
            .verification.certificate = (const char *)mqtt_jajica_pem_start
        },
    };

    client = esp_mqtt_client_init(&mqtt_cfg);


    if (!client) {
    	ESP_LOGE(TAG, ""TRAZAR"El cliente no esta inicializado", INFOTRAZA);
    }

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    if (datosApp->datosGenerales->parametrosMqtt.tls == true) {
    	ESP_LOGD(TAG, ""TRAZAR"Añadimos el certificado %s", INFOTRAZA, mqtt_jajica_pem_start);

    	esp_tls_init();
    	error = esp_tls_init_global_ca_store ();
    	if (error != ESP_OK) {
    		ESP_LOGE(TAG, ""TRAZAR"ERROR al inicializar el almacen CA %d", INFOTRAZA, sizeof ((const char*)mqtt_jajica_pem_start));
    	}


    	ESP_LOGE(TAG, ""TRAZAR"CA inicializada %d", INFOTRAZA, strlen ((const char*)mqtt_jajica_pem_start));

    	error = esp_tls_set_global_ca_store (( const  unsigned  char *) mqtt_jajica_pem_start, mqtt_jajica_pem_end - mqtt_jajica_pem_start );
    	if (error != ESP_OK) {
    		ESP_LOGE(TAG, ""TRAZAR"ERROR al crear el almacen CA", INFOTRAZA);
    	}
    	//mqtt_cfg.cert_pem = (const char *) mqtt_jajica_pem_start;
    	//mqtt_cfg.use_global_ca_store = true;
    	//mqtt_cfg.skip_cert_common_name_check = true;

    } else {

    	ESP_LOGE(TAG, ""TRAZAR"No se ha añadido el certificado para la conexion mqtt", INFOTRAZA);
    	datosApp->datosGenerales->parametrosMqtt.cert = (char*) mqtt_jajica_pem_start;
    	//strcpy(datosApp->datosGenerales->parametrosMqtt.cert, (const char*) mqtt_jajica_pem_start);
    }
    esp_mqtt_client_start(client);




    ESP_LOGI(TAG, ""TRAZAR"Nos conectamos al broker %s", INFOTRAZA, mqtt_cfg.broker.address.uri);
    send_event(__func__, EVENT_CONNECT_MQTT);
    //appuser_notify_connecting_broker_mqtt(datosApp);
    //client = esp_mqtt_client_init(&mqtt_cfg);
    //esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    //esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    //error = esp_mqtt_client_start(client);


    return ESP_OK;
}

#else

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id = -1;
    // your_context_t *context = event->context;
    datosApp.handle_mqtt = event;
    static bool arranque = false;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_CONNECTED: CONECTADO AL BROKER", INFOTRAZA);
            msg_id = esp_mqtt_client_subscribe(client, datosApp.datosGenerales->parametrosMqtt.subscribe,datosApp.datosGenerales->parametrosMqtt.qos);
            ESP_LOGI(TAG, ""TRAZAR"ACCION PARA SUBSCRIBIR AL TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);
            if (datosApp.alarmas[ALARM_MQTT].estado_alarma == ALARM_ON) {
            	//registrar_alarma(&datosApp, NOTIFICACION_ALARMA_MQTT, ALARMA_MQTT, ALARMA_OFF, true);
            	send_event(__func__,EVENT_MQTT_OK);

            }

            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, ""TRAZAR"MQTT_EVENT_DISCONNECTED: Desconectado del broker :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.broker, msg_id);
            send_event(__func__,EVENT_ERROR_MQTT);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_SUBSCRIBED: SUBSCRITOS CON EXITO AL TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);
            if (arranque == false ){
            	send_event(__func__,EVENT_MQTT_OK);
            	//appuser_notify_application_started(&datosApp);
            	//datosApp.datosGenerales->estadoApp = ESPERA_FIN_ARRANQUE;
            	arranque = true;
            }
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGW(TAG, ""TRAZAR"MQTT_EVENT_UNSUBSCRIBED: YA NO ESTAS SUBSCRITO AL TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_PUBLISHED: CONFIRMACION DE EVENTO PUBLICADO. TOPIC :%s msg_id=%d", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.subscribe, msg_id);

            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, ""TRAZAR"MQTT_EVENT_DATA: RECIBIDO MENSAJE", INFOTRAZA);

            char topic[55] = {0};
            strncpy(topic, event->topic, event->topic_len);
            message_application_received(&datosApp, topic);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, ""TRAZAR"MQTT_EVENT_ERROR", INFOTRAZA);
            send_event(__func__,EVENT_ERROR_MQTT);
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
        	ESP_LOGE(TAG, ""TRAZAR"MQTT_EVENT_BEFORE_CONNECT. EVENTO NUEVO DESPUES DE CONECTAR", INFOTRAZA);
        	break;
        default:
        	break;
    }
    return ESP_OK;
}




esp_err_t init_device_mqtt(DATOS_APLICACION *datosApp) {
	esp_err_t error;

    esp_mqtt_client_config_t mqtt_cfg = {
		.uri = datosApp->datosGenerales->parametrosMqtt.broker,
		.port = datosApp->datosGenerales->parametrosMqtt.port,
        .event_handle = mqtt_event_handler,
		.username = datosApp->datosGenerales->parametrosMqtt.user,
		.password = datosApp->datosGenerales->parametrosMqtt.password,
		.cert_pem = (const char *) mqtt_jajica_pem_start,
		//.cert_pem = get_certificate(datosApp),
    };

    ESP_LOGI(TAG, ""TRAZAR"Nos conectamos al broker %s", INFOTRAZA, mqtt_cfg.uri);
    //send_event(__func__, EVENT_CONNECT_MQTT);
    client = esp_mqtt_client_init(&mqtt_cfg);
    error = esp_mqtt_client_start(client);


    return error;
}


#endif




esp_err_t publicar_mensaje(DATOS_APLICACION *datosApp, char* topic, char* message) {

	int msg_id = 0;



	msg_id = esp_mqtt_client_publish(client,
			topic,
			message,
			0,
			datosApp->datosGenerales->parametrosMqtt.qos,
			0);
	ESP_LOGI(TAG, ""TRAZAR" MENSAJE %d PUBLICADO.%s \n %s", INFOTRAZA, msg_id, topic, message);
	return ESP_OK;
}

esp_err_t publicar_mensaje_json(DATOS_APLICACION *datosApp, cJSON *mensaje, char *topic) {

	char* texto;
	ESP_LOGW(TAG, ""TRAZAR"publicar_mensaje_json: Nueva peticion", INFOTRAZA);
	if (client == NULL) {
		ESP_LOGE(TAG, ""TRAZAR"NO HAY CONEXION CON EL BROKER Y NO SE PUEDE ENVIAR EL MENSAJE", INFOTRAZA);
		cJSON_Delete(mensaje);
		return ESP_FAIL;
	}


	texto = cJSON_Print(mensaje);
	if (texto != NULL) {
		if (topic == NULL) {
			publicar_mensaje(datosApp, get_app_publish_topic(datosApp, 0), texto);
		}else {
			publicar_mensaje(datosApp, topic, texto);
		}
		free(texto);
	} else {
		ESP_LOGW(TAG, ""TRAZAR"publicar_mensaje_json: No se publica nada porque el json es nulo", INFOTRAZA);
	}
	cJSON_Delete(mensaje);
	return ESP_OK;
}


void crear_tarea_mqtt(DATOS_APLICACION *datosApp) {



    xTaskCreatePinnedToCore(mqtt_task, "mqtt_task", CONFIG_RESOURCE_MQTT_TASK, (void*) datosApp, 4, &handle,2);
    configASSERT(handle);

    if (handle == NULL) {
    	ESP_LOGE(TAG, ""TRAZAR"handle es nulo", INFOTRAZA);

    } else {
    	ESP_LOGE(TAG, ""TRAZAR"handle no es nulo", INFOTRAZA);
    }


}


void eliminar_tarea_mqtt() {

	esp_mqtt_client_destroy(datosApp.handle_mqtt->client);
	vTaskDelete(handle);



}





