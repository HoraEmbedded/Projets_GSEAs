% Script: main_simulation.m
% Description: Main script to initialize, run and plot the trajectory tracking simulation.
% Author: Horacia

clear all;
clc;
close all;

%% 1. Initialization and Waypoints Extraction
disp('Extracting waypoints from image...');
run('waypoint.m');

%% 2. Simulation Parameters
% Scaled up for pixel-based coordinates
lookahead_dist = 20; 
desired_vel = 50; 

disp('Loading environment map...');
load('../data/environment_map.mat');

% On recalcule les mêmes indices que dans create_environment.m
num_obstacles = 2; 
intervals = linspace(0.2, 0.8, num_obstacles);
obstacles_pos = zeros(num_obstacles, 2); % Matrice [N x 2] pour stocker (X,Y)

for i = 1:num_obstacles
    idx = round(intervals(i) * length(waypoints));
    obstacles_pos(i, 1) = waypoints(idx, 1); % X de l'obstacle i
    obstacles_pos(i, 2) = waypoints(idx, 2); % Y de l'obstacle i
end


%% 3. Run Simulink Model
disp('Running Simulink model...');
% Increased StopTime to allow the robot to complete the path
out = sim('../models/trajectory_tracking_model.slx', 'StopTime', '28');

%% 4. Plot Results (Performance Visualization)
disp('Plotting results...');
figure;

% Plot the desired trajectory
plot(waypoints(:,1), waypoints(:,2), 'k--', 'LineWidth', 2); 
hold on;

% Robust data extraction: ensure matrix is N x 3
state_data = out.robot_state_sim;
if size(state_data, 1) == 3
    state_data = state_data'; % Transpose if Simulink outputs 3 x N
end

robot_X = state_data(:, 1);
robot_Y = state_data(:, 2);

% Plot the actual path
plot(robot_X, robot_Y, 'b-', 'LineWidth', 1.5); 

legend('Desired Trajectory', 'Actual Robot Path');
title('Trajectory Tracking Performance');
xlabel('X Position (pixels)');
ylabel('Y Position (pixels)');
grid on;