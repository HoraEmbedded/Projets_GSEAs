function [the1_dd, the2_dd] = robot_dynamics(tor1, tor2, the1, the2, the1_d, the2_d)
% Fonction de dynamique du bras robotique 2-DoF
% Entrées:
%   tor1, tor2 : couples appliqués [Nm]
%   the1, the2 : angles actuels [rad]
%   the1_d, the2_d : vitesses angulaires [rad/s]
% Sorties:
%   the1_dd, the2_dd : accélérations angulaires [rad/s^2]

% Paramètres (définis globalement ou en arguments)
J1 = 0.05;
J2 = 0.05;
m1 = 1.0;
m2 = 1.0;
l1 = 1.0;
l2 = 1.0;
lc1 = 0.4;
lc2 = 0.4;
g = 9.81;

% Calcul des éléments de la matrice d'inertie M(theta)
H11 = J1 + J2 + m1*(lc1^2) + m2*((l1^2) + (lc2^2) + 2*l1*lc2*cos(the2));
H22 = J2 + m2*lc2*lc2;
H12 = J2 + m2*(lc2*lc2 + l1*lc2*cos(the2));
H21 = H12;

% Matrice d'inertie
H = [H11 H12;
     H21 H22];

% Termes de Coriolis et centrifuge
h = m2*l1*lc2*sin(the2);
C = [-h*the2_d, -h*(the1_d + the2_d);
      h*the1_d, 0];

% Termes de gravité
g1 = m1*lc1*g*cos(the1) + m2*g*(lc2*cos(the1+the2) + l1*cos(the1));
g2 = m2*lc2*g*cos(the1+the2);
G = [g1; g2];

% Couples appliqués
tor = [tor1; tor2];

% Calcul des accélérations : M*theta_dd = tor - C*theta_d - G
theta_d = [the1_d; the2_d];
theta_dd = pinv(H) * (tor - C*theta_d - G);

% Séparation des sorties
the1_dd = theta_dd(1);
the2_dd = theta_dd(2);

end