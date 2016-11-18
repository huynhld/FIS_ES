function O = orientation(I,R,Wo)
% Ham tim truong dinh huong cua anh I
% Wo (le): kich thuoc cua so xem xet;
I=double(I);
[m,n]=size(I);
O(1:m,1:n)=pi;
[Gy,Gx]=gradient(I);
for i=(Wo+1)/2:m-(Wo+1)/2
    for j=(Wo+1)/2:n-(Wo+1)/2
        if R(i,j)==0
            A=0;
            B=0;
            for h=i-(Wo-1)/2:i+(Wo-1)/2
                for k=j-(Wo-1)/2:j+(Wo-1)/2
                    A=A-Gx(h,k)*Gy(h,k);
                    B=B+Gx(h,k)^2-Gy(h,k)^2;
                end;
            end;
            O(i,j)=atan2(2*A,B)/2;
        end;
    end;
end;