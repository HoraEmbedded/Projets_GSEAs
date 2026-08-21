function child = Cross_Twopoint(parent, Pc, elitism, bestchorm)
    % Basé sur le Programme 3.3 du document [cite: 115]
    
    if (nargin < 4)
        error('Missing Number');
    end
    
    [N, L] = size(parent);
    child = parent; % Initialisation
    
    for p1 = 1:N
        % Si élitisme et individu supérieur, ne pas croiser
        if (elitism == 1) && (p1 == bestchorm)
            child(p1, :) = parent(p1, :);
        else
            if Pc > rand
                % Le croisement se produit
                p2 = p1;
                while p2 == p1 % Choisir un partenaire différent
                    p2 = floor(rand * N) + 1;
                end
                
                % Choisir 2 points de croisement
                k1 = floor(rand * (L - 1)) + 1;
                k2 = k1;
                while k2 == k1
                    k2 = floor(rand * (L - 1)) + 1;
                end
                
                % S'assurer que k1 < k2
                if k1 > k2
                    t = k2; k2 = k1; k1 = t;
                end
                
                % Croisement
                child(p1, 1:k1) = parent(p1, 1:k1);
                child(p1, k1+1:k2) = parent(p2, k1+1:k2);
                child(p1, k2+1:L) = parent(p1, k2+1:L);
            else
                child(p1, :) = parent(p1, :);
            end
        end
    end
end