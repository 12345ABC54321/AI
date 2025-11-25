#include "data_collection.h"
#include "pattern_recognition.h"

extern "C" void app_main(void) {
    // collect();

    joystick_init();
    tflm_init();

    int gesture_id = 0;

    while (true) {
        ESP_LOGI(TAG, "Geste #%d : préparation (3 secondes)...", gesture_id);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "3...");
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "2...");
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "1...");
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "GO ! Faites votre geste maintenant...");
        capture_gesture_into_input();

        interpreter->Invoke();

        float p0 = output_tensor->data.f[0]; // probabilité classe 0
        float p1 = output_tensor->data.f[1]; // probabilité classe 1

        printf("Sortie brute : class0=%.3f class1=%.3f\n", p0, p1);

        if (p0 > 0.7f) {
            ESP_LOGI(TAG, "Geste spécial détecté (classe 0)");
        } else {
            ESP_LOGI(TAG, "Autre / pas de geste (classe 1)");
        }

        gesture_id++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
