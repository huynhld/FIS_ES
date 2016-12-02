function W = training1(X,nuy,epsilon,D,n)
% Ham huan luyen mang neural perceptron don lop 16 ngo vao
% Ham tac dong la ham tuyen tinh
% X(N,K): ma tran cua cac vector mau ngo vao (K=1 mau, moi mau co N=16 gia tri)
% nuy: hang so hoc
% epsilon: nguong huan luyen
% W: cac vector trong so duoc huan luyen
%-------------- Tao gia tri khoi dong --------------------------
W=[0.1 0.1 1 0.1 0.1 1 0.1 0.1 1 0.1 0.1 1 -0.1 -0.1 -0.1 -0.1];
%---------------------------------------------------------------
epoch=0;
J=epsilon;
while (epoch<1000)&&(J>=epsilon)
    %----------Lan truyen thuan--------------------------
    net=W*X;
    Y=(-1)^n*net;
    %----------Lan truyen nguoc--------------------------
    W=W+(-1)^n*nuy*(D-Y)*X'; % Ham tac dong tuyen tinh
    %----------------------------------------------------
    Y=(-1)^n*W*X;
    J=((D-Y)^2)/2;
    epoch=epoch+1;
end;