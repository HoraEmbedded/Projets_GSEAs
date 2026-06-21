% Script: extract_waypoints.m
% Description: Loads a PNG trajectory image and extracts (X,Y) coordinates.
% Author: Horacia

% 1. Load the image from the data folder
img = imread('../data/track2.png');

% 2. Convert to grayscale (removes color dimensions)
gray_img = rgb2gray(img);

% 3. Binarize the image (Thresholding to find the black line)
% Pixels darker than 128 become 1 (true), others 0 (false)
bw_img = gray_img < 128; 

% 4. Skeletonize the line (Reduce thickness to exactly 1 pixel)
skel_img = bwmorph(bw_img, 'skel', Inf);

% 5. Extract (X, Y) coordinates of the trajectory
[row, col] = find(skel_img);
X = col;
Y = -row; % Invert Y axis to match a standard Cartesian mathematical plane

% 6. Save the waypoints into a .mat file for Simulink
waypoints = [X, Y];
save('../data/waypoints.mat', 'waypoints');

% 7. Plot the result to verify visually
figure;
imshow(img);
hold on;
plot(X, -Y, 'r.', 'MarkerSize', 5); % Red dots over the original image
title('Extracted Waypoints');