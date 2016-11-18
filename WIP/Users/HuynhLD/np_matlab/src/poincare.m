function P = poincare(O,N,i,j)
% Ham tinh chi so poincare de xet diem Singularity tai diem (i,j)
% O: Truong dinh huong cua anh
% N (le): Kich thuoc "duong cong so" (curve) xem xet
a=(N-1)/2;
d=0;
do=O(i-a,j-a);
for k=j-a+1:j+a
    delta=O(i-a,k)-do;
    if delta<-pi/2
        delta=pi+delta;
    elseif delta>pi/2
        delta=delta-pi;
    end;
    d=d+delta;
    do=O(i-a,k);
end;
for h=i-a+1:i+a
    delta=O(h,j+a)-do;
    if delta<-pi/2
        delta=pi+delta;
    elseif delta>pi/2
        delta=delta-pi;
    end;
    d=d+delta;
    do=O(h,j+a);
end;
for k=j+a-1:-1:j-a
    delta=O(i+a,k)-do;
    if delta<-pi/2
        delta=pi+delta;
    elseif delta>pi/2
        delta=delta-pi;
    end;
    d=d+delta;
    do=O(i+a,k);
end;
for h=i+a-1:-1:i-a
    delta=O(h,j-a)-do;
    if delta<-pi/2
        delta=pi+delta;
    elseif delta>pi/2
        delta=delta-pi;
    end;
    d=d+delta;
    do=O(h,j-a);
end;
P=round(d*180/pi);