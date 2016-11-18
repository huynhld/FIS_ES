function [V, W] = training2(X,H,nuy,epsilon)
% Ham huan luyen mang neural perceptron hai lop (mot lop an va mot lop ra)
% Ham tac dong lop an la ham sigmoid, ham tac dong lop ra la ham tuyen tinh
% X(N,K): ma tran cua cac vector mau ngo vao (K mau, moi mau co N gia tri)
% nuy: hang so hoc
% epsilon: nguong huan luyen
% V,W: cac vector trong so duoc huan luyen cua lop an va lop ra
[N, K]=size(X);
%-------------- Tao gia tri khoi dong --------------------------
V(1:H,1:N)=0.1/(N-1);
W(1:H)=1;
for i=1:H
    for j=1:N
        if j==i
            V(i,j)=-0.1;
        end;
    end;
    if mod(i,2)==0 % i chan
        W(i)=-1;
    end;
end;
%---------------------------------------------------------------
epoch=0;
J=epsilon;
dnuy=0;
while (epoch<1000)&&(J>=epsilon)
    nuy=nuy+dnuy;
    for k=1:K
        %----------Lan truyen thuan--------------------------
        for q=1:H
            netq=V(q,:)*X(:,k);
            z(q)=sigmoid(netq);
        end;
        %----------Lan truyen nguoc--------------------------
        detaO=-W*z'; % Ham tac dong tuyen tinh
        W=W+nuy*detaO*z;
        for q=1:H
            netq=V(q,:)*X(:,k);
            detaHq=detaO*W(q)*5*exp(-0.25*netq)/(1+exp(-0.25*netq))^2;
            V(q,:)=V(q,:)+nuy*detaHq*X(:,k)';
        end;
        %----------------------------------------------------
    end;
    for q=1:H
        netq=V(q,:)*X(:,k);
        z(q)=sigmoid(netq);
    end;
    y=W*z';
    Jo=J;
    J=(y^2)/2;
    dJ=J-Jo;
    if dJ<0
        dnuy=0.01*nuy;
    elseif dJ>10
        dnuy=-0.1*nuy;
    else
        dnuy=0;
    end;
    epoch=epoch+1;
end;
%--------------------------------------------------------------------------
function y = sigmoid(x)
y=20/(1+exp(-0.25*x));