function parent = select_Linear_Ranking(pop, fitness, eta, elitism, bestchorm)
    % Basé sur le Programme 3.2 du document [cite: 81]
    
    if (nargin < 5)
        error('Missing Number');
    end

    N = length(fitness); % Taille de la population
    [fitness, order] = sort(fitness); % Trier par fitness croissant
    
    % Calcul des probabilités de sélection basées sur le rang
    for k = 1:N
        p(k) = (eta + (2 - eta) * (k - 1) / (N - 1)) / N;
    end
    
    % Construction de la roulette cumulative
    s = zeros(1, N + 1);
    for k = 1:N
        s(k + 1) = s(k) + p(k);
    end

    for k = 1:N
        % Si élitisme est activé et c'est le meilleur chromosome
        if (elitism == 1) && (order(k) == bestchorm)
            parent(order(k), :) = pop(order(k), :);
        else
            % Sélection roulette
            r = rand * s(N + 1);
            index = find(s < r);
            if ~isempty(index)
                j = index(end);
                parent(order(k), :) = pop(order(j), :);
            else
                % Fallback au cas où
                parent(order(k), :) = pop(order(1), :); 
            end
        end
    end
end