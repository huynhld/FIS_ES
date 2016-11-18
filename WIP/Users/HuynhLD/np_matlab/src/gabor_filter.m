function I_filted = gabor_filter(I,Wg,T,detax,detay)
% Ham loc anh I bang bo loc Gabor doi xung chan
% Wg: Kich thuoc cua bo loc Gabor
% T: Chu ky cua ham cos trong ham loc Gabor
% detax,detay: Cac he so deta cua ham loc Gabor
% I_filted: Anh sau khi loc
%I=normalize(I);
I=double(I);
[m, n]=size(I);
for h=11:Wg:m-Wg-9
    for k=11:Wg:n-Wg-9
        %------ Lay anh con tu anh chinh de loc --------
        p=0;
        for i=h-10:h+Wg+9
            p=p+1;
            q=0;
            for j=k-10:k+Wg+9
                q=q+1;
                I_sub(p,q)=I(i,j);
            end;
        end;
        %--------- Doi ra mien tan so va loc ---------
        O=tangentDir(I_sub,(Wg+11)/2,(Wg+11)/2,9);
        I_sub=fftshift(fft2(I_sub));
        G=fftshift(fft2(es_gabor(Wg+20,T,O,detax,detay)));
        G=abs(G);
        I_sub=abs(ifft2(fftshift(I_sub.*G)));
        %------ Thay anh con da duoc loc tro lai anh chinh ------
        p=10;
        for i=h:h+Wg-1
            p=p+1;
            q=10;
            for j=k:k+Wg-1
                q=q+1;
                I(i,j)=I_sub(p,q);
            end;
        end;
    end;
end;
I_filted=uint8(I);
%--------------------------------------------------------------------------
function g = es_gabor(Wg,T,O,detax,detay)
% Ham MTF (modulation transfer function) cua bo loc Gabor
for x=-(Wg-1)/2:(Wg-1)/2
    for y=-(Wg-1)/2:(Wg-1)/2
        xO=x*cos(O)-y*sin(O);
        yO=x*sin(O)+y*cos(O);
        g(x+(Wg+1)/2,y+(Wg+1)/2)=exp(-((xO/detax)^2+(yO/detay)^2)/2)*cos(2*pi*xO/T);
    end;
end;
%--------------------------------------------------------------------------
function phit = tangentDir(I,it,jt,anpha)
% Ham xac dinh phuong tiep tuyen phit tai diem (it,jt)
A=0;
B=0;
C=0;
for h=it-(anpha-1)/2:it+(anpha-1)/2
    for k=jt-(anpha-1)/2:jt+(anpha-1)/2
        ahk=(-I(h-1,k-1)+I(h-1,k+1)+I(h+1,k+1)-I(h+1,k-1))/4;
        bhk=(-I(h-1,k-1)-I(h-1,k+1)+I(h+1,k+1)+I(h+1,k-1))/4;
        A=A+ahk^2;
        B=B+bhk^2;
        C=C+ahk*bhk;
    end;
end;
if C>0
    t1=1;
    t2=(B-A)/(2*C)-sqrt(((B-A)/(2*C))^2+1);
elseif C<0
    t1=1;
    t2=(B-A)/(2*C)+sqrt(((B-A)/(2*C))^2+1);
elseif A<=B
    t1=1;
    t2=0;
else
    t1=0;
    t2=1;
end;
if t1==0
    phit=pi/2;
else
    phit=atan(t2/t1);
end;