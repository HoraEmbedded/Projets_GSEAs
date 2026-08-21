function par = Decode_Decimal(pop, sig, dec)
    % Basé sur le Programme 3.5 du document 
    
    if (nargin < 3)
        error('Syntax: par = Decode_Decimal(pop, sig, dec)');
    end
    if (size(sig) ~= size(dec))
        error('The sig and dec are not compatible');
    end

    [N, L] = size(pop);
    d = length(sig);
    
    % Vérification de la longueur du chromosome
    if L ~= sum(sig) + d
        error('chromosome length does not match sig');
    end

    par = zeros(N, d);

    for pop_index = 1:N
        start_gene = 1;
        for par_index = 1:d
            for count = 1:sig(par_index)
                gene_index = start_gene + count;
                weight = dec(par_index) - count;
                par(pop_index, par_index) = par(pop_index, par_index) + ...
                    (pop(pop_index, gene_index)) * 10^weight;
            end
            
            % Gestion du signe (si le premier gène est < 5, c'est négatif)
            if pop(pop_index, start_gene) < 5
                par(pop_index, par_index) = -par(pop_index, par_index);
            end
            
            start_gene = start_gene + 1 + sig(par_index);
        end
    end
end