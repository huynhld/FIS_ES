% HuynhLD.
function [r f t]= main1(I)
try
    
%1. Read sample image.
%2. Find singularity points
try
[template, core]=feature_extracting(I);
f = toc;
catch
    f = 0;
end

% ans is the variable in workspace, after function feature_extracting finished.
% save('../database/ans.mat', 'ans');
%3. Create input layer from ans-list of singularity point.
neural_tem=neural_template(template);

%4. Training network
nuy = 10^-7;
epsilon = 10^-4;
D = 0;
n=16;

W=training1(neural_tem, nuy, epsilon, D, n);
t= toc;
r = mean(W*neural_tem);

catch
    r = 0;
end