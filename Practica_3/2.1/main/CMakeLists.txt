idf_component_register(
    SRCS "ejercicio_base.c" "main.c"
    PRIV_REQUIRES driver esp_adc esp_timer
    INCLUDE_DIRS ".")
    