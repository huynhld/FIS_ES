%Test 1: create all W for each input sample
%{
fe = [];
training = [];
full =[];

path = {'f0001_01', 'f0002_05', 'f0003_10', 'f0007_09', 's0249_04','s0240_01', 's0237_07', 's0238_07', 's0248_02', 'f0001_01', 'f0002_05', 'f0003_10', 'f0007_09', 's0249_04','s0240_01', 's0237_07', 's0238_07', 's0248_02', 'f0001_01', 'f0002_05', 'f0003_10', 'f0007_09', 's0249_04','s0240_01', 's0237_07', 's0238_07', 's0248_02', 'f0001_01', 'f0002_05', 'f0003_10', 'f0007_09', 's0249_04','s0240_01', 's0237_07', 's0238_07', 's0248_02', 'f0001_01', 'f0002_05', 'f0003_10', 'f0007_09', 's0249_04','s0240_01', 's0237_07', 's0238_07', 's0248_02', 'f0001_01', 'f0002_05', 'f0003_10', 'f0007_09', 's0249_04','s0240_01', 's0237_07', 's0238_07', 's0248_02', 'f0001_01', 'f0002_05', 'f0003_10', 'f0007_09', 's0249_04','s0240_01', 's0237_07', 's0238_07', 's0248_02', 'f0001_01', 'f0002_05', 'f0003_10', 'f0007_09', 's0249_04','s0240_01', 's0237_07', 's0238_07', 's0248_02'};
%for i = 101:110
    for j = 1:72
        tic
        %uri = strcat('../img/nist_fig0/figs_0', int2str(i), '_', int2str(j), '.tif');
        uri = strcat('../img/nist_fig0/figs_0/', path{j}, '.png');
        
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
%end
%}


%--------------------------------------------------------------------------
%Test2: Test n?i b? các ?nh tìm resul
%{
result  [];
for i = 1:5
    for j = 1:4
        rs = [];
        try
        disp('--------------------------------------------------------');
        uri = strcat('../img/oishi/', int2str(i), '_', int2str(j), '.bmp');
        I = imread(uri);
        [r, W, neural_tem]= main1(I);
        disp(uri);
        rs = [rs r];
        %disp(neural_tem);
        
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
                    rs = [rs r];
                    %disp(uri);
                    %disp(neural_tem);
                    %disp(r);
                catch
                    r = 0;
                    %disp(uri);
                    %disp(r);
                    rs = [rs r];
                end
                toc
            end
        end
        catch
        end
    end
    disp('************************************************************');
end
%}

%---------------------------------------------------------------------------
% Test: test each image with other group

%tic
result = [];

for i = 1:5
    for j = 1:4
        rs = [];
        try
        uri = strcat('../img/oishi/', int2str(i), '_', int2str(j), '.bmp');
        I = imread(uri);
        [r, W, neural_tem] = main1(I);
        disp(r);
        rs = [rs r];
        disp(uri);
        catch
            W = [];
        end
        
        for k = 1:5
            for h = 1:4
                
                %if i~=k
                 
                    try
                        disp(strcat( 'i:', int2str(i), ' j: ', int2str(j), ' k: ', int2str(k), ' h: ', int2str(h) ));
                        uri = strcat('../img/oishi/', int2str(k), '_', int2str(h), '.bmp');
                        I = imread(uri);                
                        [template, core]=feature_extracting(I);
                        neural_tem=neural_template(template);
                        r = mean(W*neural_tem);
                        disp(r);
                        rs = [rs r];
                        %disp(r);
                        disp('done');
                    catch
                        r = 0;
                        rs = [rs r];
                    end
                %end
                
            end
        end
        result = [result ; rs];
    end
end
%toc