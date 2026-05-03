idf_component_register(
    SRCS "main.c"
    PRIV_REQUIRES driver esp_adc esp_timer freertos
        INCLUDE_DIRS ".")

# Marcelo Fort Muñoz y Victor Arroyo Márquez     