/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <errores_proyecto.h>
#include "programmer.h"
//#include "configuracion_usuario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "user_interface.h"
#include "esp_timer.h"
#include "events_device.h"

//static ETSTimer timerDuracion;
static const char *TAG = "programmer.c";
static esp_timer_handle_t temporizador_duracion;

#define TEMPORIZADOR_MAXIMO_EN_SEGUNDOS 428
//#define TEMPORIZADOR_MAXIMO_EN_SEGUNDOS 30



//#define uint8_t uint8_t_t
static esp_timer_handle_t temporizador;


struct TIME_PROGRAM*  insertarElemento(struct TIME_PROGRAM *listProgram, uint8_t *elements) {
    
    TIME_PROGRAM *list;

    //Si es el primer elemento, se crea la lista
    if ((listProgram == NULL) || (*elements == 0)) {
        list = (TIME_PROGRAM*) calloc(1,sizeof(*listProgram));
        (*elements) ++;
       //printf("reservados : %d\n", sizeof(*list));
    } else {
        //Se añade un elemento mas
        (*elements) ++;
        list = (TIME_PROGRAM*) realloc(listProgram, sizeof(*list) * (*elements) );
        //printf("reservados : %d\n", sizeof(*list) * (*elements));
    }
    
    if (list == NULL) {
        //printf("insertarElemento--> Error al crear el elemento\n");
        return NULL;
    } else {
        //printf("insertarElemento--> Elemento creado o anadido\n");

        return list;
                
    }
    
    
}

/**
 * Esta funcion ordena los tiempos time_t en orden decreciente. Siendo el mas reciente el primero de la lista.
 */
int compararDatos(struct TIME_PROGRAM *dat1, struct TIME_PROGRAM *dat2) {
    
    if (dat1->programa < dat2->programa)
        return -1;
    else if(dat1->programa > dat2->programa)
        return 1;
    else return 0;
    
}


TIME_PROGRAM* crearPrograma(TIME_PROGRAM *peticion, TIME_PROGRAM *listaProgramas, uint8_t *nElementos) {
    
    TIME_PROGRAM *list;
    uint8_t elem;
    elem = *nElementos;
    generarIdPrograma(peticion);
    //Si se encuentra duplicado, no se crea el programa
    if (buscarProgramaDuplicado(peticion->idPrograma, listaProgramas, elem) == true) {
        //printf("crearPrograma-->Programa no creado\n");
        return listaProgramas;
    }
    
    //Primero insertamos el elemento en la lista
    list = insertarElemento(listaProgramas, (uint8_t*)nElementos);
    if (list == NULL) return NULL;
    
    
    //Lo actualizamos con la informacion de la peticion.
    memcpy(&list[elem], peticion, sizeof(*list));
    return list;
        
        
    }
    
uint8_t InicializarListaProgramas(TIME_PROGRAM *listaProgramas, int nElementos, struct NTP_CLOCK *clock) {

    int i;
    for(i=0;i<nElementos;i++) {
        if (listaProgramas[i].tipo == SEMANAL) {
            listaProgramas[i].programacion.tm_mday = listaProgramas[i].programacion.tm_mday +7;
        }
    } 
    return 0;
    
    
}
    
bool calcularMascara(int diaActual, int mascara) {
    
    int i;
    int j=1;
    int k;
    //printf("calcularMascara--> dia actual: %d, mascara : %d\n", diaActual, mascara);
    for (i=0;i<diaActual;i++) {
        
        j = j << 1;      
    }
    
    
    k= j & mascara;
    
    if (k == j) {
        //printf("calcularMascara-->programacion activa %d\n", k);
        return true;
    } else {
        //printf("calcularMascara-->programacion inactiva %d\n", k);
        return false;
    }

}




