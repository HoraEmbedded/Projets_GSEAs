
clear; clc; close all;

% --- 1. Process Parameters  ---
K = 1.5;
tau = 50;
L = 12;

s = tf('s');
G_process = (K / (tau * s + 1)) * exp(-L * s);

% --- 2. Ziegler-Nichols Tuning Formulas ---
Kp = (1.2 * tau) / (K * L);
Ti = 2 * L;
Td = 0.5 * L;

fprintf('Ziegler-Nichols Parameters Calculated:\n');
fprintf('Kp = %.3f\nTi = %.3f\nTd = %.3f\n\n', Kp, Ti, Td);

% --- 3. Build the PID Controller ---
C_zn = pidstd(Kp, Ti, Td);

closed_loop_system = feedback(C_zn * G_process, 1);

% --- 5. Simulate Response to Setpoint (850°C) ---
setpoint = 850;
simulation_time = 0:1:600; % Simulate for 600 seconds

% Step response of the closed-loop system multiplied by the setpoint
[temperature, time] = step(setpoint * closed_loop_system, simulation_time);

% --- 6. Plot the Result ---
figure('Color', [1 1 1]);
plot(time, temperature, 'LineWidth', 2, 'Color', '#D95319');
hold on;
yline(setpoint, 'k--', 'Setpoint (850°C)', 'LineWidth', 1.5, 'LabelHorizontalAlignment', 'left');
grid on;

title('Closed-Loop Oven Temperature: Ziegler-Nichols PID', 'FontSize', 12, 'FontWeight', 'bold');
xlabel('Time (seconds)');
ylabel('Temperature (°C)');
xlim([0 600]);