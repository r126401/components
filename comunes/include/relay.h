/*
 * relay.h
 *
 *  Created on: Jun 19, 2023
 *      Author: t126401
 */

#ifndef COMPONENTS_COMUNES_INCLUDE_RELAY_H_
#define COMPONENTS_COMUNES_INCLUDE_RELAY_H_


typedef enum ESTADO_RELE {
    INDETERMINADO = -1,
    OFF = 0,
    ON = 1
}ESTADO_RELE;

typedef enum TIPO_ACTUACION_RELE {
    MANUAL,
    REMOTA,
    TEMPORIZADA,
    ARRANQUE_RELE,
    EXPIRADA
}TIPO_ACTUACION_RELE;


#endif /* COMPONENTS_COMUNES_INCLUDE_RELAY_H_ */