esp_err_t ordenarListaProgramas(TIME_PROGRAM *listaProgramas, int nElementos, struct tm clock) {
    
    int i;
    
    // Añado los atributos necesarios en funcion del tipo de programacion para convertirlo en time_t valido.
    
    for(i=0;i<nElementos;i++) {
        //listaProgramas[i].programacion.tm_year = clock.tm_isdst;
        switch (listaProgramas[i].tipo) {
            case DIARIA:
                listaProgramas[i].programacion.tm_year = clock.tm_year;
                listaProgramas[i].programacion.tm_mon = clock.tm_mon;
                listaProgramas[i].programacion.tm_mday = clock.tm_mday;
                listaProgramas[i].programacion.tm_wday = clock.tm_wday;
                if (calcularMascara(clock.tm_wday, listaProgramas[i].mascara) == true) {
                    listaProgramas[i].activo = 1;
                    //ESP_LOGW(TAG, ""TRAZAR" PROGRAMA CON MASCARA ACTIVA", INFOTRAZA);
                } else {
                    listaProgramas[i].activo = 0;
                    //ESP_LOGW(TAG, ""TRAZAR" PROGRAMA CON MASCARA INACTIVA", INFOTRAZA);
                }
                
                listaProgramas[i].programacion.tm_yday = clock.tm_yday;
                break;
                
            case SEMANAL:
                listaProgramas[i].programacion.tm_year = clock.tm_year;
                
                if (clock.tm_wday > listaProgramas[i].programacion.tm_wday) {
                   // printf("dia de la semana actual > programa, %d, %d \n", (clock.tm_wday - listaProgramas[i].programacion.tm_wday),
                     //       (clock.tm_wday - listaProgramas[i].programacion.tm_wday));
                    listaProgramas[i].programacion.tm_yday = clock.tm_yday - (clock.tm_wday - listaProgramas[i].programacion.tm_wday);
                    listaProgramas[i].programacion.tm_mday = clock.tm_mday - (clock.tm_wday - listaProgramas[i].programacion.tm_wday);
                    listaProgramas[i].programacion.tm_mon = clock.tm_mon;
                    //Si ya ha pasado la programacion, le sumamos 7 dias
                    //listaProgramas[i].programacion.tm_mday = listaProgramas[i].programacion.tm_mday;
                } else if (clock.tm_wday < listaProgramas[i].programacion.tm_wday) {
                  //  printf("dia de la semana menor que el programado. Sumamos %d dias\n", (listaProgramas[i].programacion.tm_wday- clock.tm_wday));
                    listaProgramas[i].programacion.tm_yday = clock.tm_yday + (listaProgramas[i].programacion.tm_wday - clock.tm_wday);
                    listaProgramas[i].programacion.tm_mday = clock.tm_mday + (listaProgramas[i].programacion.tm_wday - clock.tm_wday);
                    listaProgramas[i].programacion.tm_mon = clock.tm_mon;
                    
                } else {
                    
                    listaProgramas[i].programacion.tm_yday = clock.tm_yday;
                    listaProgramas[i].programacion.tm_mday = clock.tm_mday;
                    listaProgramas[i].programacion.tm_mon = clock.tm_mon;

                    
                }
                break;
                
            case FECHADA:
                //printf("ordenarListaProgramas--> Pinto la fechada\n");
                //listaProgramas[i].programacion.tm_yday = 0;
                break;
                
        }
        // Convierto la fecha calculada en un time_t para ordenarlo despues.
        //Adapto la hora a la zona horaria
        //listaProgramas[i].programa = localtime_r(&listaProgramas[i].programacion);
        listaProgramas[i].programacion.tm_isdst = -1;
        listaProgramas[i].programa = mktime(&listaProgramas[i].programacion);

        //ESP_LOGI(TAG, ""TRAZAR"ordenarListaProgramas-->tPrograma : "CONFIG_UINT32_FORMAT", tActual: "CONFIG_UINT32_FORMAT", dif: "CONFIG_UINT32_FORMAT"\n", listaProgramas[i].programa, clock->time,(listaProgramas[i].programa - clock->time) );
        
    }
    // Una vez rellena toda la lista de programacion, se ordena de menor a mayor.
    qsort(listaProgramas, nElementos, sizeof(TIME_PROGRAM),(compfn) compararDatos);
    //despues de ordenar
    //printf("ordenarListaProgramas-->Despues de ordenar...\n");
    
    for(i=0;i<nElementos;i++) {
        ESP_LOGD(TAG, ""TRAZAR"ordenarListaProgramas-->%d/%02d/%02d %d %02d %02d:%02d:%02d %d time_t = "CONFIG_LONG_FORMAT"", INFOTRAZA,
                listaProgramas[i].programacion.tm_year + 1900,
                listaProgramas[i].programacion.tm_mon +1,
                listaProgramas[i].programacion.tm_mday,
                listaProgramas[i].programacion.tm_yday,
                listaProgramas[i].programacion.tm_wday,
                listaProgramas[i].programacion.tm_hour,
                listaProgramas[i].programacion.tm_min,
                listaProgramas[i].programacion.tm_sec,
                listaProgramas[i].programacion.tm_isdst,
				listaProgramas[i].programa);
        
    }
    
    
    
    return ESP_OK;
}

