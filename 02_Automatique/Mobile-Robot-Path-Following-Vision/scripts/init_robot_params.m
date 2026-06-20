% =========================================================================
% Initialization of Robot Parameters
% Differential Drive Mobile Robot
% =========================================================================

% Physical dimensions
R = 0.05;           % Wheel radius (m)
L = 0.30;           % Distance between wheels (m)

% Kinematic constraints
V_max = 1.0;        % Maximum linear velocity (m/s)
omega_max = 2.0;    % Maximum angular velocity (rad/s)

% Simulation parameters
Ts = 0.01;          % Simulink sampling period (s)


% PID Controller Gains


% Lateral control (Correction laterale)
Kp_lat = 0.5;
Ki_lat = 0.01;
Kd_lat = 0.1;

% Angular control (Correction angulaire)
Kp_ang = 0.8;
Ki_ang = 0.005;
Kd_ang = 0.15;

% Target speed
V_consigne = 0.5;   % Base forward speed (m/s)