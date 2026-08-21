
K2 = length(out.u2);
X2 = [out.e2(2:K2)'; out.de2(2:K2)'; out.u2(1:K2-1)']; 
D2 = [out.u2(2:K2)'];

N1 = 20; 

mynet2 = newff(minmax(X1), [N 5 1], {'tansig' 'logsig' 'purelin'}, 'trainrp'); 

mynet2.trainparam.show = 50; 
mynet2.trainparam.epochs = 200000; 
mynet2.trainparam.lr = 0.05; 
mynet2.trainparam.goal = 1e-7; 

% Entraînement
Mynet2 = train(mynet2, X2, D2); 

% Génération du bloc Simulink
gensim(Mynet2); 