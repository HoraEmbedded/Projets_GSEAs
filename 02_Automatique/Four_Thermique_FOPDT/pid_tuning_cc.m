% =========================================================================
% Project 2.1: Industrial Oven Temperature Control
% Script: PID Tuning using Cohen-Coon Method
% =========================================================================

clear; clc; close all;

% --- 1. Process Parameters ---
K = 1.5;
tau = 50;
L = 12;
R = L / tau;

s = tf('s');
G_process = (K / (tau * s + 1)) * exp(-L * s);

% --- 2. Cohen-Coon Tuning Formulas ---
Kp = (tau / (K * L)) * (4/3 + R/4);
Ti = L * (32 + 6*R) / (13 + 8*R);
Td = L * 4 / (11 + 2*R);

fprintf('Cohen-Coon Parameters Calculated:\n');
fprintf('Kp = %.3f\nTi = %.3f\nTd = %.3f\n\n', Kp, Ti, Td);

% --- 3. Build the PID Controller & Closed-Loop ---
C_cc = pidstd(Kp, Ti, Td);
closed_loop_cc = feedback(C_cc * G_process, 1);

% --- 4. Simulate Response to Setpoint ---
setpoint = 850;
simulation_time = 0:1:600;
[temperature_cc, time] = step(setpoint * closed_loop_cc, simulation_time);

% --- 5. Plot the Result ---
figure('Color', [1 1 1]);
plot(time, temperature_cc, 'LineWidth', 2, 'Color', '#77AC30');
hold on;
yline(setpoint, 'k--', 'Setpoint (850°C)', 'LineWidth', 1.5);
grid on;

title('Closed-Loop Oven Temperature: Cohen-Coon PID', 'FontSize', 12, 'FontWeight', 'bold');
xlabel('Time (seconds)');
ylabel('Temperature (°C)');
xlim([0 600]);