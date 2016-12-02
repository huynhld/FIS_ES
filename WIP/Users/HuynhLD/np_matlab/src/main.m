%Test 1: create all W for each input sample

fe = [];
training = [];
full =[];
for i = 101:110
    for j = 1:8
        tic
        uri = strcat('../img/DB3_B/', int2str(i), '_', int2str(j), '.tif');
        I = imread(uri);
        [r f t] = main1(I);
        %disp(i);
        %disp(j);
        %disp(r);
        
        fu = toc;
        fe = [fe f];
        training = [training t];
        full = [full fu];
        disp('done ');
    end
end


%--------------------------------------------------------------------------
%Test2: Test n?i b? các ?nh tìm resul
%{
tic
for i = 1:5
    for j = 1:4
        try
        disp('--------------------------------------------------------');
        uri = strcat('../img/oishi/', int2str(i), '_', int2str(j), '.bmp');
        I = imread(uri);
        [r, W, neural_tem]= main1(I);
        disp(uri);
        %disp(neural_tem);
        disp(r);
        %disp(W);
        for k = 1:4
            if j ~= k
                tic
                try
                    uri = strcat('../img/oishi/', int2str(i), '_', int2str(k), '.bmp');
                    I = imread(uri);                
                    [template, core]=feature_extracting(I);
                    neural_tem=neural_template(template);
                    r = mean(W*neural_tem);
                    disp(uri);
                    %disp(neural_tem);
                    disp(r);
                catch
                    r = 0;
                    disp(uri);
                    disp(r);
                end
                toc
            end
        end
        catch
        end
    end
    disp('************************************************************');
end
toc
%}

%---------------------------------------------------------------------------
% Test: test each image with other group

tic
for i = 1:5
    for j = 1:4
        try
        
        uri = strcat('../img/oishi/', int2str(i), '_', int2str(j), '.bmp');
        I = imread(uri);
        [r, W, neural_tem] = main1(I);
        disp(r);
        catch
            W = [];
        end
        for k = 1:5
            for h = 1:4
                
                if i~=k
                    tic
                    try
                        disp(strcat( 'i:', int2str(i), ' j: ', int2str(j), ' k: ', int2str(k), ' h: ', int2str(h) ));
                        uri = strcat('../img/oishi/', int2str(k), '_', int2str(h), '.bmp');
                        I = imread(uri);                
                        [template, core]=feature_extracting(I);
                        neural_tem=neural_template(template);
                        r = mean(W*neural_tem);
                        disp(r);
                    catch
                    end
                    toc
                end
                
            end
        end
    end
end
toc