int calcularProgramaActivoActual(TIME_PROGRAM *listaProgramas, int nElementos, int actual) {
    
    int i;

    
    for (i=actual; i > -1; i--) {
        //printf("calcularProgramaActivoActual-->actual: %d\n", i);
        if ((listaProgramas[i].activo == 0) || (listaProgramas[i].estadoPrograma == INACTIVO)) {
            visualizartiempo(listaProgramas[i].programacion);
             actual --;
           
        } else {
            //printf("calcularProgramaActivoActual-->encontrado actual: %d\n", i);
            return i;
        }
    }
    //printf("calcularProgramaActivoActual--> actual no encontrado. Retornado: %d\n", i);
    return i;
    

    
}

bool esActivoElPrograma(TIME_PROGRAM *listaProgramas, int nElementos, int indice) {
    
    if ((listaProgramas->activo == 1) && (listaProgramas->estadoPrograma == ACTIVO)) {
        //printf("esActivoElPrograma--> activo, idPrograma: %s, indice:%d\n", listaProgramas->idPrograma, indice);
        
        return true;
    } else {
        //printf("esActivoElPrograma--> inactivo, idPrograma: %s, indice: %d\n", listaProgramas->idPrograma, indice);
        return false;
    }
    
    
}






void visualizartiempo(struct tm tiempo) {

    ESP_LOGI(TAG, ""TRAZAR"%02d/%02d/%d %02d:%02d:%02d diaSem: %d, dias del ano: %d, ", INFOTRAZA,
    		tiempo.tm_mday, tiempo.tm_mon + 1, (tiempo.tm_year + 1900),
			tiempo.tm_hour, tiempo.tm_min, tiempo.tm_sec, tiempo.tm_wday,
			tiempo.tm_yday);

               
               
}

struct TIME_PROGRAM * borrarPrograma(struct TIME_PROGRAM *listProgram, uint8_t *elements, uint8_t element) {
    
    uint8_t i;
    struct TIME_PROGRAM *list = NULL;
    

    
    for (i=element; i< *elements - 1; i++) {
        memcpy(listProgram+i, listProgram+i+1, sizeof(*listProgram));
        //printf("pongo el elemento %d en el %d", i, i+1);
    }
    *elements = *elements - 1;
    
    list = (TIME_PROGRAM*) realloc(listProgram, sizeof(*listProgram) * (*elements) );
    
    listProgram = list;
    return list;
 
}


uint8_t modificarElemento(struct TIME_PROGRAM *listProgram, uint8_t elements, struct TIME_PROGRAM *idPrograma) {
    
    uint8_t i;
    
    for(i=0;i<elements;i++) {
        if (idPrograma->programa == listProgram[i].programa) {
            memcpy(&(listProgram[i]), idPrograma, sizeof(struct TIME_PROGRAM));
            //printf("modificarElemento--> Programacion modificada\n");
            return 0;
        }
    }
    
    
    return -1;
}

bool buscarProgramaDuplicado(char* idPrograma, struct TIME_PROGRAM *program, uint8_t elements) {
    
    if(elements == 0) {
        //printf("buscarProgramaDuplicado--> primer elemento, se devuelve false\n");
        return false;
    }
    
    if (localizarProgramaPorId(idPrograma, program, elements) < 0 ) {
        return false;
    } else {
        //printf("buscarProgramaDuplicado--> Registro %s duplicado. No se inserta\n", idPrograma);
        return true;
    }

    return true;
}

