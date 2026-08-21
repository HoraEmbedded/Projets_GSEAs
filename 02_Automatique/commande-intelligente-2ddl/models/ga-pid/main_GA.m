clc; close all; clear all;
rand('state', sum(100*clock));

% Paramètres GA
max_generation = 200; 
max_stall_generation = 50; 
eplilon = 0.0001; 
N = 20; % Population [cite: 1119]
npar = 6; % Kp1, Ki1, Kd1, Kp2, Ki2, Kd2 [cite: 1121]
range = [0 0 0 0 0 0; 70 70 70 70 70 70]; % Bornes [cite: 1123, 1124]
dec = [2 2 2 2 2 2]; sig = [5 5 5 5 5 5]; 
Pc = 0.8; Pm = 0.1; rho = 0.02; elitism = 1; 

% Initialisation aléatoire (Utilisez une fonction init simple ou rand)
par = range(1,:) + (range(2,:) - range(1,:)) .* rand(N, npar); 
terminal = 0; generation = 0;

while ~terminal
    generation = generation + 1;
    for pop_index = 1:N
        Kp1=par(pop_index, 1); Ki1=par(pop_index, 2); Kd1=par(pop_index, 3);
        Kp2=par(pop_index, 4); Ki2=par(pop_index, 5); Kd2=par(pop_index, 6);
        sim('GA_PID_2DOF'); % Nom de votre fichier Simulink [cite: 1148]
        
        % Calcul Fitness 
        J = (e1'*e1) + (e2'*e2) + rho*(u1'*u1 + u2'*u2);
        fitness(pop_index) = 1 / (J + eps);
    end
    
    [bestfit(generation), bestchorm] = max(fitness);

    fprintf('Génération %d/%d terminée. Meilleure Fitness : %f\n', generation, max_generation, bestfit(generation));
    
    % Algorithme Évolutif [cite: 1166, 1172]
    pop = Encode_Decimal(par, sig, dec);
    parent = select_Linear_Ranking(pop, fitness, 0.5, elitism, bestchorm);
    child = Cross_Twopoint(parent, Pc, elitism, bestchorm);
    pop = Mutata_Uniform(child, Pm, elitism, bestchorm);
    par = Decode_Decimal(pop, sig, dec);
    
    % Condition d'arrêt [cite: 1193, 1200]
    if generation == max_generation, terminal = 1; end
end

% Affichage final [cite: 1211, 1216]
disp('Paramètres optimaux trouvés :');
Kp1 = par(bestchorm, 1), Ki1 = par(bestchorm, 2), Kd1 = par(bestchorm, 3)
Kp2 = par(bestchorm, 4), Ki2 = par(bestchorm, 5), Kd2 = par(bestchorm, 6)