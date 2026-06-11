#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdbool.h>

// Initialize all pins (LEDs, Buttons)
void HW_Init(void);

// Read inputs
bool HW_IsMenuButtonPressed(void);

// Write outputs
void HW_SetIdleLed(bool state);

bool HW_IsSelectButtonPressed(void);
bool HW_IsCoinButtonPressed(void);

#endif // HARDWARE_H