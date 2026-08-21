/*
 * robot_model.c
 *
 * Created on: May 14, 2026
 * Author: Groupe
 */

#include <math.h>

// Paramètres physiques du Robot
#define J1  0.05f
#define J2  0.05f
#define m1  1.0f
#define m2  1.0f
#define l1  1.0f
#define l2  1.0f
#define lc1 0.4f
#define lc2 0.4f
#define g   9.81f

// Fonction de dynamique du robot (Inversion d'Euler-Lagrange)
void robot_dynamics(float tau1, float tau2,
                    float th1, float th2,
                    float dth1, float dth2,
                    float *ddth1, float *ddth2)
{
    float c1, c2, c12, s2;
    float H11, H12, H21, H22;
    float h, V1, V2;
    float G1, G2;
    float det, invH11, invH12, invH21, invH22;
    float rhs1, rhs2;

    // --- Calculs trigonométriques ---
    c1  = cosf(th1);
    c2  = cosf(th2);
    s2  = sinf(th2);
    c12 = cosf(th1 + th2);

    // --- 1. Matrice d'Inertie H(q) ---
    H11 = J1 + J2 + m1*(lc1*lc1) + m2*(l1*l1 + lc2*lc2 + 2.0f*l1*lc2*c2);
    H22 = J2 + m2*(lc2*lc2);
    H12 = J2 + m2*(lc2*lc2 + l1*lc2*c2);
    H21 = H12; // La matrice est symétrique

    // --- 2. Termes de Coriolis et Centrifuge V(q, dq) ---
    h = m2 * l1 * lc2 * s2;
    V1 = -h * dth2 * (2.0f * dth1 + dth2);
    V2 = h * dth1 * dth1;

    // --- 3. Termes de Gravité G(q) ---
    G1 = m1*lc1*g*c1 + m2*g*(lc2*c12 + l1*c1);
    G2 = m2*lc2*g*c12;

    // --- 4. Calcul de l'accélération (Inversion de la matrice H) ---
    rhs1 = tau1 - V1 - G1;
    rhs2 = tau2 - V2 - G2;

    det = H11*H22 - H12*H21;

    // Sécurité contre la division par zéro (singularité)
    if (fabsf(det) < 1e-9f) {
        det = 1e-9f;
    }

    invH11 =  H22 / det;
    invH12 = -H12 / det;
    invH21 = -H21 / det;
    invH22 =  H11 / det;

    // --- 5. Remplissage des accélérations via les pointeurs ---
    *ddth1 = invH11 * rhs1 + invH12 * rhs2;
    *ddth2 = invH21 * rhs1 + invH22 * rhs2;
}
