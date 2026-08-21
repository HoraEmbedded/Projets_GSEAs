function newpop = Mutata_Uniform(pop, Pm, elitism, bestchorm)
    % Basé sur le Programme 3.4 du document [cite: 165]
    
    if (nargin < 4)
        error('Lack of numbers');
    end
    
    [N, L] = size(pop);
    newpop = pop;
    
    for pop_index = 1:N
        % Ne pas muter le meilleur si élitisme activé
        if (elitism == 0) || (elitism == 1 && pop_index ~= bestchorm)
            for gene_index = 1:L
                if Pm > rand
                    rand_gene = rand * 10;
                    % S'assurer que le nouveau gène est différent de l'actuel
                    while (pop(pop_index, gene_index) == floor(rand_gene) || rand_gene == 10)
                        rand_gene = rand * 10;
                    end
                    newpop(pop_index, gene_index) = floor(rand_gene);
                end
            end
        end
    end
end