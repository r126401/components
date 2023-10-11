/* Common functions for protocol examples, to establish Wi-Fi or Ethernet connection.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#include "conexiones.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "configuracion.h"
#include "alarmas.h"
#include "interfaz_usuario.h"
#include "smartconfig_ack.h"
#include "esp_smartconfig.h"



#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "smartconfig_ack.h"
#include "esp_err.h"
#include "esp_smartconfig.h"
#include "esp_netif.h"
#include "alarmas.h"
#include "conexiones_mqtt.h"



static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;


static EventGroupHandle_t grupo_eventos;
 ip4_addr_t s_ip_addr;
static const char *TAG = "CONEXIONES";
extern DATOS_APLICACION datosApp;

/*
void extraer_datos_mqtt(void * event_data, wifi_config_t *wifi_config) {

	char* ptr;
	char broker[100] = "mqtt://";
	char pass[65] = {0};
	char *texto;

	texto = (char*) calloc(256, sizeof(char));
	memcpy(texto, event_data+32, 256);
	texto[253] = 33;
	texto[255]=0;
	ESP_LOGI(TAG, ""TRAZAR" datos:%s, longitud: %d", INFOTRAZA, texto, strlen(texto));

	smartconfig_event_got_ssid_pswd_t* evt = (smartconfig_event_got_ssid_pswd_t*)event_data;


	memcpy(pass, (char*) evt->password, sizeof(wifi_config->sta.password) );
    ptr = strtok((char*) evt->password, ":");
    strcpy(pass, ptr);
    if ((ptr = strtok(NULL, ":")) != NULL) {
        strcpy(datosApp.datosGenerales->parametrosMqtt.broker, ptr);
        strcat(broker, datosApp.datosGenerales->parametrosMqtt.broker);
        strcpy(datosApp.datosGenerales->parametrosMqtt.broker, broker);
        printf ("broker:%s\n", datosApp.datosGenerales->parametrosMqtt.broker);
    }

    if ((ptr = strtok(NULL, ":")) != NULL) {
        datosApp.datosGenerales->parametrosMqtt.port = atoi(ptr);
        ESP_LOGI(TAG, ""TRAZAR" PUERTO: %d\n",INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.port);
    }

    if ((ptr = strtok(NULL, ":")) != NULL) {
        strcpy(datosApp.datosGenerales->parametrosMqtt.user, ptr);
        ESP_LOGI(TAG, ""TRAZAR"user: %s\n", INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.user);
    }

    if ((ptr = strtok(NULL, ":")) != NULL) {
        strcpy(datosApp.datosGenerales->parametrosMqtt.password, ptr);
        ESP_LOGI(TAG, ""TRAZAR"password: %s\n",INFOTRAZA, datosApp.datosGenerales->parametrosMqtt.password);
    }

    memset(wifi_config->sta.password, 0, 64);
    memcpy((char*) wifi_config->sta.password, pass,sizeof(wifi_config->sta.password) );



}
*/
void extraer_datos_smartconfig(void * event_data, wifi_config_t *wifi_config) {



	 smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
	 bzero(wifi_config, sizeof(wifi_config_t));
	 memcpy(wifi_config->sta.password, evt->password, sizeof(wifi_config->sta.password));
	 memcpy(wifi_config->sta.ssid, evt->ssid, sizeof(wifi_config->sta.ssid));


}




void volcar_datos_conexion(void * event_data){

    smartconfig_event_got_ssid_pswd_t* evt = (smartconfig_event_got_ssid_pswd_t*)event_data;
    wifi_config_t wifi_config;


    bzero(&wifi_config, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));

    ESP_LOGI(TAG, ""TRAZAR" vamos a extraer los datos de conexion", INFOTRAZA);
    //extraer_datos_mqtt(event_data, &wifi_config);
    extraer_datos_smartconfig(event_data, &wifi_config);

    ESP_LOGI(TAG, ""TRAZAR" SSID: %s, PASS:%s", INFOTRAZA,
    		wifi_config.sta.ssid, wifi_config.sta.password);


    wifi_config.sta.bssid_set = evt->bssid_set;

    if (wifi_config.sta.bssid_set == true) {
        memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
    }
    appuser_notify_connecting_wifi(&datosApp);
    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_connect());



}





