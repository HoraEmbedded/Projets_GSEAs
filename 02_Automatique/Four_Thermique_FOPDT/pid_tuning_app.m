% =========================================================================
% Project 2.1: Industrial Oven Temperature Control
% Script: Launching PID Tuner App
% =========================================================================

clear; clc; close all;

% --- 1. Process Parameters ---
K = 1.5;
tau = 50;
L = 12;

s = tf('s');
G_process = (K / (tau * s + 1)) * exp(-L * s);

% --- 2. Launching the PID Tuner GUI ---
disp('Launching the PID Tuner App...');
disp('1. Use the sliders at the top to adjust the response.');
disp('2. Aim for a response with NO OVERSHOOT (temperature must not exceed the dashed line).');
disp('3. Click "Update Block" or note the P, I, D values given by the app.');

% This command opens the interactive tuning interface
pidTuner(G_process, 'pid');