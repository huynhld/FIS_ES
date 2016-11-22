% HuynhLD.

%1. Read sample image.
I = imread('../img/104_1.tif');

%2. Find singularity points
[template, core]=feature_extracting(I);

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

mean(W*neural_tem)
