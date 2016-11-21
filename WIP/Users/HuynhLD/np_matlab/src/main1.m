% HuynhLD.


%1. Read sample image.
%I = imread('../img/1_1.bmp');

%2. Find singularity points
%feature_extracting(I);

% ans is the variable in workspace, after function feature_extracting finished.
% save('../database/ans.mat', 'ans');
%3. Create input layer from ans-list of singularity point.
%neural_template(ans);

% Load n_temp - input layer.
%load('../database/n_temp.mat', 'n_tem');

%4. Training network
nuy = 10^-7;
epsilon = 10^-4;
D = 0;
n=16;
n_tem = [   300.0000
            300.0000
            109.0000
            -65.0000
            101.8667
            49.6667
            15.0000
            91.1000
            49.6000
            10.0000
            20.3733
            9.9333
            5.0000
            93.8000
            57.2000
            5.0000];

training1(n_tem, nuy, epsilon, D, n);
load('../database/w.mat', 'W');

mean(W*n_tem)

n_tem2 = [  300.0000
            300.0000
            109.0000
            -65.0000
            105.8000
            50.2667
            15.0000
            97.1000
            48.9000
            10.0000
            21.1600
            10.0533
            5.0000
            89.2000
            58.6000
            5.0000];
mean(W*n_tem2)