idf_component_register(SRCS "alarmas.c" "events_device.c" INCLUDE_DIRS "include" 
REQUIRES api_json
comunes
conexiones
nvslib
user_interface
configuracion
espota
programador)
