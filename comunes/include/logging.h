/*
 * logging.h
 *
 *  Created on: Jun 19, 2023
 *      Author: t126401
 */

#ifndef COMPONENTS_COMUNES_INCLUDE_LOGGING_H_
#define COMPONENTS_COMUNES_INCLUDE_LOGGING_H_

#include "ntp.h"
#include "stdint.h"
#include "nvs_flash.h"
#include "esp_app_format.h"
#ifndef CONFIG_IDF_TARGET_ESP8266

#else
#include "esp_app_desc.h"
#endif

#include "common_data.h"


#define TRAZAR "%s:%s:%d-->"
#define INFOTRAZA pintar_fecha(), __func__, __LINE__

char* pintar_fecha();
char* status2mnemonic(ESTADO_APP status);

#endif /* COMPONENTS_COMUNES_INCLUDE_LOGGING_H_ */
