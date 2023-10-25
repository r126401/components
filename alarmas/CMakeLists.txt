idf_component_register(SRCS "alarmas.c" "events_device.c" INCLUDE_DIRS "include" 
REQUIRES api_json
comunes
conexiones
nvslib
interfaz_usuario
configuracion
espota
programador)