TIME_PROGRAM* generarIdPrograma(struct TIME_PROGRAM *program) {
    
    char mascara[3];
    uint8_t i;
    
    for (i=0;i<19;i++) {
        program->idPrograma[i] = '\0';
    }
    
    
    switch (program->tipo) {
        case DIARIA:
            sprintf(mascara, "%x", program->mascara);
            printf("generarIdPrograma-->Generando diaria\n");
            sprintf(program->idPrograma, "%02d%02d%02d%02d%s",
                program->tipo, program->programacion.tm_hour,
                program->programacion.tm_min,program->programacion.tm_sec, mascara );
            break;
            
        case SEMANAL:
            printf("generarIdPrograma-->Generando semanal\n");
            sprintf(program->idPrograma, "%02d%02d%02d%02d%02d", 
                program->tipo, program->programacion.tm_hour,
                program->programacion.tm_min,program->programacion.tm_sec,
                program->programacion.tm_wday);
            break;
            
        case FECHADA:
            printf("generarIdPrograma-->Generando fechada\n");
            sprintf(program->idPrograma, "%02d%02d%02d%02d%04d%02d%02d", 
                program->tipo, program->programacion.tm_hour,
                program->programacion.tm_min,program->programacion.tm_sec,
                program->programacion.tm_year, program->programacion.tm_mon,
                program->programacion.tm_mday);

            break;
            
        default:
            break;
    }
    printf("generarIdPrograma--> Generado id:%s\n", program->idPrograma);
    visualizartiempo(program->programacion);
    
    return program;
    

    
}

int localizarProgramaPorId(char *idPrograma, struct TIME_PROGRAM *program, uint8_t elements) {

    uint8_t i;
    char *cadena1;
    char *cadena2;
    char *tipo;
    cadena1 = (char*) calloc(19, sizeof(char));
    cadena2 = (char*) calloc(19, sizeof(char));
    tipo = (char*) calloc(3, sizeof(char));
    
    strncpy(tipo, idPrograma, 2);
    printf("localizarProgramaPorId--> tipo programa es %s\n", tipo);
    if ((strcmp(tipo, "00") == 0) || (strcmp(tipo, "01") == 0)) {
        //Diaria o semanal
        printf("localizarProgramaPorId--> Programacion diaria o semanal\n");
        strncpy(cadena1, idPrograma, 8);
        for(i=0;i<elements;i++) {
            strncpy(cadena2, program[i].idPrograma, 8);
             printf("localizarProgramaPorId--> comparamos %s con %s\n", cadena1, cadena2);
            if (strcmp(cadena1, cadena2) == 0) {
                free(cadena1);
                free(cadena2);
                free(tipo);
                printf("localizarProgramaPorId--> registro duplicado\n");
                return i;
            }
    }
        
        
    } else {
        //Fechada
        printf("localizarProgramaPorId--> Programacion fechada\n");
        strcpy(cadena1, idPrograma);
        for(i=0;i<elements;i++) {
            strcpy(cadena2, program[i].idPrograma);
             printf("localizarProgramaPorId--> comparamos %s con %s\n", cadena1, cadena2);
            if (strcmp(cadena1, cadena2) == 0) {
                free(cadena1);
                free(cadena2);
                free(tipo);
                printf("localizarProgramaPorId--> registro duplicado\n");
                return i;
            }
    }
        
        
    }

    printf("localizarProgramaPorId--> registro no localizado\n");
    free(cadena1);
    free(cadena2);
    free(tipo);
    return -1;
    
    

    
}


void chequear_ejecucion_programa(NTP_CLOCK *clock, TIME_PROGRAM *programs) {

	ESP_LOGI(TAG, ""TRAZAR"Chequeando si cumple algun temporizador", INFOTRAZA);

}




