K1 = length(out.u1);
X1 = [out.e1(2:K1)'; out.de1(2:K1)'; out.u1(1:K1-1)']; 
D1 = [out.u1(2:K1)'];
N = 20;


mynet1 = newff(minmax(X1), [N 5 1], {'tansig' 'logsig' 'purelin'}, 'trainrp'); 


mynet1.trainparam.show = 50; 
mynet1.trainparam.epochs = 200000; 
mynet1.trainparam.lr = 0.05; 
mynet1.trainparam.goal = 1e-7; 

% Entraînement
Mynet1 = train(mynet1, X1, D1); 

% Génération du bloc Simulink
gensim(Mynet1);