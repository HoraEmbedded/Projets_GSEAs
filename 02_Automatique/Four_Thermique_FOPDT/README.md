# Industrial Oven Temperature Control

## Project Overview
This project focuses on the temperature regulation of an industrial heat treatment oven. The system is modeled as a First Order Plus Dead Time (FOPDT) process to maintain a strict temperature setpoint (850°C).

## Objectives
- Model the FOPDT thermal process.
- Tune a PID controller using Ziegler-Nichols, Cohen-Coon, and the PID Tuner App.
- Compare controller performance (overshoot, rise time, steady-state error).
- Test system robustness against disturbances (simulated oven door opening).


# Process Identification Report: Industrial Oven

## 1. Methodology
The industrial oven thermal process is modeled as a First Order Plus Dead Time (FOPDT) system. The transfer function is defined as:

$$G(s) = \frac{K \cdot e^{-Ls}}{\tau s + 1}$$

To identify the unknown parameters ($K$, $L$, and $\tau$), a open-loop step response test was conducted using MATLAB. 
- **Input Step Amplitude ($\Delta MV$):** 10% power injection at $t = 0$ seconds.
- **Output Response ($\Delta PV$):** Temperature variation monitored until steady state.

## 2. Graphical Identification Analysis

### 2.1 Process Gain ($K$)
The process gain represents the sensitivity of the system output relative to changes in the input.
- Initial temperature variation: $0^\circ\text{C}$
- Final steady-state temperature variation: $15^\circ\text{C}$
- Output change ($\Delta PV$): $15^\circ\text{C}$

$$K = \frac{\Delta PV}{\Delta MV} = \frac{15}{10} = 1.5 \text{ } ^\circ\text{C}/\%$$

### 2.2 Dead Time ($L$)
The dead time is the pure time delay before the system visually begins to react to the input step.
- Using the tangent method at the point of inflection, the line crosses the time-axis exactly at:

$$L = 12 \text{ seconds}$$

### 2.3 Time Constant ($\tau$)
According to the standard Broida/Ziegler-Nichols graphical estimation, the system reaches $63.2\%$ of its total response at time $t_{63.2\%}$.
- $63.2\%$ of total variation: $15^\circ\text{C} \times 0.632 = 9.48^\circ\text{C}$
- Graph reading shows that the temperature reaches $9.48^\circ\text{C}$ at $t = 62 \text{ seconds}$.

$$\tau = t_{63.2\%} - L = 62 - 12 = 50 \text{ seconds}$$

## 3. Identified Transfer Function Model
Substituting the identified parameters into the FOPDT mathematical structure yields the final model used for controller tuning:

$$G(s) = \frac{1.5 \cdot e^{-12s}}{50s + 1}$$