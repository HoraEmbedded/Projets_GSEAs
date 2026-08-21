/*
 * fuzzy_controller.c
 *
 * Created on: May 14, 2026
 * Author: Groupe
 */
#include <math.h>

// Macros utilitaires
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

// Fonction d'appartenance Triangle (trimf)
float trimf(float x, float a, float b, float c) {
    if (x <= a || x >= c) return 0.0f;
    if (x == b) return 1.0f;
    if (x < b) return (x - a) / (b - a);
    return (c - x) / (c - b);
}

// Fonction d'appartenance Trapèze (trapmf)
float trapmf(float x, float a, float b, float c, float d) {
    if (x <= a || x >= d) return 0.0f;
    if (x >= b && x <= c) return 1.0f;
    if (x < b) return (x - a) / (b - a);
    return (d - x) / (d - c);
}

// Moteur d'inférence flou
float fuzzy_compute(float e_actuel, float de_actuel, float K_E, float K_DE, float K_U)
{
    // 1. Normalisation
    float E  = e_actuel * K_E;
    float DE = de_actuel * K_DE;

    if (E > 1.0f) E = 1.0f;   else if (E < -1.0f) E = -1.0f;
    if (DE > 1.0f) DE = 1.0f; else if (DE < -1.0f) DE = -1.0f;

    //  2. Fuzzification
    float mu_E[5], mu_DE[5];

    // Entrée 1 : Erreur (E)
    mu_E[0] = trapmf(E, -1.5f, -1.0f, -0.5f, -0.1f);  // NB
    mu_E[1] = trimf(E, -0.5f, -0.1f, 0.0f);           // NE
    mu_E[2] = trimf(E, -0.1f,  0.0f, 0.1f);           // ZE
    mu_E[3] = trimf(E,  0.0f,  0.1f, 0.5f);           // PO
    mu_E[4] = trapmf(E,  0.1f,  0.5f, 1.0f, 1.5f);    // PB

    // Entrée 2 : Dérivée de l'erreur (DE)
    mu_DE[0] = trimf(DE, -1.5f, -1.0f, -0.5f);        // NB
    mu_DE[1] = trimf(DE, -1.0f, -0.5f, 0.0f);         // NE
    mu_DE[2] = trimf(DE, -0.5f,  0.0f, 0.5f);         // ZE
    mu_DE[3] = trimf(DE,  0.0f,  0.5f, 1.0f);         // PO
    mu_DE[4] = trimf(DE,  0.5f,  1.0f, 1.5f);         // PB

    //  3. Base de Règles
   float centers[7] = {-1.0f, -0.66f, -0.33f, 0.0f, 0.33f, 0.66f, 1.0f};

    int rules[5][5] = {
        {0, 0, 1, 2, 3}, // Si E=NB
        {0, 1, 2, 3, 4}, // Si E=NE
        {1, 2, 3, 4, 5}, // Si E=ZE
        {2, 3, 4, 5, 6}, // Si E=PO
        {3, 4, 5, 6, 6}  // Si E=PB
    };

    // --- 4. Défuzzification (Méthode du Barycentre) ---
    float num = 0.0f;
    float den = 0.0f;
    int i_e, i_de;

    for (i_e = 0; i_e < 5; i_e++) {
        for (i_de = 0; i_de < 5; i_de++) {

                        float fire = MIN(mu_E[i_e], mu_DE[i_de]);

            if (fire > 0.0f) {
                int out_idx = rules[i_e][i_de];
                num += fire * centers[out_idx];
                den += fire;
            }
        }
    }

    // --- 5. Dénormalisation ---
    float DU_norm = (den != 0.0f) ? (num / den) : 0.0f;

    // Application du gain de sortie
    return DU_norm * K_U;
}
