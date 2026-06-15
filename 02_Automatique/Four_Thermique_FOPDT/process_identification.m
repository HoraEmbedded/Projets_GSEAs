% =========================================================================
% Project 2.1: Industrial Oven Temperature Control
% Script: Process Identification using a 10% Step Input
% Domain: Industrial Control / Thermal Systems
% =========================================================================

clear; clc; close all;


K_true = 1.5;       % True process gain (°C/%)
tau_true = 50;      % True time constant (seconds)
L_true = 12;        % True dead time (seconds)

% Create the FOPDT Transfer Function Model

s = tf('s');
% G(s) = (K * e^(-L*s)) / (tau * s + 1)
oven_process = (K_true / (tau_true * s + 1)) * exp(-L_true * s);

% Simulate a 10% Step Input
step_amplitude = 10; % 10% power input change
simulation_time = 0:1:300; % Simulate for 300 seconds

% Simulate the response to the step input
[temperature, time] = lsim(oven_process, step_amplitude * ones(size(simulation_time)), simulation_time);

% Plot the Graphical Results 
figure('Color', [1 1 1]);
plot(time, temperature, 'LineWidth', 2.5, 'Color', [0 0.4470 0.7410]);
grid on;

% Formatting the plot professionally
title('Oven Temperature Response to a 10% Step Input', 'FontSize', 12, 'FontWeight', 'bold');
xlabel('Time (seconds)', 'FontSize', 11);
ylabel('Temperature Variation (\Delta°C)', 'FontSize', 11);
xlim([0 300]);
ylim([0 18]);

disp('Simulation completed. Use the plot to visually identify K, tau, and L.');