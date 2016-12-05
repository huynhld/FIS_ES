%HuynhLD

load('C:\git\FIS_ES\WIP\Users\HuynhLD\np_matlab\d\result_fixedW_np2_oishi.mat');
[row col] = size(result);

threshhold = 0;
FAR = 0;
FRR = 0;
far_l = [];
frr_l = [];

table = [];

for threshhold = 0:0.0001:0.1
    for i=1:row
        for j=2:col
            
            if(j<=5)

                if abs(result(i,1) - result(i, j)) <= threshhold
                    % OK
                else
                    % FRR
                    FRR = FRR + 1;
                    l = [i j];
                    frr_l = [frr_l ; l];
                end

            end

            if(j>5)
                if abs(result(i,1) - result(i, j)) <= threshhold
                    % FAR
                    FAR = FAR + 1;
                    l = [i j];
                    far_l = [far_l ; l];
                else
                    %oke
                end
            end
        end
    end
    %disp('**********************************');
    FAR  = FAR / (row*col);
    FRR = FRR / (row*col);
    SUM = FAR + FRR;
    r = [ threshhold FAR FRR SUM];
    table = [table ; r];
end