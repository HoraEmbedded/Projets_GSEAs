% Script: create_environment.m
% Description: Generates a 2D occupancy grid with multiple obstacles distributed on the path.
% Author: Horacia

%% 1. Load the existing waypoints
load('../data/waypoints.mat');

%% 2. Initialize the Map
min_x = min(waypoints(:,1)) - 100;
max_x = max(waypoints(:,1)) + 100;
min_y = min(waypoints(:,2)) - 100;
max_y = max(waypoints(:,2)) + 100;

map = binaryOccupancyMap((max_x - min_x), (max_y - min_y), 1);
map.LocalOriginInWorld = [min_x, min_y];

%% 3. Add Multiple Obstacles Automatically
% PARAMÈTRES : Ajustez ici le nombre d'obstacles et leur taille
num_obstacles = 2; 
obstacle_size = 40; % Taille du bloc carré en pixels
half_size = round(obstacle_size / 2);

% Répartition des indices le long de la trajectoire
% Évite le tout début (0%) et la toute fin (100%) pour ne pas bloquer le départ/l'arrivée
intervals = linspace(0.2, 0.8, num_obstacles);

for i = 1:num_obstacles
    % Trouver le point de passage correspondant à l'intervalle
    idx = round(intervals(i) * length(waypoints));
    obs_x = waypoints(idx, 1);
    obs_y = waypoints(idx, 2);
    
    % Générer le bloc d'obstacle
    [x_grid, y_grid] = meshgrid(obs_x - half_size : obs_x + half_size, ...
                                obs_y - half_size : obs_y + half_size);
                            
    % Assigner l'obstacle sur la carte (1 = occupé)
    setOccupancy(map, [x_grid(:), y_grid(:)], 1);
end

%% 4. Save and Visualize
save('../data/environment_map.mat', 'map');

figure;
show(map);
hold on;
plot(waypoints(:,1), waypoints(:,2), 'r-', 'LineWidth', 2);
title(['Environment Map with Obstacles']);