SCHEDULE_SEARCH buscar_programa(TIME_PROGRAM *programas, int elementos, int *programa_actual, time_t *t_tiempo_siguiente) {


	NTP_CLOCK hora;
	actualizar_hora(&hora);
	time_t hora_actual = hora.time;
	struct tm tm_dia_siguiente;
	tm_dia_siguiente = hora.date;
	int i;


	if (elementos <= 0) {
		ESP_LOGW(TAG, ""TRAZAR"No hay programas registrados", INFOTRAZA);
		*programa_actual = -2;
		*t_tiempo_siguiente = -1;
		return NO_SCHEDULE;
	}

	//ESP_LOGW(TAG, ""TRAZAR"PASAMOS POR EL ELEMENTO" , INFOTRAZA);

	//Bucle hacia atras para encontrar el programa actual

	for (i=(elementos - 1);i>-1;i--) {

		ESP_LOGW(TAG, ""TRAZAR"PASAMOS POR EL ELEMENTO %d", INFOTRAZA, i);
		if((hora_actual >= programas[i].programa) &&
				(programas[i].estadoPrograma == ACTIVO) && (programas[i].activo == ACTIVO)){

			ESP_LOGI(TAG, ""TRAZAR"ENCONTRADO SCHEDULE ID %d: "CONFIG_LONG_FORMAT"", INFOTRAZA, i, programas[i].programa);

			if (i >= 0) {
				//ESP_LOGW(TAG, ""TRAZAR"Puntero asignado al indice %d. Hora actual: "CONFIG_LONG_FORMAT" >=  "CONFIG_LONG_FORMAT"", INFOTRAZA, i, hora.time, programas[i].programa);
				*programa_actual = i;
				*t_tiempo_siguiente = programas[i+1].programa;
			}

			if(i == elementos - 1) {
				tm_dia_siguiente.tm_mday++;
				tm_dia_siguiente.tm_hour = 0;
				tm_dia_siguiente.tm_min = 0;
				tm_dia_siguiente.tm_sec = 0;
				*t_tiempo_siguiente = mktime(&tm_dia_siguiente);
				//ESP_LOGW(TAG, ""TRAZAR" EL PROXIMO INTERVALO ES "CONFIG_LONG_FORMAT" y se corresponde con la media noche", INFOTRAZA, *t_tiempo_siguiente);
				*programa_actual = i;


			}


			ESP_LOGW(TAG, ""TRAZAR"PROGRAMA ACTUAL: %d --- "CONFIG_LONG_FORMAT". SIGUIENTE: "CONFIG_LONG_FORMAT"",
					INFOTRAZA, *programa_actual,
					programas[*programa_actual].programa,
					*t_tiempo_siguiente);

			if (hora.time > programas[*programa_actual].programa + programas[*programa_actual].duracion) {
				ESP_LOGW(TAG, "NO_ACTIVE_SCHEDULE: HORA: "CONFIG_LONG_FORMAT", programa actual: "CONFIG_LONG_FORMAT", duracion: "CONFIG_UINT32_FORMAT" ",
						hora.time,
						programas[*programa_actual].programa,
						programas[*programa_actual].duracion);

				return NO_ACTIVE_SCHEDULE;
			} else {
				ESP_LOGW(TAG, "ACTIVE_SCHEDULE: HORA: "CONFIG_LONG_FORMAT", programa actual: "CONFIG_LONG_FORMAT", duracion: "CONFIG_UINT32_FORMAT" ",
						hora.time,
						programas[*programa_actual].programa,
						programas[*programa_actual].duracion);
				return ACTIVE_SCHEDULE;
			}





		} else {
			//ESP_LOGW(TAG, ""TRAZAR"elemento %d no es seleccionado, se pasa al siguiente", INFOTRAZA, i);
		}
	}

	*programa_actual = -1;
	*t_tiempo_siguiente = programas[0].programa;
	return NO_ACTIVE_SCHEDULE;


}

/**
 *
 */

