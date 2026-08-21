% Program 3.6: Genetic algorithm program to find PID parameter set
clc;
close all;
clear all;

% Initialisation du générateur aléatoire
rand('state', sum(100*clock));

%% 1. Déclaration des variables [cite: 233]
max_generation = 200;       % Nombre max de générations [cite: 237]
max_stall_generation = 50;  % Arrêt si pas d'amélioration pendant 50 gen [cite: 238]
epsilon = 0.0001;           % Seuil de convergence [cite: 239]

N = 20;                     % Taille de la population [cite: 240]
npar = 6;                   % 6 paramètres: Kp1, Ki1, Kd1, Kp2, Ki2, Kd2 [cite: 242]

% Plages de recherche pour les PID (Min ligne 1, Max ligne 2)
range = [0  0  0  0  0  0; ...
         70 70 70 70 70 70]; % [cite: 244-246]

% Paramètres de codage décimal
sig = [5 5 5 5 5 5];    % Chiffres significatifs [cite: 245]
dec = [2 2 2 2 2 2];    % Position décimale [cite: 247]

Pc = 0.8;   % Probabilité de croisement [cite: 250]
Pm = 0.1;   % Probabilité de mutation [cite: 251]
rho = 0.02; % Facteur de pénalité pour l'énergie de commande [cite: 253]
elitism = 1; % Élitisme activé [cite: 254]

%% 2. Initialisation de la population
disp('Initialisation de la population...');
par = Init(N, npar, range); % [cite: 260]

terminal = 0;
generation = 0;
stall_generation = 0;
bestfit = []; % Pour stocker l'historique


%% 3. Boucle principale de l'Algorithme Génétique
while ~terminal
    generation = generation + 1;
    disp(['Generation #' num2str(generation) ' of maximum ' num2str(max_generation)]);
    
    % --- Évaluation de la population ---
    for pop_index = 1:N
        % Extraction des paramètres pour l'individu courant
        Kp1 = par(pop_index, 1);
        Ki1 = par(pop_index, 2);
        Kd1 = par(pop_index, 3);
        Kp2 = par(pop_index, 4);
        Ki2 = par(pop_index, 5);
        Kd2 = par(pop_index, 6);
        
        % Envoi des variables dans le Workspace pour Simulink
        assignin('base', 'Kp1', Kp1);
        assignin('base', 'Ki1', Ki1);
        assignin('base', 'Kd1', Kd1);
        assignin('base', 'Kp2', Kp2);
        assignin('base', 'Ki2', Ki2);
        assignin('base', 'Kd2', Kd2);
        
        try
            % Lancement de la simulation
            sim('GA_PID_2DOF'); 
            
            % Calcul de la fonction de coût J [cite: 276]
            % On suppose que Simulink renvoie e1, e2, u1, u2 dans le workspace
            % J = erreur + rho * effort_commande
            J = (e1'*e1) + (e2'*e2) + rho * (u1'*u1 + u2'*u2);
            fitness(pop_index) = 1 / (J + eps); % Fitness est l'inverse du coût
            
        catch
            % En cas d'erreur de simulation (instabilité), on punit l'individu
            J = 10^100;
            fitness(pop_index) = 1 / (J + eps);
        end
    end
    
    % --- Sauvegarde du meilleur de cette génération ---
    [best_val, best_idx] = max(fitness);
    bestfit(generation) = best_val;
    bestchrom = best_idx;
    
    % --- Vérification des conditions d'arrêt [cite: 316-327] ---
    if generation == max_generation
        terminal = 1;
    elseif generation > 1
        if abs(bestfit(generation) - bestfit(generation-1)) < epsilon
            stall_generation = stall_generation + 1;
            if stall_generation == max_stall_generation
                terminal = 1;
                disp('Convergence atteinte (Stall Generation).');
            end
        else
            stall_generation = 0;
        end
    end
    
    if terminal
        break;
    end
    
    % --- Opérateurs Génétiques (Si on ne s'arrête pas) ---
    
    % 1. Encodage
    pop = Encode_Decimal(par, sig, dec);
    
    % 2. Sélection (Linear Ranking)
    parent = select_Linear_Ranking(pop, fitness, 0.5, elitism, bestchrom);
    
    % 3. Croisement (Two-point)
    child = Cross_Twopoint(parent, Pc, elitism, bestchrom);
    
    % 4. Mutation (Uniform)
    % Note: dans le code original, la sortie de mutation s'appelle pop
    pop_new = Mutata_Uniform(child, Pm, elitism, bestchrom);
    
    % 5. Décodage
    par = Decode_Decimal(pop_new, sig, dec);
    
end

%% 4. Résultats finaux [cite: 330-340]
figure;
plot(1./bestfit);
title('Evolution de la fonction de coût (J)');
xlabel('Génération');
ylabel('Coût (Erreur + Commande)');

% Affichage des meilleurs paramètres
disp('Paramètres optimaux trouvés :');
Kp1 = par(bestchrom, 1)
Ki1 = par(bestchrom, 2)
Kd1 = par(bestchrom, 3)
Kp2 = par(bestchrom, 4)
Ki2 = par(bestchrom, 5)
Kd2 = par(bestchrom, 6)

% Simulation finale avec les meilleurs paramètres
assignin('base', 'Kp1', Kp1);
assignin('base', 'Ki1', Ki1);
assignin('base', 'Kd1', Kd1);
assignin('base', 'Kp2', Kp2);
assignin('base', 'Ki2', Ki2);
assignin('base', 'Kd2', Kd2);

sim('GA_PID_2DOF');

% Ici vous pouvez ajouter le code pour tracer les courbes de réponse (theta vs reference)
% comme dans la Figure 3.5