static void manejador_eventos_smart(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data) {


    switch (event_id) {
        case SC_EVENT_SCAN_DONE:
        	ESP_LOGI(TAG, ""TRAZAR"Escaneo realizado", INFOTRAZA);
            break;
        case SC_EVENT_FOUND_CHANNEL:
        	ESP_LOGI(TAG, ""TRAZAR"Encontrado canal...", INFOTRAZA);
            break;
        case SC_EVENT_GOT_SSID_PSWD:
        	ESP_LOGI(TAG, ""TRAZAR"Obteniendo datos de conexion...", INFOTRAZA);
        	volcar_datos_conexion(event_data);
            break;
        case SC_EVENT_SEND_ACK_DONE:
            ESP_LOGW(TAG, "SC_EVENT_SEND_ACK_DONE");
            xEventGroupSetBits(grupo_eventos, ESPTOUCH_DONE_BIT);
            break;
        default:
            break;
    }

}










const char * get_my_id(void)
{
   // Use MAC address for Station as unique ID
   static char my_id[13];
   static bool my_id_done = false;
   esp_err_t error;
   int8_t i;
   uint8_t x;
   if (my_id_done)
       return my_id;
   //if (!wifi_get_macaddr(STATION_IF, my_id))

   if ((error = esp_wifi_get_mac(WIFI_IF_STA, (uint8_t*) my_id)) == ESP_OK) {
   	ESP_LOGI(TAG, ""TRAZAR"Mac extraida correctamente", INFOTRAZA);
   } else {
   	ESP_LOGE(TAG, ""TRAZAR"ERROR AL OBTENER LA MAC. ERROR : %d", INFOTRAZA, error);
   	return NULL;
   }

   //if ((esp_wifi_get_mac(WIFI_IF_STA, (uint8_t*) my_id)) != ESP_OK)
   //    return NULL;
   for (i = 5; i >= 0; --i)
   {
       x = my_id[i] & 0x0F;
       if (x > 9) x += 7;
       my_id[i * 2 + 1] = x + '0';
       x = my_id[i] >> 4;
       if (x > 9) x += 7;
       my_id[i * 2] = x + '0';
   }
   my_id[12] = '\0';
   my_id_done = true;
   return my_id;
}





