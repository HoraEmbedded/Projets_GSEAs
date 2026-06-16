% =========================================================================
% Project 2.1: Industrial Oven Temperature Control
% Script: Final Comparison & Robustness Test (Door Opening Disturbance)
% =========================================================================

clear; clc; close all;

% --- 1. Process Parameters ---
K = 1.5;
tau = 50;
L = 12;
s = tf('s');
G_process = (K / (tau * s + 1)) * exp(-L * s);

% --- 2. Define the 3 Controllers ---
% Controller 1: Ziegler-Nichols (Standard Form: Kp, Ti, Td)
C_zn = pidstd(3.333, 24, 6);

% Controller 2: Cohen-Coon (Standard Form: Kp, Ti, Td)
C_cc = pidstd(3.87, 26.89, 4.18);

% Controller 3: PID Tuner App (Parallel Form: Kp, Ki, Kd)
C_app = pid(1.238, 0.025234, 0);

% --- 3. Build Transfer Functions for Tracking and Disturbance ---
% Tracking: Y(s)/R(s) = (C*G) / (1 + C*G)
T_track_zn = feedback(C_zn * G_process, 1);
T_track_cc = feedback(C_cc * G_process, 1);
T_track_app = feedback(C_app * G_process, 1);

% Disturbance Rejection (Load at plant input): Y(s)/D(s) = G / (1 + C*G)
T_dist_zn = feedback(G_process, C_zn);
T_dist_cc = feedback(G_process, C_cc);
T_dist_app = feedback(G_process, C_app);

% --- 4. Simulation Scenario Setup ---
time = 0:1:800; % Total simulation time: 800 seconds

% Reference Setpoint: 850°C from t=0
setpoint = 850 * ones(size(time));

% Disturbance: Oven door opens at t=400s (simulated as a -20% power drop)
disturbance = zeros(size(time));
disturbance(time >= 400) = -20;

% --- 5. Run the Simulation (Tracking + Disturbance) ---
% Total Response = Tracking Response + Disturbance Response
temp_zn = lsim(T_track_zn, setpoint, time) + lsim(T_dist_zn, disturbance, time);
temp_cc = lsim(T_track_cc, setpoint, time) + lsim(T_dist_cc, disturbance, time);
temp_app = lsim(T_track_app, setpoint, time) + lsim(T_dist_app, disturbance, time);

% --- 6. Plot the Final Results ---
figure('Color', [1 1 1], 'Position', [100, 100, 900, 500]);

plot(time, temp_zn, 'LineWidth', 1.5, 'Color', '#D95319'); hold on;
plot(time, temp_cc, 'LineWidth', 1.5, 'Color', '#77AC30');
plot(time, temp_app, 'LineWidth', 2.5, 'Color', '#0072BD');
yline(850, 'k--', 'Setpoint (850°C)', 'LineWidth', 1.5, 'LabelHorizontalAlignment', 'left');
xline(400, 'r:', 'Door Opened (Disturbance)', 'LineWidth', 1.5, 'LabelVerticalAlignment', 'bottom');

grid on;
title('Robustness Test: Controller Comparison with Disturbance', 'FontSize', 13, 'FontWeight', 'bold');
xlabel('Time (seconds)', 'FontSize', 11);
ylabel('Temperature (°C)', 'FontSize', 11);
legend('Ziegler-Nichols', 'Cohen-Coon', 'PID Tuner (Optimal)', 'Location', 'best');
xlim([0 800]);