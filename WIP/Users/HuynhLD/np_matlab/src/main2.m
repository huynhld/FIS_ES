function [r W f t] = main2(I)
r = 0;
try
[template, core]=feature_extracting(I);

f = toc;
neural_tem=neural_template(template);
    
H = 10;
nuy = 10^-7;
epsilon = 10^-4;
[V, W] = training2(neural_tem, H, nuy, epsilon);
t = toc;
r = mean(mean(mtimes(neural_tem,W)));
%r = mean(mean(neural_tem,W));
catch
     if exist('f','var')
     else
         f = toc;
     end
     
     if exist('t','var')
     else
         t = toc;
     end
     
     if exist('W','var')
     else
         W = [];
     end
end