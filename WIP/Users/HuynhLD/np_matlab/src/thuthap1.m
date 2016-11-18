function [FAR, FRR] = thuthap1(threshold)
False_match=0;
False_non_match=0;
load('C:\neural matching\database\NDB.mat'); %load NDB
for n=1:NDB
    filename=['C:\neural matching\template\matching\tem' num2str(n)];
    load(filename,'matching_tem'); % load matching_tem
    match_min=threshold;
    j_min=0;
    for j=1:NDB
        filename=['C:\neural matching\database\1\db' num2str(j)];
        load(filename); % load W
        match=W*matching_tem(:,1i);
        if match<match_min
            match_min=match;
            j_min=j;
        end;
    end;
    if j_min~=0
        if j_min~=n
            False_match=False_match+1;
        end;
    else
        False_non_match=False_non_match+1;
    end;
end;
FAR=False_match/(NDB);
FRR=False_non_match/(NDB);