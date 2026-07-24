
clc; clear; close all;


L1 = 0.5; 
L2 = 0.4; 


R_max = L1 + L2; 
R_min = abs(L1 - L2); 

fprintf('Position maximale (Rayon) : %.2f m\n', R_max);
fprintf('Position minimale (Rayon) : %.2f m\n', R_min);


n = 15000; 
q1 = rand(1, n) * 2 * pi;
q2 = rand(1, n) * 2 * pi;


x = L1*cos(q1) + L2*cos(q1 + q2);
y = L1*sin(q1) + L2*sin(q1 + q2);


figure('Color', 'w');
plot(x, y, '.', 'Color', [0.8 0.8 0.8]); 
hold on; axis equal; grid on;

 
theta = linspace(0, 2*pi, 100);
plot(R_max*cos(theta), R_max*sin(theta), 'r--', 'LineWidth', 2); 
plot(R_min*cos(theta), R_min*sin(theta), 'g--', 'LineWidth', 2); 


plot(R_max, 0, 'ro', 'MarkerFaceColor', 'r', 'MarkerSize', 8); 
plot(R_min, 0, 'go', 'MarkerFaceColor', 'g', 'MarkerSize', 8); 

title('Espace de Travail et Limites du Robot 2R');
xlabel('X (m)'); ylabel('Y (m)');
legend('Points atteignables', 'Position Max', 'Position Min');