SCHEDULE_SEARCH calcular_programa_activo(DATOS_APLICACION *datosApp, time_t *t_siguiente_intervalo) {

	SCHEDULE_SEARCH error;

	ESP_LOGI(TAG, ""TRAZAR"CALCULAR_PROGRAMA_ACTIVO...", INFOTRAZA);
	datosApp->datosGenerales->nProgramaCandidato = -1;
	ordenarListaProgramas(datosApp->datosGenerales->programacion, datosApp->datosGenerales->nProgramacion, datosApp->datosGenerales->clock.date);

	error = buscar_programa(datosApp->datosGenerales->programacion,
				datosApp->datosGenerales->nProgramacion,
				&datosApp->datosGenerales->nProgramaCandidato,
				t_siguiente_intervalo);


	ESP_LOGW(TAG, ""TRAZAR"RESULTADO CALCULAR_PROGRAMA_ACTIVO: %d", INFOTRAZA, error);


	return error;

}

/**
 * @fn void gestion_programas(DATOS_APLICACION*)
 * @brief Esta rutina guarda toda la logica del programador en funcion de los estados.
 *
 * @param datosApp
 */
void gestion_programas(void *arg) {


	DATOS_APLICACION *datosApp;

	datosApp = (DATOS_APLICACION*) arg;

	struct tm hora;
	actualizar_hora(&datosApp->datosGenerales->clock);
	hora = datosApp->datosGenerales->clock.date;
	static time_t t_siguiente_intervalo;


	switch(datosApp->datosGenerales->estadoApp) {

	case CHECK_SCHEDULES:
		ESP_LOGW(TAG, ""TRAZAR"CHECK_PROGRAMS", INFOTRAZA);
		if (datosApp->alarmas[ALARM_NTP].estado_alarma == ALARM_ON) {
			ESP_LOGW(TAG, ""TRAZAR"SIN CALCULO DE PROGRAMA ACTIVO POR FALLO NTP", INFOTRAZA);
		} else {
			if (datosApp->datosGenerales->nProgramacion == 0) {
				send_event(__func__,EVENT_NO_ACTIVE_SCHEDULE);
				//change_status_application(datosApp, NORMAL_SIN_PROGRAMACION);
			} else {
				ESP_LOGI(TAG, ""TRAZAR"CALCULAMOS EL PROGRAMA ACTIVO EN CHECK_PROGRAMS", INFOTRAZA);
				switch((calcular_programa_activo(datosApp, &t_siguiente_intervalo))) {

				case NO_ACTIVE_SCHEDULE:
				case NO_SCHEDULE:
					ESP_LOGI(TAG, ""TRAZAR"INTERVALO SIN PROGRAMACION ACTIVA", INFOTRAZA);
					send_event(__func__,EVENT_NO_ACTIVE_SCHEDULE);
					break;
				case ACTIVE_SCHEDULE:
					ESP_LOGI(TAG, ""TRAZAR"ENCONTRADA PROGRAMACION ACTIVA", INFOTRAZA);
					//start_schedule(datosApp);
					send_event(__func__,EVENT_START_SCHEDULE);
					//change_status_application(datosApp, NORMAL_AUTO);
					break;


				}

			}
		}

		break;
/*
	case STARTING:
		ESP_LOGW(TAG, ""TRAZAR"STARTING", INFOTRAZA);
		calcular_programa_activo(datosApp, &t_siguiente_intervalo);
		datosApp->datosGenerales->estadoApp = ESPERA_FIN_ARRANQUE;
		break;
*/
	case NORMAL_AUTO:
	case NORMAL_AUTOMAN:
	case SCHEDULING:
		if(((hora.tm_hour == 0) && (hora.tm_min == 0) && (hora.tm_sec == 0)) || (datosApp->datosGenerales->clock.time == t_siguiente_intervalo)) {

			ESP_LOGI(TAG, ""TRAZAR"EVALUACION PROXIMO PROGRAMA", INFOTRAZA);
			switch((calcular_programa_activo(datosApp, &t_siguiente_intervalo))) {

			case NO_ACTIVE_SCHEDULE:
			case NO_SCHEDULE:
				ESP_LOGI(TAG, ""TRAZAR"INTERVALO SIN PROGRAMACION ACTIVA", INFOTRAZA);
				send_event(__func__,EVENT_NO_ACTIVE_SCHEDULE);
				break;
			case ACTIVE_SCHEDULE:
				ESP_LOGI(TAG, ""TRAZAR"ENCONTRADA PROGRAMACION ACTIVA", INFOTRAZA);
				//start_schedule(datosApp);
				send_event(__func__,EVENT_START_SCHEDULE);
				//change_status_application(datosApp, NORMAL_AUTO);
				break;

			}
		} else {
			ESP_LOGD(TAG, ""TRAZAR"HORA: "CONFIG_LONG_FORMAT". siguiente intervalo: "CONFIG_LONG_FORMAT", diff: "CONFIG_LONG_FORMAT"", INFOTRAZA,
					datosApp->datosGenerales->clock.time,
					t_siguiente_intervalo,
					(t_siguiente_intervalo - datosApp->datosGenerales->clock.time ));
		}



		break;
		/*
	case NO_PROGRAM:
		ESP_LOGW(TAG, ""TRAZAR"NORMAL_SIN_PROGRAMACION", INFOTRAZA);
		break;
		*/
/*
	case NORMAL_FIN_PROGRAMA_ACTIVO:
		ESP_LOGW(TAG, ""TRAZAR"NORMAL_FIN_PROGRAMA_ACTIVO", INFOTRAZA);
		if (calcular_programa_activo(datosApp, &t_siguiente_intervalo) == ESP_OK) {
					ESP_LOGI(TAG, ""TRAZAR"FIN DE PROGRAMA REALIZADO", INFOTRAZA);
					//datosApp->datosGenerales->estadoApp = NORMAL_AUTO;
					appuser_notify_app_status(datosApp, NORMAL_AUTO);
				} else {
					ESP_LOGE(TAG, ""TRAZAR"ERROR AL AJUSTAR LOS PROGRAMAS", INFOTRAZA);

				}

		break;
*/
	default:
		break;

	}

	appuser_notify_schedule_events(datosApp);


}

