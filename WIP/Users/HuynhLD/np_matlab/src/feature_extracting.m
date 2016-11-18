function [template, core] = feature_extracting(I)
% Ham xay dung tap mau cho ham matching va ham sampling (vi tri cua cac
% diem dac trung co dang toa do decac)
[J, R]=normalize(I(:,:,1),10);
O=orientation(I(:,:,1),R,9);
[core, delta]=singularity(R,O,45,25);
J=(255-double(J))/255;
J=sigmoid(J,25,0.4);
J=uint8((1-J)*255);
J=gabor_filter(J,21,6,1,2);
Rm1=minutiae(J,R,core);
%-----------------------------------------------
[J, R]=normalize(I(:,:,2),10);
J=(255-double(J))/255;
J=sigmoid(J,25,0.4);
J=uint8((1-J)*255);
J=gabor_filter(J,21,6,1,2);
Rm2=minutiae(J,R,core);
%-----------------------------------------------
[m, n]=size(Rm1);
for i=1:m
    for j=1:n
        if (Rm1(i,j)==1)||(Rm1(i,j)==2)
            label=false;
            for x=i-7:i+7
                for y=j-7:j+7
                    if (Rm2(x,y)==1)||(Rm2(x,y)==2)
                        label=true;
                        Rm2(x,y)=0;
                    end;
                end;
            end;
            if ~label
                Rm1(i,j)=0;
            end;
        end;
    end;
end;
%------------------------------------------------
template=delta;
for i=1:m
    for j=1:n
        if (Rm1(i,j)==1)||(Rm1(i,j)==2)
            mi1=i-core(1);
            mi2=j-core(2);
            i3=0;
            mi3=O(i,j)*180/pi;
            mi=[mi1;mi2;mi3];
            template=[template mi];
        end;
    end;
end;
%--------------------------------------------------------------------------
function y = sigmoid(x,a,b)
y=1./(1+exp(-a*(x-b)));