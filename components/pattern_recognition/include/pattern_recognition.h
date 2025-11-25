#ifndef PATTERN_RECOGNITION_H
#define PATTERN_RECOGNITION_H

#include "esp_adc/adc_oneshot.h"
#include "tensorflow/lite/micro/micro_interpreter.h"

// ==================================================
// Constantes publiques
// ==================================================
#define NUM_SAMPLES        50
#define SAMPLE_PERIOD_MS   50

// Taille de l’arène mémoire TFLM
constexpr int TENSOR_ARENA_SIZE = 40 * 1024;

// ==================================================
// Joystick – ADC config
// ==================================================
#define JOYSTICK_X_CHANNEL ADC_CHANNEL_4   // GPIO 32
#define JOYSTICK_Y_CHANNEL ADC_CHANNEL_5   // GPIO 33

// Handle ADC (défini dans le .c)
extern adc_oneshot_unit_handle_t s_adc_handle;

// ==================================================
// TFLM – objets accessibles
// ==================================================
extern tflite::MicroInterpreter *interpreter;
extern TfLiteTensor *input_tensor;
extern TfLiteTensor *output_tensor;

// ==================================================
// Fonctions publiques
// ==================================================
void ai_init(void);                       // Joystick + TFLM init
void capture_gesture_into_input(void);    // Lit 50 samples et remplit input_tensor->data.f
int ai_predict(void);                     // Lance interpreter->Invoke() et retourne la classe

#endif // PATTERN_RECOGNITION_H