esp_err_t actualizar_programa_real(DATOS_APLICACION *datosApp) {


	ESP_LOGI(TAG, ""TRAZAR"ENTRAMOS EN ACTUALIZAR PROGRAMA REAL, handle:"CONFIG_UINT32_FORMAT"", INFOTRAZA, datosApp->handle);


	if (datosApp->datosGenerales->nProgramaCandidato >= 0) {
		datosApp->datosGenerales->programa_estatico_real = datosApp->datosGenerales->nProgramaCandidato;
		ESP_LOGI(TAG, ""TRAZAR"VAMOS A ACTUALIZAR EL PROGRAMA ESTATICO", INFOTRAZA);
		nvs_set_u8(datosApp->handle, CONFIG_CLAVE_PROGRAMA_ESTATICO, datosApp->datosGenerales->programa_estatico_real);
		ESP_LOGI(TAG, ""TRAZAR"Actualizado dinamico --> estatico", INFOTRAZA);
	} else {
		nvs_get_u8(datosApp->handle, CONFIG_CLAVE_PROGRAMA_ESTATICO, &datosApp->datosGenerales->programa_estatico_real);
		datosApp->datosGenerales->nProgramaCandidato = datosApp->datosGenerales->programa_estatico_real;
		ESP_LOGI(TAG, ""TRAZAR"Actualizado estatico-->dinamico", INFOTRAZA);
	}

	ESP_LOGI(TAG, ""TRAZAR"Programa activo dinamico: %d, Programa_activo_estatico: %d", INFOTRAZA, datosApp->datosGenerales->nProgramaCandidato, datosApp->datosGenerales->programa_estatico_real);

	return ESP_OK;
}

void temporizacion_intermedia(void *arg) {

	DATOS_APLICACION *datosApp;

	datosApp = (DATOS_APLICACION*) arg;


	ESP_LOGW(TAG, ""TRAZAR"EJECUTANDO TEMPORIZADOR DE TEMPORIZACION INTERMEDIA", INFOTRAZA);
	logica_temporizacion(datosApp);
	//activacion_programa(datosApp);
}


void end_schedule(void *arg) {

	send_event(__func__,EVENT_END_SCHEDULE);


}



