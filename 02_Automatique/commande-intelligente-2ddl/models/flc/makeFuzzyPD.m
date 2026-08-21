u_val = out.input';  
y_val = out.output';

N = length(y_val);

P = [y_val(3:N); 
     y_val(2:N-1); 
     y_val(1:N-2)];


T = u_val(2:N-1);

global net_c


net_c = feedforwardnet(51, 'traingd'); 
net_c.layers{1}.transferFcn = 'tansig';
net_c.layers{2}.transferFcn = 'purelin';

net_c.trainParam.show = 1;
net_c.trainParam.epochs = 3000;
net_c.trainParam.goal = 1e-6; 

