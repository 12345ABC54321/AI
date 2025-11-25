#include "pattern_recognition.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"

#include "model_data.h"

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"

// -----------------------------------------------------------------------------
// Logs
// -----------------------------------------------------------------------------
static const char *TAG = "ai";

// -----------------------------------------------------------------------------
// Variables globales définies (déclarées en extern dans le .h)
// -----------------------------------------------------------------------------
adc_oneshot_unit_handle_t s_adc_handle = nullptr;

// Arène mémoire TFLM
uint8_t tensor_arena[TENSOR_ARENA_SIZE];

// Pointeurs TFLM
tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *input_tensor = nullptr;
TfLiteTensor *output_tensor = nullptr;

// Canaux joystick (privés au module)
static adc_channel_t s_joystick_x_channel = JOYSTICK_X_CHANNEL; // GPIO 32
static adc_channel_t s_joystick_y_channel = JOYSTICK_Y_CHANNEL; // GPIO 33

// -----------------------------------------------------------------------------
// Init ADC joystick
// -----------------------------------------------------------------------------
static void joystick_init()
{
    adc_oneshot_unit_init_cfg_t unit_config{};
    unit_config.unit_id = ADC_UNIT_1;
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_config, &s_adc_handle));

    adc_oneshot_chan_cfg_t ch_config{};
    ch_config.atten    = ADC_ATTEN_DB_12;      // ~0..3.3V
    ch_config.bitwidth = ADC_BITWIDTH_12;      // 0..4095

    ESP_ERROR_CHECK(adc_oneshot_config_channel(s_adc_handle, s_joystick_x_channel, &ch_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(s_adc_handle, s_joystick_y_channel, &ch_config));

    ESP_LOGI(TAG, "Joystick ADC initialisé.");
}

// -----------------------------------------------------------------------------
// Init TFLM
// -----------------------------------------------------------------------------
static void tflm_init()
{
    const tflite::Model *model = tflite::GetModel(model_data);

    // Même set d’opérateurs que ceux utilisés à l’entraînement
    static tflite::MicroMutableOpResolver<3> resolver;
    resolver.AddFullyConnected();
    resolver.AddRelu();
    resolver.AddSoftmax();

    // Interpréteur statique (pas de malloc/free)
    static tflite::MicroInterpreter static_interpreter(
        model,
        resolver,
        tensor_arena,
        TENSOR_ARENA_SIZE
    );

    interpreter = &static_interpreter;

    TfLiteStatus alloc_status = interpreter->AllocateTensors();
    if (alloc_status != kTfLiteOk) {
        ESP_LOGE(TAG, "Échec AllocateTensors()");
        return;
    }

    input_tensor  = interpreter->input(0);
    output_tensor = interpreter->output(0);

    ESP_LOGI(TAG, "TFLM initialisé. input dims: %d, output dims: %d",
             input_tensor->dims->size, output_tensor->dims->size);
}

// -----------------------------------------------------------------------------
// API publique
// -----------------------------------------------------------------------------

void ai_init(void)
{
    joystick_init();
    tflm_init();
}

// Capture NUM_SAMPLES mesures joystick dans input_tensor->data.f
void capture_gesture_into_input(void)
{
    if (!input_tensor) {
        ESP_LOGE(TAG, "capture_gesture_into_input() appelé avant ai_init()");
        return;
    }

    float *in = input_tensor->data.f;
    const float norm = 1.0f / 4095.0f;

    for (int i = 0; i < NUM_SAMPLES; ++i) {
        int x = 0;
        int y = 0;

        adc_oneshot_read(s_adc_handle, s_joystick_x_channel, &x);
        adc_oneshot_read(s_adc_handle, s_joystick_y_channel, &y);

        // flatten: [x0, y0, x1, y1, ...]
        in[2 * i]     = x * norm;
        in[2 * i + 1] = y * norm;

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_MS));
    }
}