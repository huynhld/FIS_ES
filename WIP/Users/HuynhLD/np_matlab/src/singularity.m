function [core, delta] = singularity(R,O,N1,N2)
% Ham tim cac diem Singularity tren anh van tay
% core: Vi tri cua diem core
% delta: Vi tri tuong doi cua diem delta so voi diem core (toa do decac)
% R: Mat na vung van tay (region mask)
% O: Truong dinh huong cua anh van tay
% N1 (le): Kich thuoc "duong cong so" (curve) xem xet (theo phuong phap
% poincare) de tim diem core
% N2 (le): Kich thuoc "duong cong so" (curve) xem xet (theo phuong phap
% poincare) de tim diem delta
[m, n]=size(O);
S1(1:m,1:n)=0;
S2(1:m,1:n)=0;
%---------- Tim tat ca cac diem singularity (co the co diem loi) ----------
for i=N1+1:m-N1
    for j=N1+1:n-N1
        if (R(i-N1,j)==0)&&(R(i+N1,j)==0)&&(R(i,j-N1)==0)&&(R(i,j+N1)==0)
            P=poincare(O,N1,i,j);
            if (P==360)||(P==180)
                S1(i,j)=1; % Whorl or loop
            end;
        end;
    end;
end;
for i=N2+1:m-N2
    for j=N2+1:n-N2
        if (R(i-N2,j)==0)&&(R(i+N2,j)==0)&&(R(i,j-N2)==0)&&(R(i,j+N2)==0)
            P=poincare(O,N2,i,j);
            if P==-180
                S2(i,j)=1; % Delta
            end;
        end;
    end;
end;
%-------------------------- Tim diem core ---------------------------------
core=0.4;
i_core=[];
j_core=[];
for i=(N1+1)/2:m-(N1-1)/2
    for j=(N1+1)/2:n-(N1-1)/2
        S1_sub=S1(i-(N1-1)/2:i+(N1-1)/2,j-(N1-1)/2:j+(N1-1)/2);
        if (mean(mean(S1_sub))>=core)
            i_core=[i_core i];
            j_core=[j_core j];
        end;
    end;
end;
i_core=round(mean(i_core));
j_core=round(mean(j_core));
core=[i_core j_core];
%-------------------- Tim diem delta ben phai (neu co) --------------------
delta=0.5;
i_delta=0;
j_delta=0;

for i=i_core+(N2+1)/2:m-(N2-1)/2
    for j=j_core+(N2+1)/2:n-(N2-1)/2
        S2_sub=S2(i-(N2-1)/2:i+(N2-1)/2,j-(N2-1)/2:j+(N2-1)/2);
        if (mean(mean(S2_sub))>delta)
            delta=mean(mean(S2_sub));
            i_delta=i;
            j_delta=j;
        end;
    end;
end;
delta_right1=300;
delta_right2=300;
if (i_delta~=0)&&(j_delta~=0)
    delta_right1=i_delta-i_core;
    delta_right2=j_delta-j_core;
end;
delta_right=[delta_right1;delta_right2;0];
%-------------------- Tim diem delta ben trai (neu co)---------------------
delta=0.5;
i_delta=0;
j_delta=0;
for i=i_core+(N2+1)/2:m-(N2-1)/2
    for j=(N2+1)/2:j_core-(N2-1)/2
        S2_sub=S2(i-(N2-1)/2:i+(N2-1)/2,j-(N2-1)/2:j+(N2-1)/2);
        if (mean(mean(S2_sub))>delta)
            delta=mean(mean(S2_sub));
            i_delta=i;
            j_delta=j;
        end;
    end;
end;
delta_left1=300;
delta_left2=-300;
if (i_delta~=0)&&(j_delta~=0)
    delta_left1=i_delta-i_core;
    delta_left2=j_delta-j_core;
end;
delta_left=[delta_left1;delta_left2;0];
%--------------------------------------------------------------------------
delta=[delta_right delta_left];