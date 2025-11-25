#ifndef DATA_COLLECTION_H
#define DATA_COLLECTION_H

#include "esp_adc/adc_oneshot.h"

// --- Constantes publiques ---
#define NUM_SAMPLES        50    // Nombre d'échantillons par geste
#define SAMPLE_PERIOD_MS   50    // 50 ms entre chaque lecture
#define NUM_GESTURES       30    // Nombre de gestes total à capturer

// --- GPIO / ADC Joystick ---
#define JOYSTICK_X_CHANNEL ADC_CHANNEL_6   // GPIO 34
#define JOYSTICK_Y_CHANNEL ADC_CHANNEL_7   // GPIO 35

// --- Buffers stockant les valeurs capturées ---
// Format : gesture[g][sample[i]]
extern int s_x_vals[NUM_GESTURES][NUM_SAMPLES];
extern int s_y_vals[NUM_GESTURES][NUM_SAMPLES];

// --- Handle ADC exposé si jamais tu veux lire en dehors ---
extern adc_oneshot_unit_handle_t s_adc_handle;

// --- Fonction publique ---
void collect(void);

#endif // AI_TRAINING_H
