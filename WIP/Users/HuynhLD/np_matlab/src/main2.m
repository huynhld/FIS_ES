tic

uri = strcat('../img/DB1_B/', int2str(101), '_', int2str(1), '.tif');
I = imread(uri);

[template, core]=feature_extracting(I);

neural_tem=neural_template(template);

H = 10;
nuy = 10^-7;
epsilon = 10^-4;
[V, W] = training2(neural_tem, H, nuy, epsilon);

mean(mean(mtimes(neural_tem,W)))


    for i=101:110
        for j=1:8
            try
            disp(strcat(int2str(i), ' ', int2str(j)));
            uri = strcat('../img/DB1_B/', int2str(i), '_', int2str(j), '.tif');
            I = imread(uri);
            [template, core]=feature_extracting(I);
            neural_tem=neural_template(template);
            mean(mean(mtimes(neural_tem,W)))
            catch
                disp('error');
            end
        end
    end


toc