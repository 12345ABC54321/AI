idf_component_register(SRCS "data_collection.c"
                       PRIV_REQUIRES spi_flash
                       INCLUDE_DIRS "."
                       REQUIRES esp_adc driver freertos)