esp_err_t logica_temporizacion(DATOS_APLICACION *datosApp) {

	uint8_t indice;
	TIME_PROGRAM programa_actual;
	int duracion;
	int tiempo_restante;
	time_t hora;

	ESP_LOGW(TAG, ""TRAZAR"ACTIVADA LOGICA DE TEMPORIZACION", INFOTRAZA);
	indice = datosApp->datosGenerales->nProgramaCandidato;
	programa_actual = datosApp->datosGenerales->programacion[indice];
	duracion = programa_actual.duracion;
	hora = datosApp->datosGenerales->clock.time;
	tiempo_restante = (programa_actual.programa + duracion) - hora;

    const esp_timer_create_args_t first_shot_timer_args = {
            .callback = &temporizacion_intermedia,
            /* name is optional, but may help identify the timer when debugging */
            .name = "start schedule",
			.arg = (void*) datosApp
    };

    const esp_timer_create_args_t second_shot_timer_args = {
            .callback = &end_schedule,
            /* name is optional, but may help identify the timer when debugging */
            .name = "end schedule",
			.arg = (void*) datosApp
    };



	if (duracion > 0) {



		if (tiempo_restante > TEMPORIZADOR_MAXIMO_EN_SEGUNDOS ) {
			ESP_LOGW(TAG, ""TRAZAR"ACTIVADO TEMPORIZADOR DE TEMPORIZACION INTERMEDIA.  QUEDAN %d repeticiones", INFOTRAZA, tiempo_restante/TEMPORIZADOR_MAXIMO_EN_SEGUNDOS);
		    ESP_ERROR_CHECK(esp_timer_create(&first_shot_timer_args, &temporizador_duracion));
		    ESP_ERROR_CHECK(esp_timer_start_once(temporizador_duracion, (TEMPORIZADOR_MAXIMO_EN_SEGUNDOS * 1000000)));

		} else {
		    ESP_ERROR_CHECK(esp_timer_create(&second_shot_timer_args, &temporizador_duracion));
		    ESP_ERROR_CHECK(esp_timer_start_once(temporizador_duracion, (tiempo_restante * 1000000)));
		    ESP_LOGI(TAG, ""TRAZAR"ACTIVADO TEMPORIZADOR DE %d SEGUNDOS", INFOTRAZA, tiempo_restante);
		}


		if (tiempo_restante < 0 ) {
			ESP_LOGW(TAG, ""TRAZAR"LA DURACION YA HA EXCEDIDO DE LA HORA Y NO SE ACTIVA", INFOTRAZA);
			send_event(__func__,EVENT_END_SCHEDULE);
			//appuser_end_schedule(datosApp);
			return PROGRAMACION_DURACION_EXCEDIDA;
		}
	}
	return ESP_OK;
}





esp_err_t start_schedule(DATOS_APLICACION *datosApp) {



	ESP_LOGW(TAG, ""TRAZAR"start_schedule", INFOTRAZA);
	if (logica_temporizacion(datosApp) == ESP_OK) {
		ESP_LOGW(TAG, ""TRAZAR"COMIENZA EL PROGRAMA", INFOTRAZA);
		//send_event(__func__,EVENT_START_SCHEDULE);
	}



	return ESP_OK;

}

/**
 * @fn esp_err_t iniciar_gestion_programacion(DATOS_APLICACION*)
 * @brief Es el inicio del programador
 *
 * @param datosApp
 * @return
 */
esp_err_t init_schedule_service(DATOS_APLICACION *datosApp) {



    const esp_timer_create_args_t schedules_shot_timer_args = {
            .callback = &gestion_programas,
            /* name is optional, but may help identify the timer when debugging */
            .name = "end schedule",
			.arg = datosApp
    };

    ESP_ERROR_CHECK(esp_timer_create(&schedules_shot_timer_args, &temporizador));
    ESP_ERROR_CHECK(esp_timer_start_periodic(temporizador, 1000000));
    send_event(__func__,EVENT_SCHEDULES_OK);
    //change_status_application(datosApp, CHECK_PROGRAMS);


	return ESP_OK;
}

esp_err_t parar_gestion_programacion(DATOS_APLICACION *datosApp) {

	ESP_ERROR_CHECK(esp_timer_stop(temporizador));
	ESP_ERROR_CHECK(esp_timer_delete(temporizador));
	return ESP_OK;
}





