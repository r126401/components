/*
 * espota.h
 *
 *  Created on: 16 feb. 2021
 *      Author: t126401
 */

#ifndef COMPONENTS_ESPOTA_INCLUDE_ESPOTA_H_
#define COMPONENTS_ESPOTA_INCLUDE_ESPOTA_H_
//#include "configuracion_usuario.h"

#ifndef CONFIG_IDF_TARGET_ESP8266

#include "esp_app_desc.h"
#else
#include "esp_app_format.h"
#endif



typedef struct OTADATA {
    char server[100];
    char url[150];
    int puerto;
    char file[50];
    const esp_app_desc_t* swVersion;
    char newVersion[32];


}OTADATA;


typedef enum ESTADO_OTA {

    OTA_ERROR,
            OTA_DESCARGA_FIRMWARE,
            OTA_BORRANDO_SECTORES,
            OTA_COPIANDO_SECTORES,
            OTA_UPGRADE_FINALIZADO,
            OTA_FALLO_CONEXION,
            OTA_DATOS_CORRUPTOS,
            OTA_PAQUETES_ERRONEOS,
            OTA_CRC_ERRONEO,
            OTA_ERROR_PARTICION,
            OTA_CABECERA_MAL_FORMADA,
            OTA_ERROR_MEMORIA_DISPONIBLE,
            OTA_ERROR_OTA_BEGIN,
            OTA_ERROR_AL_ESCRIBIR_EN_MEMORIA
} ESTADO_OTA;





#endif /* COMPONENTS_ESPOTA_INCLUDE_ESPOTA_H_ */
