function par = Init(N, npar, range)
    % Initialise la population avec des valeurs aléatoires dans les plages données
    % N: Taille de la population
    % npar: Nombre de paramètres
    % range: Matrice [2 x npar] contenant [min; max] pour chaque paramètre
    
    par = zeros(N, npar);
    for i = 1:npar
        par_min = range(1, i);
        par_max = range(2, i);
        % Génération uniforme entre min et max
        par(:, i) = par_min + (par_max - par_min) * rand(N, 1);
    end
end