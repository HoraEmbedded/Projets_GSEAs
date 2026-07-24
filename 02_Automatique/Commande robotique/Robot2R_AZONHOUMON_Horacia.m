%% Script MATLAB : Robot plan 2R avec PID

clc; clear; close all;

%% Définition du robot
robot = rigidBodyTree('DataFormat','column','MaxNumBodies',2);

% Link 1
L1 = 1; % longueur en m
body1 = rigidBody('link1');
joint1 = rigidBodyJoint('joint1','revolute');
setFixedTransform(joint1,trvec2tform([0 0 0]));
body1.Joint = joint1;
addBody(robot,body1,'base');

% Link 2
L2 = 1; % longueur en m
body2 = rigidBody('link2');
joint2 = rigidBodyJoint('joint2','revolute');
setFixedTransform(joint2,trvec2tform([L1 0 0]));
body2.Joint = joint2;
addBody(robot,body2,'link1');

%% Paramètres PID
Kp = [50; 50];
Ki = [10; 10];
Kd = [5; 5];

q_d = [deg2rad(30); deg2rad(-15)]; % consignes

%% Initialisation
q = zeros(2,1);
qd = zeros(2,1); % vitesse articulaire
e_int = zeros(2,1); % erreur intégrale
dt = 0.01;
T = 5; % durée simulation
time = 0:dt:T;

q_log = zeros(2,length(time));

%% Simulation PID
for k = 1:length(time)
    e = q_d - q;
    e_int = e_int + e*dt;
    e_der = -qd;
    
    tau = Kp.*e + Ki.*e_int + Kd.*e_der; % commande PID simple
    
    % Modèle dynamique simplifié (masse unitaire)
    qdd = tau;
    
    % Intégration
    qd = qd + qdd*dt;
    q = q + qd*dt;
    
    q_log(:,k) = q;
end

%% Affichage
figure;
plot(time, rad2deg(q_log(1,:)),'r','LineWidth',1.5); hold on;
plot(time, rad2deg(q_log(2,:)),'b','LineWidth',1.5);
yline(30,'r--'); yline(-15,'b--');
xlabel('Temps [s]'); ylabel('Angle [°]');
legend('q1','q2','q1 consigne','q2 consigne');
grid on;
title('Suivi de trajectoire avec PID');
