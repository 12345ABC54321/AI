idf_component_register(SRCS "main.cc"
                       INCLUDE_DIRS "."
                       REQUIRES esp-tflite-micro esp_adc driver data_collection, pattern_recognition)
