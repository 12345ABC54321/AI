#include "data_collection.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include <stdio.h>

// -----------------------------------------------------------------------------
// Variables internes
// -----------------------------------------------------------------------------

static const char *TAG = "ai_training";

// Handle ADC rendu global dans le .h
adc_oneshot_unit_handle_t s_adc_handle = NULL;

// Buffers de données (définis ici, déclarés dans le .h)
int s_x_vals[NUM_GESTURES][NUM_SAMPLES];
int s_y_vals[NUM_GESTURES][NUM_SAMPLES];

// -----------------------------------------------------------------------------
// Initialisation du joystick (ADC)
// -----------------------------------------------------------------------------

static void joystick_init()
{
    ESP_LOGI(TAG, "Initialisation de l'ADC et des canaux du joystick.");

    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_config, &s_adc_handle));

    adc_oneshot_chan_cfg_t ch_config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(s_adc_handle, JOYSTICK_X_CHANNEL, &ch_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(s_adc_handle, JOYSTICK_Y_CHANNEL, &ch_config));

    ESP_LOGI(TAG, "Joystick ADC initialisé.");
}

// -----------------------------------------------------------------------------
// Collecte des données
// -----------------------------------------------------------------------------

void collect(void)
{
    joystick_init();

    for (int g = 0; g < NUM_GESTURES; ++g) {

        printf("Début de l'enregistrement du geste #%d dans 3 secondes.\n", g + 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("3...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("2...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("1...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("GO ! Bougez le joystick...\n");

        for (int i = 0; i < NUM_SAMPLES; ++i) {
            int x = 0;
            int y = 0;

            adc_oneshot_read(s_adc_handle, JOYSTICK_X_CHANNEL, &x);
            adc_oneshot_read(s_adc_handle, JOYSTICK_Y_CHANNEL, &y);

            s_x_vals[g][i] = x;
            s_y_vals[g][i] = y;

            vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_MS));
        }

        printf("Geste #%d enregistré.\n\n", g + 1);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    printf("\nFin de la collecte.\n\n");

    // -------------------------------------------------------------------------
    // Affichage CSV final (à copier-coller dans ton dataset)
    // Format : x1,y1,x2,y2,...,x50,y50
    // Un geste par ligne
    // -------------------------------------------------------------------------

    for (int g = 0; g < NUM_GESTURES; ++g) {
        for (int i = 0; i < NUM_SAMPLES; ++i) {
            printf("%d,%d", s_x_vals[g][i], s_y_vals[g][i]);
            if (i < NUM_SAMPLES - 1) printf(",");
        }
        printf("\n");
    }

    printf("\nTerminé.\n");

    // Boucle infinie pour laisser le programme actif
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