static void on_wifi_disconnect(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{

    //system_event_sta_disconnected_t *event = (system_event_sta_disconnected_t *)event_data;

    ESP_LOGW(TAG, ""TRAZAR"Wi-Fi desconectado, se intenta la reconexion...", INFOTRAZA);
    /*
    if (event->reason == WIFI_REASON_BASIC_RATE_NOT_SUPPORT) {

        esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    }
*/
    if (datosApp.datosGenerales->estadoApp != UPGRADE_EN_PROGRESO) {
    	registrar_alarma(&datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
    	ESP_ERROR_CHECK(esp_wifi_connect());
    	xEventGroupClearBits(grupo_eventos, CONNECTED_BIT);
    }

    registrar_alarma(&datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_ON, false);
    appuser_notify_error_wifi_connection(&datosApp);


}

static void on_got_ip(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
	ESP_LOGW(TAG, ""TRAZAR"Wi-Fi conectado", INFOTRAZA);
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    memcpy(&s_ip_addr, &event->ip_info.ip, sizeof(s_ip_addr));
    xEventGroupSetBits(grupo_eventos, CONNECTED_BIT);
    registrar_alarma(&datosApp, NOTIFICACION_ALARMA_WIFI, ALARMA_WIFI, ALARMA_OFF, false);
    appuser_notify_wifi_connected_ok(&datosApp);
    sync_app_by_ntp(&datosApp);
}


inline static void conectar_wifi() {

	ESP_LOGW(TAG, ""TRAZAR"Wi-Fi vamos a conectar", INFOTRAZA);
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGW(TAG, ""TRAZAR"Wi-Fi vamos 2.A a conectar", INFOTRAZA);
	esp_wifi_connect();
	ESP_LOGW(TAG, ""TRAZAR"Wi-Fi vamos 2 a conectar", INFOTRAZA);
	xEventGroupWaitBits(grupo_eventos, CONNECTED_BIT, true, true, portMAX_DELAY);
	ESP_LOGW(TAG, ""TRAZAR"Wi-Fi fin de vamos a conectar", INFOTRAZA);

}


static void on_wifi_scan_done(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {


	wifi_ap_record_t ap_info[CONFIG_DEFAULT_SCAN_LIST_SIZE];
	uint16_t number = CONFIG_DEFAULT_SCAN_LIST_SIZE;
	uint16_t ap_count = 0;
	memset(ap_info, 0, sizeof(ap_info));


	ESP_LOGI(TAG, ""TRAZAR"RECIBIDO SCANDONE", INFOTRAZA);


    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_LOGI(TAG, ""TRAZAR"RECIBIDO SCANDONE 2", INFOTRAZA);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, ""TRAZAR"RECIBIDO SCANDONE 3", INFOTRAZA);
    if (ap_count > number) {
    	ap_count = number;
    }

    ESP_LOGI(TAG, ""TRAZAR"RECIBIDO SCANDONE 4", INFOTRAZA);
	esp_wifi_scan_stop();
	appuser_notify_scan_done(&datosApp, ap_info, &ap_count);



}




inline static void inicializar_wifi() {

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S3)
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

#endif

	ESP_LOGI(TAG, ""TRAZAR" INICIALIZAR_WIFI", INFOTRAZA);
    //tcpip_adapter_init();
	if (datosApp.datosGenerales->estadoApp != FACTORY) {
		appuser_notify_connecting_wifi(&datosApp);
	}

    grupo_eventos = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S3)
    esp_netif_create_default_wifi_sta ();
#endif


    //ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S3)
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, &instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &manejador_eventos_smart, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_SCAN_DONE, &on_wifi_scan_done, NULL));
#else
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &manejador_eventos_smart, NULL));
#endif
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));


    //ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    //ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, ""TRAZAR" WIFI INICIALIZADO", INFOTRAZA);
}

esp_err_t restaurar_wifi_fabrica() {

	wifi_config_t wifi_config;
	strcpy((char*) &wifi_config.sta.ssid, "none");
	strcpy((char*) &wifi_config.sta.password, "none");
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));

    return ESP_OK;

}

void tarea_smartconfig(void* parm) {
    EventBits_t uxBits;
    DATOS_APLICACION *datosApp;
    datosApp = (DATOS_APLICACION*) parm;
    appuser_notify_smartconfig(datosApp);
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

    while (1) {
        uxBits = xEventGroupWaitBits(grupo_eventos, (CONNECTED_BIT | ESPTOUCH_DONE_BIT), true, false, portMAX_DELAY);

        if (uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG, "CONECTADO A LA ESTACION WIFI DESPUES DE SMARTCONFIG");
        }

        if (uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "SMARTCONFIG TERMINADO");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}


esp_err_t conectar_dispositivo_wifi() {

	wifi_config_t conf_wifi;
	int i;
	inicializar_wifi();
    esp_wifi_get_config(WIFI_IF_STA, &conf_wifi);
    conf_wifi.sta.pmf_cfg.capable = true;
    conf_wifi.sta.pmf_cfg.required = false;
    ESP_LOGW(TAG, ""TRAZAR"(1)", INFOTRAZA);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_LOGW(TAG, ""TRAZAR"(2)", INFOTRAZA);
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &conf_wifi) );
    ESP_LOGW(TAG, ""TRAZAR"(3)", INFOTRAZA);

    ESP_LOGW(TAG, ""TRAZAR"(4)", INFOTRAZA);

	for (i=0;i<32;i++) {
		if (conf_wifi.sta.ssid[i] != 0) {
			ESP_LOGW(TAG, ""TRAZAR" WIFI CONFIGURADA %s, %s", INFOTRAZA, (char*) conf_wifi.sta.ssid, (char*) conf_wifi.sta.password);
			conectar_wifi();
			return ESP_OK;
		}
	}

	ESP_LOGW(TAG, ""TRAZAR" WIFI NO CONFIGURADA", INFOTRAZA);

	xTaskCreate(tarea_smartconfig, "tarea_smart", 4096, (void*)&datosApp, tskIDLE_PRIORITY + 0, NULL);
	//xEventGroupWaitBits(grupo_eventos, CONNECTED_BIT, true, true, portMAX_DELAY);
	conectar_wifi();
	return ESP_FAIL;

}


