% Script: extract_waypoints.m
% Description: Loads a PNG trajectory image, extracts and sorts (X,Y) coordinates sequentially.
% Author: Horacia


%% 1. Image Processing
img = imread('../data/track2.png');

% Check if image has color channels before converting
if size(img, 3) == 3
    gray_img = rgb2gray(img);
else
    gray_img = img;
end

% Binarize and skeletonize
bw_img = gray_img < 128; 
skel_img = bwmorph(bw_img, 'skel', Inf);

%% 2. Extract Raw Coordinates
[row, col] = find(skel_img);
raw_pts = [col, -row]; % Invert Y axis for mathematical Cartesian plane
num_pts = size(raw_pts, 1);

%% 3. Nearest Neighbor Sorting Algorithm (Corrigé)
ordered_waypoints = zeros(num_pts, 2);

% --- ÉTAPE CORRECTIVE : Trouver une extrémité de la ligne ---
% Calcule la matrice des distances entre tous les points
all_dist = squareform(pdist(raw_pts));
% Un point d'extrémité a très peu de voisins proches (seuil de distance à adapter, ex: 2 pixels)
voisins = sum(all_dist > 0 & all_dist < 2.5, 2);
[~, start_idx] = min(voisins); % On choisit le point avec le moins de voisins

% Initialisation avec l'extrémité trouvée
ordered_waypoints(1, :) = raw_pts(start_idx, :);
raw_pts(start_idx, :) = []; 

% Boucle de tri inchangée
for i = 2:num_pts
    diff = raw_pts - ordered_waypoints(i-1, :);
    dist = sum(diff.^2, 2); 
    
    [~, min_idx] = min(dist);
    
    ordered_waypoints(i, :) = raw_pts(min_idx, :);
    raw_pts(min_idx, :) = [];
end


%% 4. Downsampling & Saving
% Take 1 out of every 10 points to create a smooth, trackable path
waypoints = ordered_waypoints(1:10:end, :);

save('../data/waypoints.mat', 'waypoints');

%% 5. Verification Plot
figure;
imshow(img);
hold on;
% Re-invert Y for image plotting overlay
plot(waypoints(:,1), -waypoints(:,2), 'r-', 'LineWidth', 2); 
title('Sequentially Ordered Waypoints');