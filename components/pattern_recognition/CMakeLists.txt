idf_component_register(SRCS "pattern_recognition.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES esp-tflite-micro esp_adc driver freertos log)