void sync_app_by_ntp(DATOS_APLICACION *datosApp) {

	esp_err_t error;

	inicializar_parametros_ntp(&datosApp->datosGenerales->clock);
	appuser_get_date_sntp(datosApp);
	ESP_LOGW(TAG, ""TRAZAR"(1)", INFOTRAZA);
	error = obtener_fecha_hora(&datosApp->datosGenerales->clock);
	ESP_LOGW(TAG, ""TRAZAR"(2)", INFOTRAZA);

	if (error != ESP_OK) {
		ESP_LOGW(TAG, ""TRAZAR"NO SE HA PODIDO OBTENER LA HORA DEL SERVIDOR NTP. NO HABRA PROGRAMACION. error: %d", INFOTRAZA, error);
		datosApp->datosGenerales->estadoProgramacion = INVALID_PROG;
		appuser_error_get_date_sntp(datosApp);
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_NTP, ALARMA_NTP, ALARMA_ON, false);

	} else {
		ESP_LOGI(TAG, ""TRAZAR" VAMOS A REGISTRAR ALARMA", INFOTRAZA);
		registrar_alarma(datosApp, NOTIFICACION_ALARMA_NTP, ALARMA_NTP, ALARMA_OFF, false);
		appuser_sntp_ok(datosApp);
		actualizar_hora(&datosApp->datosGenerales->clock);
		//ESP_LOGI(TAG, ""TRAZAR"Hora inicializada:%s", INFOTRAZA, pintar_fecha(datosApp->datosGenerales->clock.date);
		ESP_LOGI(TAG, ""TRAZAR"Hora inicializada:%s", INFOTRAZA,pintar_fecha(datosApp->datosGenerales->clock.date));
	    datosApp->datosGenerales->estadoProgramacion = VALID_PROG;


	}
}

esp_err_t get_scan_station_list() {


	    wifi_scan_config_t config_scan = {
	    		.ssid = 0,
				.bssid = 0,
	    		.channel = 0,
				.scan_type = WIFI_SCAN_TYPE_ACTIVE,
				.scan_time.active.min = 120,
				.scan_time.active.max = 150
	    };

	    esp_wifi_disconnect();
	    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	    ESP_ERROR_CHECK(esp_wifi_start());
	    esp_wifi_scan_start(&config_scan, false);


	return ESP_OK;

}


void wifi_task(void *arg) {

	DATOS_APLICACION *datosApp = (DATOS_APLICACION*) arg;
	while (1) {
		conectar_dispositivo_wifi();
		sync_app_by_ntp(datosApp);
		crear_tarea_mqtt(datosApp);
		iniciar_gestion_programacion(datosApp);
	}

	vTaskDelete(NULL);


}
/*
static bool is_wifi_configured(wifi_config_t *conf_wifi) {

	int i;
    esp_wifi_get_config(WIFI_IF_STA, conf_wifi);
    conf_wifi->sta.pmf_cfg.capable = true;
    conf_wifi->sta.pmf_cfg.required = false;

	for (i=0;i<32;i++) {
		if (conf_wifi->sta.ssid[i] != 0) {
			ESP_LOGW(TAG, ""TRAZAR" WIFI CONFIGURADA %s, %s", INFOTRAZA, (char*) conf_wifi->sta.ssid, (char*) conf_wifi->sta.password);
			return conf_wifi;
		}
	}

	return NULL;

}
*/
