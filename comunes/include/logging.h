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
#include "esp_app_desc.h"


#define TRAZAR "%s:%s:%d-->"
#define INFOTRAZA pintar_fecha(), __func__, __LINE__

char* pintar_fecha();

#endif /* COMPONENTS_COMUNES_INCLUDE_LOGGING_H_ */
