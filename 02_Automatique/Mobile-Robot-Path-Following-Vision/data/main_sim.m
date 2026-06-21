% Script: main_simulation.m
% Description: Main script to initialize, run and plot the trajectory tracking simulation.
% Author: Horacia

%% 1. Initialization and Waypoints Extraction
disp('Extracting waypoints from image...');
% Runt the script we created earlier
run('waypoint.m');

%% 2. Simulation Parameters
% Set the distance the robot looks ahead (in meters or pixels)
lookahead_dist = 5; 
% Set the robot's constant speed
desired_vel = 2; 

%% 3. Run Simulink Model
disp('Running Simulink model...');
% Run the simulation for 30 seconds
out = sim('../models/trajectory_tracking_model.slx', 'StopTime', '30');

%% 4. Plot Results (Performance Visualization)
disp('Plotting results...');
figure;

% Plot the desired trajectory (the black line we extracted)
plot(waypoints(:,1), waypoints(:,2), 'k--', 'LineWidth', 2); 
hold on;

% Extract actual X and Y coordinates the robot drove through
% Assuming the output is an N x 3 matrix [X, Y, Theta]
robot_X = out.robot_state_sim(:, 1);
robot_Y = out.robot_state_sim(:, 2);

% Plot the actual path the robot took in blue
plot(robot_X, robot_Y, 'b-', 'LineWidth', 1.5); 

legend('Desired Trajectory', 'Actual Robot Path');
title('Trajectory Tracking Performance');
xlabel('X Position');
ylabel('Y Position');
grid on;