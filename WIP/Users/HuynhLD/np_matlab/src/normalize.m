function [I_nor, R] = normalize(I,N)
% Ham chuan hoa muc xam cua anh I, dong thoi tim vung co van tay (region
% mask)
% I_nor: anh sau khi chuan hoa muc xam
% R: region mask
I=double(I);
[m, n]=size(I);
R(1:m,1:n)=255;
for i=1:N:m-N
    for j=1:N:n-N
        I_sub=I(i:i+N-1,j:j+N-1);
        X=[];
        for x=1:N
            X=[X I_sub(x,:)];
        end;
        M=mean(X);
        VAR=sqrt(var(X,1));
        if VAR>6
            for x=1:N
                for y=1:N
                    if I_sub(x,y)>M
                        I_sub(x,y)=100+sqrt(100*(I_sub(x,y)-M)^2/VAR);
                    else
                        I_sub(x,y)=100-sqrt(100*(I_sub(x,y)-M)^2/VAR);
                    end;
                end;
            end;
            I(i:i+N-1,j:j+N-1)=I_sub;
            R(i:i+N-1,j:j+N-1)=0;
        end;
    end;
end;
I_nor=uint8(I);