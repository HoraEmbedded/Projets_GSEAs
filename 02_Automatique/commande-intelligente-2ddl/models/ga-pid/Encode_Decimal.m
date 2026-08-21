function pop = Encode_Decimal(par, sig, dec)
    % Basé sur le Programme 3.1 du document 
    % Input:
    % par: tableau 2D [Nxd] des valeurs de population
    % sig: vecteur ligne [1xd] chiffres significatifs
    % dec: vecteur ligne [1xd] position du point décimal

    if (nargin < 3)
        error('Missing number. Syntax: pop = Encode_Decimal(par, sig, dec)');
    end
    if (size(sig) ~= size(dec))
        error('The sig and dec are not compatible with each other');
    end

    [N, d] = size(par);
    
    % Initialisation de pop. La taille dépend de la somme des sig + d (signe)
    % Note: Le code original ne pré-alloue pas pop, mais c'est mieux de le faire.
    % Ici on suit la logique de la boucle du document.
    
    for pop_index = 1:N
        gene_index = 1;
        for par_index = 1:d
            % Encodage du signe
            if par(pop_index, par_index) < 0
                pop(pop_index, gene_index) = 0;
            else
                pop(pop_index, gene_index) = 9;
            end
            gene_index = gene_index + 1;
            
            % Encodage de la valeur
            temp(par_index) = abs(par(pop_index, par_index)) / 10^dec(par_index);
            for count = 1:sig(par_index)
                temp(par_index) = temp(par_index) * 10;
                val = temp(par_index) - rem(temp(par_index), 1); % Partie entière
                pop(pop_index, gene_index) = val;
                
                temp(par_index) = temp(par_index) - pop(pop_index, gene_index);
                gene_index = gene_index + 1;
            end
        end
    end
end