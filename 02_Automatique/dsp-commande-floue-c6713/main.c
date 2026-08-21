#include <stdio.h>
#include <math.h>

#define CHIP_6713
// Bibliothèques matérielles
#include "dsk6713.h"
#include "dsk6713_aic23.h"

// Configuration de la simulation
#define TS 0.000125f

extern float fuzzy_compute(float e_actuel, float de_actuel, float K_E, float K_DE, float K_U);
extern void robot_dynamics(float tau1, float tau2, float th1, float th2, float dth1, float dth2, float *ddth1, float *ddth2);

void main(void) {
    // Initialisation matérielle
    DSK6713_AIC23_Config config = DSK6713_AIC23_DEFAULTCONFIG;
    DSK6713_AIC23_CodecHandle hCodec;

    DSK6713_init();
    hCodec = DSK6713_AIC23_openCodec(0, &config);
    DSK6713_AIC23_setFreq(hCodec, DSK6713_AIC23_FREQ_8KHZ);

    // États du robot
    float q1 = 0.0f, q2 = 0.0f;
    float dq1 = 0.0f, dq2 = 0.0f;
    float ddq1 = 0.0f, ddq2 = 0.0f;

    float e1 = 0.0f, e2 = 0.0f;
    float de1 = 0.0f, de2 = 0.0f;
    float q1_prev = 0.0f, q2_prev = 0.0f;
    float tau1 = 0.0f, tau2 = 0.0f;

    float ref1 = 0.0f, ref2 = 0.0f;
    float temps_t = 0.0f;

    // Variables pour l'oscilloscope
    short sortie_gauche_16b;
    short sortie_droite_16b;
    Uint32 sortie_audio_32b;

    printf("--- SIMULATION TEMPS RÉEL SUR OSCILLOSCOPE EN COURS ---\n");

    // Boucle infinie temps réel
    while(1) {

        // 1. Génération de la consigne (Sinusoïde 0.5 Hz)
        ref1 = 30.0f * sinf(2.0f * 3.14159f * 0.5f * temps_t) * (3.14159f / 180.0f);
        ref2 = 15.0f * cosf(2.0f * 3.14159f * 0.5f * temps_t) * (3.14159f / 180.0f);

        // 2. Calcul de l'erreur
        e1 = ref1 - q1;
        e2 = ref2 - q2;
        de1 = (q1 - q1_prev) / TS;
        de2 = (q2 - q2_prev) / TS;
        q1_prev = q1;
        q2_prev = q2;

        // 3. Contrôleur Flou
        tau1 = fuzzy_compute(e1, de1, 9.55f, 0.95f, 40.0f);
        tau2 = fuzzy_compute(e2, de2, 7.96f, 0.16f, 10.0f);

        // Saturation
        if(tau1 >  50.0f) tau1 =  50.0f; if(tau1 < -50.0f) tau1 = -50.0f;
        if(tau2 >  50.0f) tau2 =  50.0f; if(tau2 < -50.0f) tau2 = -50.0f;

        // 4. Dynamique
        robot_dynamics(tau1, tau2, q1, q2, dq1, dq2, &ddq1, &ddq2);

        // 5. Intégration
        dq1 = dq1 + ddq1 * TS;
        dq2 = dq2 + ddq2 * TS;
        q1 = q1 + dq1 * TS;
        q2 = q2 + dq2 * TS;

        // Avancement du temps
        temps_t += TS;

        // 6. Envoi vers l'oscilloscope
        sortie_gauche_16b = (short)(q1 * 10000.0f);
        sortie_droite_16b = (short)(q2 * 10000.0f);

        // Fusion des canaux
        sortie_audio_32b = ((Uint32)sortie_gauche_16b << 16) | (sortie_droite_16b & 0xFFFF);

        while (!DSK6713_AIC23_write(hCodec, sortie_audio_32b));
    }
}
