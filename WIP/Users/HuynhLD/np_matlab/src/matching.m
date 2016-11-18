function kq = matching(tem,db_tem,thres_P,thres_O)
% Ham doi sanh mau van tay "tem" (template) voi mot mau van tay trong
% database "db_tem" (database template). Mau phai co cau truc la mot
% mang 2 hang (hang 1 chua cac gia tri bien do, hang 2 chua cac gia tri
% goc cua cac diem dac trung) va N cot (N la so diem dac trung detect
% duoc tren anh van tay). Trong do 2 cot dau tien la vi tri cua hai diem
% delta (neu co).
% thres_po: nguong sai lech ve vi tri (position).
% thres_ph: nguong sai lech ve goc (orientation).
% kq: ket qua doi sanh - ty le phan tram su giong nhau
N_tem=length(tem(1,:));
N_dbtem=length(db_tem(1,:));
N=0; % So diem giong nhau
d_delta_right=sqrt((tem(1,1)-db_tem(1,1))^2+(tem(2,1)-db_tem(2,1))^2);
d_delta_left =sqrt((tem(1,2)-db_tem(1,2))^2+(tem(2,2)-db_tem(2,2))^2);
if d_delta_right<thres_P
    N=N+5; % Mot diem delta bang nam lan diem minutiae
end;
if d_delta_left<thres_P
    N=N+5; % Mot diem delta bang nam lan diem minutiae
end;
for i=3:N_tem
    dmin=500;
    jmin=0;
    for j=3:N_dbtem
        d=sqrt((tem(1,i)-db_tem(1,j))^2+(tem(2,i)-db_tem(2,j))^2);
        if dmin>d
            dmin=d;
            jmin=j;
        end;
    end;
    if (dmin<thres_P)&&(abs(tem(3,i)-db_tem(3,jmin))<thres_O)
        N=N+1; % Mot diem minutiae giong nhau
        db_tem(1,jmin)=0;
        db_tem(2,jmin)=0;
    end;
end;
kq=N/(N_dbtem+8)*100;